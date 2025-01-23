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

#include "common/Exception.h"
#include "common/endianlove.h"
#include "common/io/SpanReader.h"
#include "common/mappedfile.h"
#include "common/shared_array.h"
#include "mngparser.h"

#include <algorithm>
#include <assert.h>
#include <fmt/core.h>

static void decryptbuf(uint8_t* buf, size_t len) {
	unsigned char pad = 5;
	for (size_t i = 0; i < len; i++) {
		buf[i] ^= pad;
		pad += 0xC1;
	}
}

std::string mngdecrypt(const std::vector<uint8_t>& buf) {
	std::string s((char*)buf.data(), (char*)buf.data() + buf.size());
	decryptbuf((uint8_t*)&s[0], s.size());
	return s;
}

std::vector<uint8_t> mngencrypt(const std::string& s) {
	std::vector<uint8_t> buf((uint8_t*)s.data(), (uint8_t*)s.data() + s.size());
	decryptbuf(&buf[0], buf.size());
	return buf;
}

MNGFile::MNGFile() = default;

MNGFile::MNGFile(std::string n) {
	name = n;

	mappedfile m(n);
	SpanReader stream(m);

	// Read metavariables from beginning of file
	uint32_t numsamples = read32le(stream);
	uint32_t scriptoffset = read32le(stream);
	uint32_t scriptlength = read32le(stream);

	struct MNGSampleHeader {
		uint32_t position;
		uint32_t size;
	};
	std::vector<MNGSampleHeader> sample_headers(numsamples);

	// read the samples
	for (uint32_t i = 0; i < numsamples; i++) {
		// Sample offsets and lengths are stored in pairs after the initial 16 bytes
		sample_headers[i].position = read32le(stream);
		sample_headers[i].size = read32le(stream);
	}

	// read and decode the MNG script
	// TODO: warning if scriptoffset isn't in usual place?
	stream.seek_absolute(scriptoffset);
	std::vector<uint8_t> scrambled_script(scriptlength);
	stream.read(reinterpret_cast<char*>(scrambled_script.data()), scrambled_script.size());
	script = mngdecrypt(scrambled_script);

	auto mngscript = mngparse(script);

	auto wave_names = mngscript.getWaveNames();
	for (size_t i = 0; i < wave_names.size(); ++i) {
		samplemappings[wave_names[i]] = i;
	}

	// read the samples
	for (uint32_t i = 0; i < numsamples; i++) {
		// TODO: warning if sample isn't in expected place?
		stream.seek_absolute(sample_headers[i].position);
		shared_array<uint8_t> data(sample_headers[i].size + 16);
		memcpy(&data[0], "RIFF", 4);
		write32le(&data[4], sample_headers[i].size + 4);
		memcpy(&data[8], "WAVEfmt ", 8);
		stream.read(reinterpret_cast<char*>(&data[16]), sample_headers[i].size);
		samples.push_back(data);
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
}

/* vim: set noet: */
