/*
 *  c16Image.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat May 22 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#include "c16Image.h"
#include "endianlove.h"
#include "openc2e.h"
#include <cassert>
#include <memory>

c16Image::c16Image(std::ifstream &in, std::string n) : creaturesImage(n) {
	imgformat = if_16bit;

	uint32_t flags = read32le(in);
	is_565 = (flags & 0x01);
	assert(flags & 0x02);
	m_numframes = read16le(in);

	widths.resize(m_numframes);
	heights.resize(m_numframes);
	std::vector<std::vector<uint32_t>> lineoffsets(m_numframes);

	// first, read the headers.
	for (unsigned int i = 0; i < m_numframes; i++) {
		uint32_t offset = read32le(in);
		widths[i] = read16le(in);
		heights[i] = read16le(in);
		lineoffsets[i].resize(heights[i]);
		if (heights[i] > 0) {
			lineoffsets[i][0] = offset;
		}
		for (unsigned int j = 1; j < heights[i]; j++) {
			lineoffsets[i][j] = read32le(in);
		}
	}
	
	buffers.resize(m_numframes);
	
	// then, read the files. this involves seeking around, and is hence immensely ghey
	// todo: we assume the file format is valid here. we shouldn't.
	for (unsigned int i = 0; i < m_numframes; i++) {
		buffers[i].resize(widths[i] * heights[i] * 2);
		uint16_t *bufferpos = (uint16_t *)buffers[i].data();
		for (unsigned int j = 0; j < heights[i]; j++) {
			in.seekg(lineoffsets[i][j], std::ios::beg);
			while (true) {
				uint16_t tag = read16le(in);
				if (tag == 0) break;
				bool transparentrun = ((tag & 0x0001) == 0);
				uint16_t runlength = (tag & 0xFFFE) >> 1;
				if (transparentrun)
					memset((char *)bufferpos, 0, (runlength * 2));
				else {
					readmany16le(in, bufferpos, runlength);
				}
				bufferpos += runlength;
			}
		}
	}
}

bool s16Image::transparentAt(unsigned int frame, unsigned int x, unsigned int y) {
	unsigned int offset = (y * widths[frame]) + x;
	unsigned short *buffer = (unsigned short *)buffers[frame].data();
	return (buffer[offset] == 0);
}

bool c16Image::transparentAt(unsigned int frame, unsigned int x, unsigned int y) {
	unsigned int offset = (y * widths[frame]) + x;
	unsigned short *buffer = (unsigned short *)buffers[frame].data();
	return (buffer[offset] == 0);
}

s16Image::s16Image(std::ifstream &in, std::string n) : creaturesImage(n) {
	imgformat = if_16bit;

	uint32_t flags = read32le(in);
	is_565 = (flags & 0x01);
	m_numframes = read16le(in);

	widths.resize(m_numframes);
	heights.resize(m_numframes);
	std::vector<uint32_t> offsets(m_numframes);

	// first, read the headers.
	for (unsigned int i = 0; i < m_numframes; i++) {
		offsets[i] = read32le(in);
		widths[i] = read16le(in);
		heights[i] = read16le(in);
	}
	
	buffers.resize(m_numframes);

	for (unsigned int i = 0; i < m_numframes; i++) {
		buffers[i].resize(2 * widths[i] * heights[i]);
		readmany16le(in, (uint16_t*)buffers[i].data(), widths[i] * heights[i]);
	}
}

s16Image::~s16Image() {}

c16Image::~c16Image() {}

/* vim: set noet: */
