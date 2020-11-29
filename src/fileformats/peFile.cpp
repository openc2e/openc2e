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

#include "creaturesException.h"
#include "endianlove.h"
#include "fileformats/bmpImage.h"
#include "fileformats/peFile.h"
#include "spanstream.h"

// debug helper
std::string nameForType(uint32_t t) {
	switch (t) {
		case 1: return "Cursor";
		case 2: return "Bitmap";
		case 3: return "Icon";
		case 4: return "Menu";
		case 5: return "Dialog";
		case 6: return "String";
		case 7: return "Fontdir";
		case 8: return "Font";
		case 9: return "Accelerator";
		case 10: return "RCData";
		case 11: return "MessageTable";
		case 16: return "Version";
	}

	return "Unknown";
}

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
	file.seekg(58, std::ios::cur);

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
	file.seekg(2, std::ios::cur);
	uint16_t nosections = read16le(file);
	file.seekg(12, std::ios::cur);
	uint16_t optionalheadersize = read16le(file);
	file.seekg(2, std::ios::cur);

	// skip the optional header
	file.seekg(optionalheadersize, std::ios::cur);

	for (unsigned int i = 0; i < nosections; i++) {
		char section_name[9]; section_name[8] = 0;
		file.read(section_name, 8);
		
		file.seekg(4, std::ios::cur);

		peSection section;
		section.vaddr = read32le(file);
		section.size = read32le(file);
		section.offset = read32le(file);
		sections[std::string(section_name)] = section;
		
		file.seekg(16, std::ios::cur);
	}

	parseResources();
}

void peFile::parseResources() {
	std::map<std::string, peSection>::iterator si = sections.find(std::string(".rsrc"));
	if (si == sections.end()) return;
	peSection &s = si->second;

	parseResourcesLevel(s, s.offset, 0);
}

unsigned int currtype, currname, currlang;

void peFile::parseResourcesLevel(peSection &s, unsigned int off, unsigned int level) {
	file.seekg(off, std::ios::beg);

	file.seekg(12, std::ios::cur);
	
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
			currlang = name;
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
			info.offset = offset;
			info.size = size;
			info.data = 0;

			//if ((currlang & 0xff) == 0x09) // LANG_ENGLISH
			resources[std::pair<uint32_t, uint32_t>(currtype, currlang)][currname] = info;
		}
		
		file.seekg(here, std::ios::beg);
	}
}

peFile::~peFile() {
	for (std::map<std::pair<uint32_t, uint32_t>, std::map<uint32_t, resourceInfo> >::iterator i = resources.begin(); i != resources.end(); i++) {
		for (std::map<uint32_t, resourceInfo>::iterator j = i->second.begin(); j != i->second.end(); j++) {
			if (j->second.data) {
				delete[] j->second.data;
			}
		}
	}
}

resourceInfo *peFile::getResource(uint32_t type, uint32_t lang, uint32_t name) {
	if (resources.find(std::pair<uint32_t, uint32_t>(type, lang)) == resources.end()) return 0;
	if (resources[std::pair<uint32_t, uint32_t>(type, lang)].find(name) == resources[std::pair<uint32_t, uint32_t>(type, lang)].end()) return 0;

	resourceInfo *r = &resources[std::pair<uint32_t, uint32_t>(type, lang)][name];
	if (!r->data) {
		file.seekg(r->offset, std::ios::beg);
		r->data = new char[r->size];
		file.read(r->data, r->size);
	}
	return r;
}

Image peFile::getBitmap(uint32_t name) {
	resourceInfo *r = getResource(PE_RESOURCETYPE_BITMAP, HORRID_LANG_ENGLISH, name);
	if (!r) r = getResource(PE_RESOURCETYPE_BITMAP, 0x400, name);
	if (!r) return {};
	
	spanstream ss(r->getData(), r->getSize());
	
	Image bmp = ReadDibFile(ss);
	return bmp;
}

std::vector<std::string> resourceInfo::parseStrings() {
	std::vector<std::string> strings;

	for (unsigned int i = 0; i < size / 2;) {
		uint16_t strsize = read16le(data + i * 2);
		i++;

		std::string s;
		for (unsigned int j = 0; (j < strsize) && (i < size / 2); j++) {
			uint16_t d = read16le(data + i * 2);
			s += (uint8_t)d; // TODO: convert properly from utf16, somehow
			i++;
		}
		strings.push_back(s);
	}

	return strings;
}

/* vim: set noet: */
