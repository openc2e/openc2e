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

std::vector<PrayBlockMetadata> readPrayMetadata(Reader& stream) {
	std::vector<PrayBlockMetadata> blocks;

	char majic[4];
	stream.read(majic, 4);
	if (strncmp(majic, "PRAY", 4) != 0)
		throw Exception("bad magic of PRAY file");

	while (true) {
		if (!stream.has_data_left()) {
			break;
		}

		PrayBlockMetadata block;
		block.offset = stream.tell();

		char type[5];
		type[4] = 0;
		stream.read(type, 4);
		block.type = type;

		char name[129];
		name[128] = 0;
		stream.read(name, 128);
		block.name = ensure_utf8(name);

		uint32_t compressed_size = read32le(stream);
		(void)read32le(stream); // size
		uint32_t flags = read32le(stream);
		block.is_compressed = flags & 1;

		blocks.push_back(block);
		stream.seek_relative(compressed_size);
	}
	return blocks;
}

PrayFileReader::PrayFileReader(Reader& stream_)
	: stream(stream_) {
	blocks = readPrayMetadata(stream);
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
	return blocks[i].is_compressed;
}

std::vector<uint8_t> readPrayBlockRawData(Reader& stream, const PrayBlockMetadata& block) {
	stream.seek_absolute(block.offset);

	char type[5];
	type[4] = 0;
	stream.read(type, 4);
	if (type != block.type) {
		throw Exception(fmt::format("Block type {:?} doesn't match expected type {:?}", type, block.type));
	}

	char name_[129];
	name_[128] = 0;
	stream.read(name_, 128);
	std::string name = ensure_utf8(name_);
	if (name != block.name) {
		throw Exception(fmt::format("Block name {:?} doesn't match expected name {:?}", name, block.name));
	}

	uint32_t compressedsize = read32le(stream);
	uint32_t size = read32le(stream);
	uint32_t flags = read32le(stream);
	bool compressed = ((flags & 1) == 1);
	// we could check is_compressed against the passed-in block metadata, but it doesn't really matter

	if (!compressed && size != compressedsize)
		throw Exception("Size doesn't match compressed size for uncompressed block.");

	std::vector<uint8_t> buffer(size);

	if (compressed) {
		// TODO: check pray_uncompress_sanity_check
		std::vector<uint8_t> src(compressedsize);
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

std::vector<uint8_t> PrayFileReader::getBlockRawData(size_t i) {
	return readPrayBlockRawData(stream, blocks[i]);
}

static std::string tagStringRead(Reader& in) {
	unsigned int len = read32le(in);

	std::string data(len, '0');
	in.read(&data[0], len);
	return data;
}

PrayTagBlock readPrayBlockTags(Reader& r, const PrayBlockMetadata& block) {
	std::map<std::string, uint32_t> integerValues;
	std::map<std::string, std::string> stringValues;

	auto buffer = readPrayBlockRawData(r, block);
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
		throw Exception("Stream failure reading tags from PRAY block \"" + block.name + "\"");
	}

	if (s.has_data_left()) {
		throw Exception("Didn't read whole block while reading tags from PRAY block \"" + block.name + "\"");
	}

	return {integerValues, stringValues};
}

std::pair<std::map<std::string, uint32_t>, std::map<std::string, std::string>> PrayFileReader::getBlockTags(size_t i) {
	return readPrayBlockTags(stream, blocks[i]);
}