/*
 *  peFile.h
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

#ifndef PEFILE_H
#define PEFILE_H

#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

#include "endianlove.h"

#define PE_RESOURCETYPE_BITMAP 2
#define PE_RESOURCETYPE_ICON 3
#define PE_RESOURCETYPE_STRING 6

// these are not actually a sensible way to do this, hence HORRID
#define HORRID_LANG_GERMAN 0x407
#define HORRID_LANG_ENGLISH 0x809 // also 0x409, 0x411
#define HORRID_LANG_FRENCH 0x40c
#define HORRID_LANG_ITALIAN 0x410
#define HORRID_LANG_DUTCH 0x413
#define HORRID_LANG_SPANISH 0xc0a

struct peSection {
	uint32 vaddr;
	uint32 offset;
	uint32 size;
};

class resourceInfo {
	friend class peFile;

	uint32 offset;
	uint32 size;
	char *data;

public:
	uint32 getSize() { return size; }
	char *getData() { return data; }
	std::vector<std::string> parseStrings();
};

class peFile {
protected:
	fs::path path;
	std::ifstream file;

	std::map<std::string, peSection> sections;
	std::map<std::pair<uint32, uint32>, std::map<uint32, resourceInfo> > resources;

	void parseResources();
	void parseResourcesLevel(peSection &s, unsigned int off, unsigned int level);

public:
	peFile(fs::path filepath);
	~peFile();

	resourceInfo *getResource(uint32 type, uint32 lang, uint32 name);
};

#endif
/* vim: set noet: */
