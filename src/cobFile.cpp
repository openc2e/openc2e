/*
 *  cobFile.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Fri Jan 18 2008.
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

#include "cobFile.h"
#include "exceptions.h"
#include "endianlove.h"
#include <cassert>

cobFile::cobFile(fs::path filepath) {
	path = filepath;
	file.open(path.string().c_str(), std::ios::binary);

	if (!file.is_open())
		throw creaturesException(std::string("couldn't open COB file \"") + path.string() + "\"");

	// TODO: c1 cob support
	char majic[4];
	file.read(majic, 4);
	if (strncmp(majic, "cob2", 4) != 0)
		throw creaturesException(std::string("bad magic of C2 COB file \"") + path.string() + "\"");
	
	while (!file.eof()) {
		// TODO: catch exceptions, and free all blocks before passing it up the stack
		cobBlock *b = new cobBlock(this);
		blocks.push_back(b);

		file.peek(); // make sure eof() gets set
	}
}

cobFile::~cobFile() {
	for (std::vector<cobBlock *>::iterator i = blocks.begin(); i != blocks.end(); i++) {
		delete *i;
	}
}

cobBlock::cobBlock(cobFile *p) {
	std::istream &file = p->getStream();

	char cobtype[4];
	file.read(cobtype, 4);
	type = std::string(cobtype, 4);

	size = read32le(file);

	offset = file.tellg();
	file.seekg(size, std::ios::cur);
	
	loaded = false;
	buffer = 0;
	parent = p;
}

cobBlock::~cobBlock() {
	if (loaded)
		free();
}

void cobBlock::load() {
	assert(!loaded);
	std::istream &file = parent->getStream();
	
	file.clear();
	file.seekg(offset);
	if (!file.good())
		throw creaturesException("Failed to seek to block offset.");
	
	loaded = true;

	buffer = new unsigned char[size];
	file.read((char *)buffer, size);
	if (!file.good()) {
		free();
		throw creaturesException("Failed to read block.");
	}
}

void cobBlock::free() {
	assert(loaded);

	loaded = false;
	
	delete[] buffer;
	buffer = 0;
}

// TODO: argh, isn't there a better way to do this?
std::string readstring(std::istream &file) {
	unsigned int i = 0, n = 4096;
	char *buf = (char *)malloc(n);

	while (true) {
		file.read(&buf[i], 1);
		if (!file.good())
			throw creaturesException("Failed to read string.");

		// found null terminator
		if (buf[i] == 0) {
			std::string s = buf;
			free(buf);
			return s;
		}

		i++;

		// out of space?
		if (i == n) {
			n = n * 2;
			buf = (char *)realloc(buf, n);
		}
	}
}

cobAgentBlock::cobAgentBlock(cobBlock *p) {
	parent = p;
	std::istream &file = p->getParent()->getStream();

	file.clear();
	file.seekg(p->getOffset());
	if (!file.good())
		throw creaturesException("Failed to seek to block offset.");

	quantityremaining = read16le(file);
	lastusage = read32le(file);
	reuseinterval = read32le(file);
	usebyday = read8(file);
	usebymonth = read8(file);
	usebyyear = read32le(file);

	file.seekg(12, std::ios::cur); // unused

	name = readstring(file);
	description = readstring(file);
	installscript = readstring(file);
	removescript = readstring(file);

	unsigned short noevents = read16le(file);
	for (unsigned int i = 0; i < noevents; i++) {
		scripts.push_back(readstring(file));
	}

	unsigned short nodeps = read16le(file);
	for (unsigned int i = 0; i < nodeps; i++) {
		unsigned short deptype = read16le(file);
		deptypes.push_back(deptype);

		// depnames should be read as lower-case to ease comparison
		std::string depname = readstring(file);
		std::transform(depname.begin(), depname.end(), depname.begin(), (int(*)(int))tolower);
		depnames.push_back(depname);
	}

	thumbnailwidth = read16le(file);
	thumbnailheight = read16le(file);
	thumbnail = new unsigned short[thumbnailwidth * thumbnailheight];
	file.read((char *)thumbnail, 2 * thumbnailwidth * thumbnailheight);
}

cobAgentBlock::~cobAgentBlock() {
	delete[] thumbnail;
}

cobFileBlock::cobFileBlock(cobBlock *p) {
	parent = p;
	std::istream &file = p->getParent()->getStream();

	file.clear();
	file.seekg(p->getOffset());
	if (!file.good())
		throw creaturesException("Failed to seek to block offset.");

	filetype = read16le(file);
	file.seekg(4, std::ios::cur); // unused
	filesize = read32le(file);
		
	// filenames should be read as lower-case to ease comparison
	filename = readstring(file);
	std::transform(filename.begin(), filename.end(), filename.begin(), (int(*)(int))tolower);
}

cobFileBlock::~cobFileBlock() {
}

unsigned char *cobFileBlock::getFileContents() {
	if (!parent->isLoaded())
		parent->load();

	return parent->getBuffer() + 10 + filename.size() + 1;
}

/* vim: set noet: */
