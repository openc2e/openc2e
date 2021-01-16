/*
 *  peFile.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon Apr 28 2008.
 *  Copyright (c) 2008 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "fileformats/peFile.h"

#include "creaturesException.h"
#include "fileformats/bmpImage.h"
#include "utils/encoding.h"
#include "utils/endianlove.h"
#include "utils/spanstream.h"

/*
 * This isn't a full PE parser, but it manages to extract resources from the
 * .exe files included with both Creatures and Creatures 2.
 */

peFile::peFile(fs::path filepath) {
	path = filepath;
	file.open(path.string().c_str(), std::ios::binary);

	if (!file.is_open())
		throw creaturesException(std::string("couldn't open PE file \"") + path.string() + "\"");

	// check the signature of the file
	char majic[2];
	file.read(majic, 2);
	if (strncmp(majic, "MZ", 2) != 0)
		throw creaturesException(std::string("couldn't understand PE file \"") + path.string() + "\" (not a PE file?)");

	// skip the rest of the DOS header
	file.ignore(58);

	// read the location of the PE header
	uint32_t e_lfanew = read32le(file);
	if (e_lfanew == 0)
		throw creaturesException(std::string("couldn't understand PE file \"") + path.string() + "\" (DOS program?)");

	// seek to the PE header and check the signature
	file.seekg(e_lfanew, std::ios::beg);
	char pemajic[4];
	file.read(pemajic, 4);
	if (memcmp(pemajic, "PE\0\0", 4) != 0)
		throw creaturesException(std::string("couldn't understand PE file \"") + path.string() + "\" (corrupt?)");

	// read the necessary data from the PE file header
	file.ignore(2);
	uint16_t nosections = read16le(file);
	file.ignore(12);
	uint16_t optionalheadersize = read16le(file);
	file.ignore(2);

	// skip the optional header
	file.ignore(optionalheadersize);

	std::map<std::string, peSection> sections;
	for (unsigned int i = 0; i < nosections; i++) {
		char section_name[9];
		section_name[8] = 0;
		file.read(section_name, 8);

		file.ignore(4);

		peSection section;
		section.vaddr = read32le(file);
		section.size = read32le(file);
		section.offset = read32le(file);
		sections[std::string(section_name)] = section;

		file.ignore(16);
	}

	// parse resources
	auto si = sections.find(std::string(".rsrc"));
	if (si == sections.end())
		return;
	peSection& s = si->second;

	parseResourcesLevel(s, s.offset, 0);
}

void peFile::parseResourcesLevel(peSection& s, unsigned int off, unsigned int level) {
	file.seekg(off, std::ios::beg);

	file.ignore(12);

	uint16_t nonamedentries = read16le(file);
	uint16_t noidentries = read16le(file);

	for (unsigned int i = 0; i < nonamedentries + noidentries; i++) {
		uint32_t name = read32le(file);
		uint32_t offset = read32le(file);

		unsigned int here = file.tellg();

		if (level == 0) {
			currtype = name;
		} else if (level == 1) {
			/* we don't check for strings here because we don't care :) */
			currname = name;
		} else if (level == 2) {
			currlang = name & 0xff;
			currsublang = name >> 10;
		}

		if (level < 2) {
			/* another level, more horror */

			parseResourcesLevel(s, s.offset + (offset & 0x7fffffff), level + 1);
		} else {
			/* bottom level, file data is here */

			file.seekg(s.offset + offset, std::ios::beg);

			uint32_t offset = read32le(file);
			offset += s.offset;
			offset -= s.vaddr;
			uint32_t size = read32le(file);

			resourceInfo info;
			info.type = (PeResourceType)currtype;
			info.lang = (PeLanguage)currlang;
			info.sublang = (PeSubLanguage)currsublang;
			info.name = currname;
			info.offset = offset;
			info.size = size;

			// if ((currlang & 0xff) == 0x09) // LANG_ENGLISH
			resources.push_back(info);
		}

		file.seekg(here, std::ios::beg);
	}
}

peFile::~peFile() {
}

optional<resourceInfo> peFile::findResource(PeResourceType type, PeLanguage lang, uint32_t name) {
	optional<resourceInfo> best_match;
	for (auto& r : resources) {
		if (!(r.type == type && r.lang == lang && r.name == name)) {
			continue;
		}
		if (r.sublang == PE_SUBLANG_DEFAULT) {
			best_match = r;
			break;
		}
		if (!best_match) {
			best_match = r;
		}
	}
	return best_match;
}

