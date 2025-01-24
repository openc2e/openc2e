/*
 *  PrayFileReader.cpp
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

#include "PrayFileReader.h"

#include "common/Exception.h"
#include "common/encoding.h"
#include "common/endianlove.h"
#include "common/io/IOException.h"
#include "common/io/SpanReader.h"

#include <cstring>
#include <zlib.h>

PrayFileReader::PrayFileReader(Reader& stream_)
	: stream(stream_) {
	char majic[4];
	stream.read(majic, 4);
	if (strncmp(majic, "PRAY", 4) != 0)
		throw Exception("bad magic of PRAY file");

	while (true) {
		if (!stream.has_data_left()) {
			break;
		}

		prayFileBlock block;
		block.offset = stream.tell();

		char type[5];
		type[4] = 0;
		stream.read(type, 4);
		block.type = type;

		char name[129];
		name[128] = 0;
		stream.read(name, 128);
		block.name = ensure_utf8(name);

		block.compressed_size = read32le(stream);
		block.size = read32le(stream);
		block.flags = read32le(stream);

		blocks.push_back(block);
		stream.seek_relative(block.compressed_size);
	}
}

PrayFileReader::~PrayFileReader() {
}

size_t PrayFileReader::getNumBlocks() {
	return blocks.size();
}

std::string PrayFileReader::getBlockType(size_t i) {
	return blocks[i].type;
}

std::string PrayFileReader::getBlockName(size_t i) {
	return blocks[i].name;
}

bool PrayFileReader::getBlockIsCompressed(size_t i) {
	return (blocks[i].flags & 1) == 1;
}

std::vector<unsigned char> PrayFileReader::getBlockRawData(size_t i) {
	std::string name = getBlockName(i);

	stream.seek_absolute(blocks[i].offset + 144);
	uint32_t compressedsize = blocks[i].compressed_size;
	uint32_t size = blocks[i].size;
	uint32_t flags = blocks[i].flags;
	bool compressed = ((flags & 1) == 1);

	if (!compressed && size != compressedsize)
		throw Exception("Size doesn't match compressed size for uncompressed block.");

	std::vector<unsigned char> buffer(size);

	if (compressed) {
		// TODO: check pray_uncompress_sanity_check
		std::vector<unsigned char> src(compressedsize);
		try {
			stream.read(src);
		} catch (const IOException&) {
			throw Exception("Failed to read all of compressed block.");
		}
		uLongf usize = size;
		int r = uncompress((Bytef*)buffer.data(), (uLongf*)&usize, (Bytef*)src.data(), compressedsize);
		if (r != Z_OK) {
			std::string o = "Unknown error";
			switch (r) {
				case Z_MEM_ERROR: o = "Out of memory"; break;
				case Z_BUF_ERROR: o = "Out of buffer space"; break;
				case Z_DATA_ERROR: o = "Corrupt data"; break;
			}
			o = o + " while decompressing PRAY block \"" + name + "\"";
			throw Exception(o);
		}
		if (usize != size) {
			throw Exception("Decompressed data is not the correct size.");
		}
	} else {
		try {
			stream.read(buffer);
		} catch (const IOException&) {
			throw Exception("Failed to read all of uncompressed block.");
		}
	}

	return buffer;
}

static std::string tagStringRead(Reader& in) {
	unsigned int len = read32le(in);

	std::string data(len, '0');
	in.read(&data[0], len);
	return data;
}

std::pair<std::map<std::string, uint32_t>, std::map<std::string, std::string>> PrayFileReader::getBlockTags(size_t i) {
	std::string name = getBlockName(i);

	std::map<std::string, uint32_t> integerValues;
	std::map<std::string, std::string> stringValues;

	auto buffer = getBlockRawData(i);
	SpanReader s(buffer);

	try {
		unsigned int nointvalues = read32le(s);
		for (unsigned int i = 0; i < nointvalues; i++) {
			std::string n = ensure_utf8(tagStringRead(s));
			unsigned int v = read32le(s);
			if (integerValues.find(n) == integerValues.end()) {
				integerValues[n] = v;
			} else if (integerValues[n] != v) {
				throw Exception(std::string("Duplicate tag \"") + n + "\"");
			}
		}

		unsigned int nostrvalues = read32le(s);
		for (unsigned int i = 0; i < nostrvalues; i++) {
			std::string n = ensure_utf8(tagStringRead(s));
			std::string v = ensure_utf8(tagStringRead(s));
			if (stringValues.find(n) == stringValues.end()) {
				stringValues[n] = v;
			} else if (stringValues[n] != v) {
				throw Exception(std::string("Duplicate tag \"") + n + "\"");
			}
		}
	} catch (const IOException&) {
		throw Exception("Stream failure reading tags from PRAY block \"" + name + "\"");
	}

	if (s.has_data_left()) {
		throw Exception("Didn't read whole block while reading tags from PRAY block \"" + name + "\"");
	}

	return {integerValues, stringValues};
}
