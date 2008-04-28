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
#include "exceptions.h"
#include "streamutils.h"

// debug helper
std::string nameForType(uint32 t) {
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
	file.open(path.native_directory_string().c_str(), std::ios::binary);

	if (!file.is_open())
		throw creaturesException(std::string("couldn't open PE file \"") + path.native_directory_string() + "\"");

	// check the signature of the file
	char majic[2];
	file.read(majic, 2);
	if (strncmp(majic, "MZ", 2) != 0)
		throw creaturesException(std::string("couldn't understand PE file \"") + path.native_directory_string() + "\" (not a PE file?)");

	// skip the rest of the DOS header
	file.seekg(58, std::ios::cur);

	// read the location of the PE header
	uint32 e_lfanew = read32(file);
	if (e_lfanew == 0)
		throw creaturesException(std::string("couldn't understand PE file \"") + path.native_directory_string() + "\" (DOS program?)");

	// seek to the PE header and check the signature
	file.seekg(e_lfanew, std::ios::beg);
	char pemajic[4];
	file.read(pemajic, 4);
	if (memcmp(pemajic, "PE\0\0", 4) != 0)
		throw creaturesException(std::string("couldn't understand PE file \"") + path.native_directory_string() + "\" (corrupt?)");

	// read the necessary data from the PE file header
	file.seekg(2, std::ios::cur);
	uint16 nosections = read16(file);
	file.seekg(12, std::ios::cur);
	uint16 optionalheadersize = read16(file);
	file.seekg(2, std::ios::cur);

	// skip the optional header
	file.seekg(optionalheadersize, std::ios::cur);

	for (unsigned int i = 0; i < nosections; i++) {
		char section_name[9]; section_name[8] = 0;
		file.read(section_name, 8);
		
		file.seekg(4, std::ios::cur);

		peSection section;
		section.vaddr = read32(file);
		section.size = read32(file);
		section.offset = read32(file);
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
	
	uint16 nonamedentries = read16(file);
	uint16 noidentries = read16(file);

	for (unsigned int i = 0; i < nonamedentries + noidentries; i++) {
		uint32 name = read32(file);
		uint32 offset = read32(file);

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

			parseResourcesLevel(s, s.offset + offset & 0x7fffffff, level + 1);
		} else {
			/* bottom level, file data is here */

			file.seekg(s.offset + offset, std::ios::beg);
			
			uint32 offset = read32(file);
			offset += s.offset;
			offset -= s.vaddr;
			uint32 size = read32(file);
			
			/*if ((currlang & 0xff) == 0x09) { // LANG_ENGLISH
				file.seekg(offset, std::ios::beg);
				
				char *data = (char*)malloc(size);
				file.read(data, size);
	
				char buf[500];
				sprintf(buf, "/tmp/oh/%s_%d_%d_%d", nameForType(currtype).c_str(), currname, currlang, offset);
				std::ofstream f(buf);
				f.write(data, size);

				free(data);
			}*/
		}
		
		file.seekg(here, std::ios::beg);
	}
}

peFile::~peFile() {
}

/* vim: set noet: */
