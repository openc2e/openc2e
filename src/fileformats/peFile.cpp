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

#include "peFile.h"

#include "bmpImage.h"
#include "common/Exception.h"
#include "common/encoding.h"
#include "common/endianlove.h"
#include "common/io/SpanReader.h"

namespace fs = ghc::filesystem;

/*
 * This isn't a full PE parser, but it manages to extract resources from the
 * .exe files included with both Creatures and Creatures 2.
 */

struct peSection {
	uint32_t vaddr;
	uint32_t offset;
};

template <typename F>
void parseResourceDirectoryTable(Reader& file, F&& f) {
	(void)read32le(file); // characteristics
	(void)read32le(file); // timestamp
	(void)read16le(file); // version_major
	(void)read16le(file); // version_minor
	uint16_t nonamedentries = read16le(file);
	uint16_t noidentries = read16le(file);

	for (unsigned int i = 0; i < nonamedentries + noidentries; i++) {
		uint32_t name_or_id = read32le(file);
		uint32_t offset = read32le(file);
		f(name_or_id, offset);
	}
}

void parseResourceSection(Reader& file, const peSection& s, std::vector<resourceInfo>& resources) {
	// for a given .rsrc section, read: (1) the table of type directories,
	// (2) the table of name directories, (3) the table of language data
	// entries, (4) the data entries themselves.
	// these are typically laid out linearly in the file, which
	// is why we do each in batches rather than simply recursing.

	struct entry {
		uint32_t type;
		uint32_t name;
		uint32_t lang;
		uint32_t offset;
	};

	std::vector<entry> type_entries;
	std::vector<entry> name_entries;
	std::vector<entry> language_entries;

	file.seek_absolute(s.offset);
	parseResourceDirectoryTable(file, [&](auto type, auto offset) {
		type_entries.push_back({type, 0, 0, offset});
	});

	for (auto& e : type_entries) {
		file.seek_absolute(s.offset + (e.offset & 0x7fffffff));
		parseResourceDirectoryTable(file, [&](auto name, auto offset) {
			name_entries.push_back({e.type, name, 0, offset});
		});
	}

	for (auto& e : name_entries) {
		file.seek_absolute(s.offset + (e.offset & 0x7fffffff));
		parseResourceDirectoryTable(file, [&](auto lang, auto offset) {
			language_entries.push_back({e.type, e.name, lang, offset});
		});
	}

	for (auto& e : language_entries) {
		file.seek_absolute(s.offset + e.offset);

		uint32_t offset = read32le(file);
		offset += s.offset;
		offset -= s.vaddr;
		uint32_t size = read32le(file);

		(void)read32le(file); // codepage, always seems to be zero
		(void)read32le(file); // reserved

		resourceInfo info;
		info.type = (PeResourceType)e.type;
		info.name = e.name;
		info.lang = (PeLanguage)(e.lang & 0xff);
		info.sublang = (PeSubLanguage)(e.lang >> 10);
		info.offset = offset;
		info.size = size;

		resources.push_back(info);
	}
}

peFile::peFile(fs::path path)
	: file(path) {
	// check the signature of the file
	char majic[2];
	file.read(majic, 2);
	if (strncmp(majic, "MZ", 2) != 0)
		throw Exception(std::string("couldn't understand PE file \"") + path.string() + "\" (not a PE file?)");

	// skip the rest of the DOS header
	file.seek_relative(58);

	// read the location of the PE header
	uint32_t e_lfanew = read32le(file);
	if (e_lfanew == 0)
		throw Exception(std::string("couldn't understand PE file \"") + path.string() + "\" (DOS program?)");

	// seek to the PE header and check the signature
	file.seek_absolute(e_lfanew);
	char pemajic[4];
	file.read(pemajic, 4);
	if (memcmp(pemajic, "PE\0\0", 4) != 0)
		throw Exception(std::string("couldn't understand PE file \"") + path.string() + "\" (corrupt?)");

	// read the necessary data from the PE file header
	file.seek_relative(2);
	uint16_t nosections = read16le(file);
	file.seek_relative(12);
	uint16_t optionalheadersize = read16le(file);
	file.seek_relative(2);

	// skip the optional header
	file.seek_relative(optionalheadersize);

	// find .rsrc sections
	std::vector<peSection> sections;
	for (unsigned int i = 0; i < nosections; i++) {
		char section_name[9];
		section_name[8] = 0;
		file.read(section_name, 8);

		file.seek_relative(4);

		peSection section;
		section.vaddr = read32le(file);
		(void)read32le(file); // size
		section.offset = read32le(file);

		if (std::string(section_name) == ".rsrc") {
			sections.push_back(section);
		}

		file.seek_relative(16);
	}

	for (auto& s : sections) {
		parseResourceSection(file, s, resources);
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

shared_array<uint8_t> getResourceData(Reader& file, resourceInfo r) {
	shared_array<uint8_t> data(r.size);
	file.seek_absolute(r.offset);
	file.read((char*)data.data(), r.size);
	return data;
}

Image peFile::getBitmap(uint32_t name) {
	auto r = findResource(PE_RESOURCETYPE_BITMAP, PE_LANGUAGE_ENGLISH, name);
	if (!r)
		r = findResource(PE_RESOURCETYPE_BITMAP, PE_LANGUAGE_NEUTRAL, name);
	if (!r)
		return {};

	auto data = getResourceData(file, *r);
	SpanReader ss(data);
	Image bmp = ReadDibFile(ss);
	return bmp;
}

std::vector<std::string> peFile::getResourceStrings(resourceInfo r) {
	auto data = getResourceData(file, r);

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

std::string format_as(PeResourceType type) {
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
