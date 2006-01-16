/*
 *  pray.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon Jan 16 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

#include "pray.h"
#include "exceptions.h"
#include "endianlove.h"
#include "zlib.h"

prayFile::prayFile(fs::path filepath) {
	path = filepath;
	file.open(path.native_directory_string().c_str(), std::ios::binary);
	if (!file.is_open())
		throw creaturesException(std::string("couldn't open PRAY file ") + path.native_directory_string());
	
	char majic[4];
	file.read(majic, 4);
	if (strncmp(majic, "PRAY", 4) != 0)
		throw creaturesException(std::string("bad magic of PRAY file ") + path.native_directory_string());

	while (!file.eof()) {
		// TODO: catch exceptions, and free all blocks before passing it up the stack
		prayBlock *b = new prayBlock(this);
		blocks.push_back(b);
		
		file.peek(); // make sure eof() gets set
	}
}

prayFile::~prayFile() {
	for (std::vector<prayBlock *>::iterator i = blocks.begin(); i != blocks.end(); i++) {
		delete *i;
	}
}

prayManager::~prayManager() {
	for (std::vector<prayFile *>::iterator i = files.begin(); i != files.end(); i++) {
		delete *i;
	}

	assert(blocks.size() == 0);
}

void prayManager::update() {
	// TODO
}

prayBlock::prayBlock(prayFile *p) {
	std::istream &file = p->getStream();

	char stringid[5]; stringid[4] = 0;
	file.read(stringid, 4);
	type = stringid;

	char nameid[129]; nameid[128] = 0;
	file.read(nameid, 128);
	name = nameid;

	file.read((char *)&compressedsize, 4); compressedsize = swapEndianLong(compressedsize);
	file.read((char *)&size, 4); size = swapEndianLong(size);
	unsigned int flags;
	file.read((char *)&flags, 4); flags = swapEndianLong(flags);
	compressed = ((flags & 1) == 1);
	if (!compressed && size != compressedsize)
		throw creaturesException("Size doesn't match compressed size for uncompressed block.");

	// Skip the data for this block.
	offset = file.tellg();
	file.seekg(compressedsize, std::ios::cur);

	loaded = false;
	tagsloaded = false;
	buffer = 0;
	parent = p;
}

prayBlock::~prayBlock() {
	if (loaded)
		delete buffer;
}

void prayBlock::load() {
	std::istream &file = parent->getStream();

	file.clear();
	file.seekg(offset);
	if (!file.good())
		throw creaturesException("Failed to seek to block offset.");

	buffer = new unsigned char[size];
	if (compressed) {
		// TODO: check pray_uncompress_sanity_check
		char *src = new char[compressedsize];
		file.read(src, compressedsize);
		unsigned int usize;
		if (uncompress((Bytef *)buffer, (uLongf *)&usize, (Bytef *)src, size) != Z_OK) {
			delete buffer; delete src;
			throw creaturesException("Failed to decompress block.");
		}
		delete src;
		if (usize != size) {
			delete buffer;
			throw creaturesException("Decompressed data is not the correct size.");
		}
	} else {
		file.read((char *)buffer, size);
	}
	loaded = true;
}

std::string tagStringRead(unsigned char *&ptr) {
	unsigned int len = *(unsigned int *)ptr;
	len = swapEndianLong(len);
	ptr += 4;

	unsigned char *data = ptr;
	ptr += len;

	return std::string((char *)data, len);
}
	
void prayBlock::parseTags() {
	if (tagsloaded)
		throw creaturesException("Attempt to load tags when already loaded.");

	if (!loaded)
		load();

	tagsloaded = true;

	unsigned char *ptr = buffer;

	unsigned int nointvalues = swapEndianLong(*(unsigned int *)ptr); ptr += 4;

	for (unsigned int i = 0; i < nointvalues; i++) {
		std::string n = tagStringRead(ptr);
		unsigned int v = swapEndianLong(*(unsigned int *)ptr); ptr += 4;

		if (integerValues.find(n) != integerValues.end())
			throw creaturesException(std::string("Duplicate tag \"") + n + "\"");
		integerValues[n] = v;
	}

	unsigned int nostrvalues = swapEndianLong(*(unsigned int *)ptr); ptr += 4;

	for (unsigned int i = 0; i < nostrvalues; i++) {
		std::string n = tagStringRead(ptr);
		std::string v = tagStringRead(ptr);
		if (stringValues.find(n) != stringValues.end()) // TODO: check integers too?
			throw creaturesException(std::string("Duplicate tag \"") + n + "\"");
		stringValues[n] = v;
	}
}

/* vim: set noet: */
