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

c16Image::c16Image(std::istream &in) {
	uint32 flags; uint16 spritecount; bool is_565;
	in.read((char *)&flags, 4); flags = swapEndianLong(flags);
	is_565 = (flags & 0x01);
	// is_valid = (flags & 0x02);
	in.read((char *)&spritecount, 2); m_numframes = swapEndianShort(spritecount);
	
  widths = new unsigned short[m_numframes];
	heights = new unsigned short[m_numframes];
	buffers = new void *[m_numframes];
	unsigned int **lineoffsets = new unsigned int *[m_numframes];
	
	// first, read the headers.
	for (unsigned int i = 0; i < m_numframes; i++) {
		uint32 offset;
		in.read((char *)&offset, 4); offset = swapEndianLong(offset);
		in.read((char *)&widths[i], 2); widths[i] = swapEndianShort(widths[i]);
		in.read((char *)&heights[i], 2); heights[i] = swapEndianShort(heights[i]);
		lineoffsets[i] = new unsigned int[heights[i]];
		lineoffsets[i][0] = offset;
		for (unsigned int j = 1; j < heights[i]; j++) {
			in.read((char *)&lineoffsets[i][j], 4); lineoffsets[i][j] = swapEndianLong(lineoffsets[i][j]);
		}
	}
	
	// then, read the files. this involves seeking around, and is hence immensely ghey
	// todo: we assume the file format is valid here. we shouldn't.
	for (unsigned int i = 0; i < m_numframes; i++) {
		buffers[i] = new char[widths[i] * heights[i] * 2];
		uint16 *bufferpos = (uint16 *)buffers[i];
		for (unsigned int j = 0; j < heights[i]; j++) {
			in.seekg(lineoffsets[i][j], std::ios::beg);
			while (true) {
				uint16 tag; in.read((char *)&tag, 2); tag = swapEndianShort(tag);
				if (tag == 0) break;
				bool transparentrun = ((tag & 0x0001) == 0);
				uint16 runlength = (tag & 0xFFFE) >> 1;
				if (transparentrun)
					memset((char *)bufferpos, 0, (runlength * 2));
				else {
					in.read((char *)bufferpos, (runlength * 2));
					for (unsigned int k = 0; k < runlength; k++) {
						bufferpos[k] = swapEndianShort(bufferpos[k]);
					}
				}
				bufferpos += runlength;
			}
		}
		delete[] lineoffsets[i];
	}
	delete[] lineoffsets;
}
