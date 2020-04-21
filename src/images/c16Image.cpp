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

void c16Image::readHeader(std::istream &in) {
	uint32_t flags; uint16_t spritecount;
	in.read((char *)&flags, 4); flags = swapEndianLong(flags);
	is_565 = (flags & 0x01);
	assert(flags & 0x02);
	in.read((char *)&spritecount, 2); m_numframes = swapEndianShort(spritecount);

	widths = new unsigned short[m_numframes];
	heights = new unsigned short[m_numframes];
	lineoffsets = new unsigned int *[m_numframes];

	// first, read the headers.
	for (unsigned int i = 0; i < m_numframes; i++) {
		uint32_t offset;
		in.read((char *)&offset, 4); offset = swapEndianLong(offset);
		in.read((char *)&widths[i], 2); widths[i] = swapEndianShort(widths[i]);
		in.read((char *)&heights[i], 2); heights[i] = swapEndianShort(heights[i]);
		lineoffsets[i] = new unsigned int[heights[i]];
		lineoffsets[i][0] = offset;
		for (unsigned int j = 1; j < heights[i]; j++) {
			in.read((char *)&lineoffsets[i][j], 4); lineoffsets[i][j] = swapEndianLong(lineoffsets[i][j]);
		}
	}
}

c16Image::c16Image(std::ifstream &in, std::string n) : creaturesImage(n) {
	imgformat = if_16bit;

	readHeader(in);
	
	buffers = new void *[m_numframes];
	
	// then, read the files. this involves seeking around, and is hence immensely ghey
	// todo: we assume the file format is valid here. we shouldn't.
	for (unsigned int i = 0; i < m_numframes; i++) {
		buffers[i] = new char[widths[i] * heights[i] * 2];
		uint16_t *bufferpos = (uint16_t *)buffers[i];
		for (unsigned int j = 0; j < heights[i]; j++) {
			in.seekg(lineoffsets[i][j], std::ios::beg);
			while (true) {
				uint16_t tag; in.read((char *)&tag, 2); tag = swapEndianShort(tag);
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
		delete[] lineoffsets[i];
	}
	delete[] lineoffsets;
}

void s16Image::readHeader(std::istream &in) {
	uint32_t flags; uint16_t spritecount;
	in.read((char *)&flags, 4); flags = swapEndianLong(flags);
	is_565 = (flags & 0x01);
	in.read((char *)&spritecount, 2); m_numframes = swapEndianShort(spritecount);
	
	widths = new unsigned short[m_numframes];
	heights = new unsigned short[m_numframes];
	offsets = new unsigned int[m_numframes];

	// first, read the headers.
	for (unsigned int i = 0; i < m_numframes; i++) {
		in.read((char *)&offsets[i], 4); offsets[i] = swapEndianLong(offsets[i]);
		in.read((char *)&widths[i], 2); widths[i] = swapEndianShort(widths[i]);
		in.read((char *)&heights[i], 2); heights[i] = swapEndianShort(heights[i]);
	}
}

bool s16Image::transparentAt(unsigned int frame, unsigned int x, unsigned int y) {
	unsigned int offset = (y * widths[frame]) + x;
	unsigned short *buffer = (unsigned short *)buffers[frame];
	return (buffer[offset] == 0);
}

bool c16Image::transparentAt(unsigned int frame, unsigned int x, unsigned int y) {
	unsigned int offset = (y * widths[frame]) + x;
	unsigned short *buffer = (unsigned short *)buffers[frame];
	return (buffer[offset] == 0);
}

s16Image::s16Image(std::ifstream &in, std::string n) : creaturesImage(n) {
	imgformat = if_16bit;

	readHeader(in);
	
	buffers = new void *[m_numframes];

	for (unsigned int i = 0; i < m_numframes; i++) {
		buffers[i] = new char[2 * widths[i] * heights[i]];
		readmany16le(in, (uint16_t*)buffers[i], widths[i] * heights[i]);
	}

	delete[] offsets;
}

s16Image::~s16Image() {
	delete[] widths;
	delete[] heights;
	for (unsigned int i = 0; i < m_numframes; i++) {
		delete[] (uint16_t *)buffers[i];
	}
	delete[] buffers;
	// TODO: we should never have 'offsets' left over here, but .. we should check
}

c16Image::~c16Image() {
	delete[] widths;
	delete[] heights;
	for (unsigned int i = 0; i < m_numframes; i++)
		delete[] (uint16_t *)buffers[i];
	delete[] buffers;
	// TODO: we should never have 'offsets' left over here, but .. we should check
}

/* vim: set noet: */
