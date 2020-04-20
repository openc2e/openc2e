/*
 *  blkImage.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
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

#include "openc2e.h"
#include "blkImage.h"
#include <cassert>
#include <iostream>

void blkImage::readHeader(std::istream &in) {
	uint32 flags; uint16 width, height, spritecount;
	in.read((char *)&flags, 4); flags = swapEndianLong(flags);
	is_565 = (flags & 0x01);
	in.read((char *)&width, 2); width = swapEndianShort(width);
	in.read((char *)&height, 2); height = swapEndianShort(height);
	totalwidth = width * 128; totalheight = height * 128;
	in.read((char *)&spritecount, 2); m_numframes = swapEndianShort(spritecount);
	
	assert(m_numframes == (unsigned int) (width * height));	

	widths = new uint16[m_numframes];
	heights = new uint16[m_numframes];
	offsets = new uint32[m_numframes];

	for (unsigned int i = 0; i < m_numframes; i++) {
		in.read((char *)&offsets[i], 4); offsets[i] = swapEndianLong(offsets[i]) + 4;
		in.read((char *)&widths[i], 2); widths[i] = swapEndianShort(widths[i]); assert(widths[i] == 128);
		in.read((char *)&heights[i], 2); heights[i] = swapEndianShort(heights[i]); assert(heights[i] == 128);
	}
}

void blkImage::writeHeader(std::ostream &s) {
	unsigned int dw; unsigned short w;
	
	dw = (is_565 ? 1 : 0);
	dw = swapEndianLong(dw); s.write((char *)&dw, 4);
	w = totalwidth / 128; assert((unsigned int)(w * 128) == totalwidth);
	w = swapEndianShort(w); s.write((char *)&w, 2);
	w = totalheight / 128; assert((unsigned int)(w * 128) == totalheight);
	w = swapEndianShort(w); s.write((char *)&w, 2);
	w = m_numframes; assert(m_numframes == (unsigned int) ((totalwidth / 128) * (totalheight / 128)));
	w = swapEndianShort(w); s.write((char *)&w, 2);

	for (unsigned int i = 0; i < m_numframes; i++) {
		dw = offsets[i] - 4;
		dw = swapEndianLong(dw); s.write((char *)&dw, 4);
		w = widths[i]; assert(w == 128);
		w = swapEndianShort(w); s.write((char *)&w, 2);
		w = heights[i]; assert(w == 128);
		w = swapEndianShort(w); s.write((char *)&w, 2);
	}
}

blkImage::blkImage(std::ifstream *in, std::string n) : creaturesImage(n) {
	stream = in;
	imgformat = if_16bit;

	readHeader(*in);
	
	buffers = new void *[m_numframes];
	
	for (unsigned int i = 0; i < m_numframes; i++) {
		buffers[i] = new char[2 * widths[i] * heights[i]];
		in->seekg(offsets[i]);
		in->read((char*)buffers[i], 2 * widths[i] * heights[i]);
	}
}

blkImage::~blkImage() {
	delete[] widths;
	delete[] heights;
	for (unsigned int i = 0; i < m_numframes; i++) {
		delete[] (char*)buffers[i];
	}
	delete[] buffers;
	delete[] offsets;
}

/* vim: set noet: */
