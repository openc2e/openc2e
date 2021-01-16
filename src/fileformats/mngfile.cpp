/*
 *  mngfile.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue 16 Nov 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
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
#include "mngfile.h"

#include "creaturesException.h"
#include "mngparser.h"
#include "utils/endianlove.h"
#include "utils/mmapifstream.h"
#include "utils/shared_array.h"

#include <algorithm>
#include <assert.h>
#include <fmt/core.h>

void decryptbuf(char* buf, int len) {
	int i;
	unsigned char pad = 5;
	for (i = 0; i < len; i++) {
		buf[i] ^= pad;
		pad += 0xC1;
	}
}

MNGFile::MNGFile(std::string n) {
	name = n;

	stream = new mmapifstream(n);
	if (!stream) {
		delete stream;
		throw MNGFileException("open failed");
	}

	// Read metavariables from beginning of file
	uint32_t numsamples = read32le(stream->map);
	uint32_t scriptoffset = read32le(stream->map + 4);
	uint32_t scriptlength = read32le(stream->map + 8);

	// read the samples
	for (size_t i = 0; i < numsamples; i++) {
		// Sample offsets and lengths are stored in pairs after the initial 16 bytes
		uint32_t position = read32le(stream->map + 12 + (8 * i));
		uint32_t size = read32le(stream->map + 16 + (8 * i));
		samples.push_back(shared_array<uint8_t>(stream->map + position, stream->map + position + size));
	}

	// now we have the samples, read and decode the MNG script
	script = std::string(stream->map + scriptoffset, stream->map + scriptoffset + scriptlength);
	// script = (char *) malloc(scriptlength + 1);
	// script[scriptlength] = 0;
	// if(! script) { delete stream; throw MNGFileException("malloc failed"); }
	// memcpy(script, stream->map + scriptoffset, scriptlength);
	decryptbuf(const_cast<char*>(script.c_str()), scriptlength);

	auto mngscript = mngparse(script);

	auto wave_names = mngscript.getWaveNames();
	for (size_t i = 0; i < wave_names.size(); ++i) {
		samplemappings[wave_names[i]] = i;
	}
}

std::vector<std::string> MNGFile::getSampleNames() const {
	return mngparse(script).getWaveNames();
}

unsigned int MNGFile::getSampleForName(std::string name) {
	if (samplemappings.find(name) == samplemappings.end()) {
		throw MNGFileException("unknown sample name"); // TODO: more info
	}
	return samplemappings[name];
}

MNGFile::~MNGFile() {
	delete stream;
}

/* vim: set noet: */