optional<resourceInfo> peFile::findResource(
	PeResourceType type, PeLanguage lang, PeSubLanguage sublang, uint32_t name) {
	for (auto& r : resources) {
		if (!(r.type == type && r.lang == lang && r.sublang == sublang && r.name == name)) {
			continue;
		}
		return r;
	}
	return {};
}

shared_array<uint8_t> peFile::getResourceData(resourceInfo r) {
	shared_array<uint8_t> data(r.size);
	file.seekg(r.offset, std::ios::beg);
	file.read((char*)data.data(), r.size);
	return data;
}

Image peFile::getBitmap(uint32_t name) {
	auto r = findResource(PE_RESOURCETYPE_BITMAP, PE_LANGUAGE_ENGLISH, name);
	if (!r)
		r = findResource(PE_RESOURCETYPE_BITMAP, PE_LANGUAGE_NEUTRAL, name);
	if (!r)
		return {};

	auto data = getResourceData(*r);
	spanstream ss(data.data(), data.size());
	Image bmp = ReadDibFile(ss);
	return bmp;
}

std::vector<std::string> peFile::getResourceStrings(resourceInfo r) {
	auto data = getResourceData(r);

	std::vector<std::string> strings;

	uint8_t* p = data.data();
	while (p < data.data() + data.size()) {
		uint16_t strsize = read16le(p);
		p += 2;
		strings.push_back(utf16le_to_utf8(p, strsize * 2));
		p += strsize * 2;
	}

	return strings;
}

std::string peFile::resource_type_to_string(PeResourceType type) {
	switch (type) {
		case PE_RESOURCETYPE_CURSOR: return "cursor";
		case PE_RESOURCETYPE_BITMAP: return "bitmap";
		case PE_RESOURCETYPE_ICON: return "icon";
		case PE_RESOURCETYPE_MENU: return "menu";
		case PE_RESOURCETYPE_DIALOG: return "dialog";
		case PE_RESOURCETYPE_STRING: return "string";
		case PE_RESOURCETYPE_ACCELERATOR: return "accelerator";
		case PE_RESOURCETYPE_GROUP_CURSOR: return "group_cursor";
		case PE_RESOURCETYPE_GROUP_ICON: return "group_icon";
		case PE_RESOURCETYPE_VERSION: return "version";
		case PE_RESOURCETYPE_TOOLBAR: return "toolbar";
	}
	return std::to_string((uint32_t)type);
}

std::string peFile::language_to_string(PeLanguage lang, PeSubLanguage sublang) {
	switch (lang) {
		case PE_LANGUAGE_NEUTRAL:
			if (sublang == PE_SUBLANG_DEFAULT) {
				return "neutral";
			}
			return "neutral-sublang" + std::to_string((uint32_t)sublang);
		case PE_LANGUAGE_GERMAN:
			if (sublang == PE_SUBLANG_GERMAN) {
				return "de";
			}
			return "de-sublang" + std::to_string((uint32_t)sublang);
		case PE_LANGUAGE_ENGLISH:
			if (sublang == PE_SUBLANG_ENGLISH_US) {
				return "en-US";
			}
			if (sublang == PE_SUBLANG_ENGLISH_UK) {
				return "en-GB";
			}
			return "en-sublang" + std::to_string((uint32_t)sublang);
		case PE_LANGUAGE_FRENCH:
			if (sublang == PE_SUBLANG_FRENCH) {
				return "fr";
			}
			return "fr-sublang" + std::to_string((uint32_t)sublang);
		case PE_LANGUAGE_ITALIAN:
			if (sublang == PE_SUBLANG_ITALIAN) {
				return "it";
			}
			return "it-sublang" + std::to_string((uint32_t)sublang);
		case PE_LANGUAGE_DUTCH:
			if (sublang == PE_SUBLANG_DUTCH) {
				return "nl";
			}
			return "nl-sublang" + std::to_string((uint32_t)sublang);
		case PE_LANGUAGE_JAPANESE:
			if (sublang == PE_SUBLANG_JAPANESE) {
				return "ja";
			}
			return "ja-sublang" + std::to_string((uint32_t)sublang);
		case PE_LANGUAGE_SPANISH:
			if (sublang == PE_SUBLANG_SPANISH_MODERN) {
				return "es";
			}
			if (sublang == PE_SUBLANG_SPANISH) {
				return "es-ES";
			}
			return "es-sublang" + std::to_string((uint32_t)sublang);
	}
	return "lang" + std::to_string((uint32_t)lang) + "-sublang" + std::to_string((uint32_t)sublang);
}

/* vim: set noet: */
