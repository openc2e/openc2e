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

#include "prayfile/pray.h"
#include "exceptions.h"
#include "endianlove.h"
#include "spanstream.h"
#include <miniz.h>

prayFile::prayFile(fs::path filepath) {
	path = filepath;
	file.open(path.string().c_str(), std::ios::binary);
	if (!file.is_open())
		throw creaturesException(std::string("couldn't open PRAY file \"") + path.string() + "\"");
	
	char majic[4];
	file.read(majic, 4);
	if (strncmp(majic, "PRAY", 4) != 0)
		throw creaturesException(std::string("bad magic of PRAY file \"") + path.string() + "\"");

	while (!file.eof()) {
		// TODO: catch exceptions, and free all blocks before passing it up the stack
		prayFileBlock *b = new prayFileBlock(this);
		blocks.push_back(b);
		
		file.peek(); // make sure eof() gets set
	}
}

prayFile::~prayFile() {
	for (std::vector<prayFileBlock *>::iterator i = blocks.begin(); i != blocks.end(); i++) {
		delete *i;
	}
}

prayFileBlock::prayFileBlock(prayFile *p) {
	std::istream &file = p->getStream();

	char stringid[5]; stringid[4] = 0;
	file.read(stringid, 4);
	type = stringid;

	char nameid[129]; nameid[128] = 0;
	file.read(nameid, 128);
	name = nameid;

	compressedsize = read32le(file);
	size = read32le(file);
	unsigned int flags = read32le(file);
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

prayFileBlock::~prayFileBlock() {
	if (loaded)
		delete[] buffer;
}

void prayFileBlock::load() {
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
		if (!file.good()) {
			delete[] buffer;
			throw creaturesException("Failed to read all of compressed block.");
		}
		uLongf usize = size;
		int r = uncompress((Bytef *)buffer, (uLongf *)&usize, (Bytef *)src, compressedsize);
		if (r != Z_OK) {
			delete[] buffer; delete[] src;
			std::string o = "Unknown error";
			switch (r) {
				case Z_MEM_ERROR: o = "Out of memory"; break;
				case Z_BUF_ERROR: o = "Out of buffer space"; break;
				case Z_DATA_ERROR: o = "Corrupt data"; break;
			}
			o = o + " while decompressing PRAY block \"" + name + "\"";
			throw creaturesException(o);
		}
		delete[] src;
		if (usize != size) {
			delete[] buffer;
			throw creaturesException("Decompressed data is not the correct size.");
		}
	} else {
		file.read((char *)buffer, size);
		if (!file.good()) {
			delete[] buffer;
			throw creaturesException("Failed to read all of uncompressed block.");
		}
	}
	loaded = true;
}

std::string tagStringRead(std::istream& in) {
	unsigned int len = read32le(in);

	std::string data(len, '0');
	in.read(&data[0], len);

	return data;
}

void prayFileBlock::parseTags() {
	if (tagsloaded) return;

	if (!loaded)
		load();

	tagsloaded = true;

	spanstream s(buffer, size);

	unsigned int nointvalues = read32le(s);
	for (unsigned int i = 0; i < nointvalues; i++) {
		std::string n = tagStringRead(s);
		unsigned int v = read32le(s);

		if (integerValues.find(n) != integerValues.end())
			throw creaturesException(std::string("Duplicate tag \"") + n + "\"");
		integerValues[n] = v;
	}

	unsigned int nostrvalues = read32le(s);
	for (unsigned int i = 0; i < nostrvalues; i++) {
		std::string n = tagStringRead(s);
		std::string v = tagStringRead(s);
		if (stringValues.find(n) != stringValues.end()) // TODO: check integers too?
			throw creaturesException(std::string("Duplicate tag \"") + n + "\"");
		stringValues[n] = v;
	}
}

/* vim: set noet: */
