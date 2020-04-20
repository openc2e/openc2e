/*
 *  sprImage.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Nov 19 2006.
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

#include "openc2e.h"
#include "sprImage.h"
#include <iostream>

sprImage::sprImage(mmapifstream *in, std::string n) : creaturesImage(n) {
	stream = in;
	imgformat = if_paletted;

	uint16 spritecount;
	in->read((char *)&spritecount, 2); m_numframes = swapEndianShort(spritecount);

	widths = new uint16[m_numframes];
	heights = new uint16[m_numframes];
	offsets = new uint32[m_numframes];
	buffers = new void *[m_numframes];

	for (unsigned int i = 0; i < m_numframes; i++) {
		in->read((char *)&offsets[i], 4); offsets[i] = swapEndianLong(offsets[i]);
		in->read((char *)&widths[i], 2); widths[i] = swapEndianShort(widths[i]);
		in->read((char *)&heights[i], 2); heights[i] = swapEndianShort(heights[i]);
		buffers[i] = in->map + offsets[i];
	}

	// check for Terra Nornia's corrupt background sprite
	if (n == "buro") {
		// apply stupid hack for corrupt offset tables in SPR files
		// Only works if the file has 'normal' offsets we can predict, but this will only be called
		// on known files anyway.
		// TODO: can't we have a better check, eg checking if offsets are identical?
		std::cout << "Applying hack for probably-corrupt Terra Nornia background." << std::endl;
		unsigned int currpos = 2 + (8 * m_numframes);
		for (unsigned int i = 0; i < m_numframes; i++) {
			buffers[i] = in->map + currpos;
			currpos += widths[i] * heights[i];
		}
	}
}

sprImage::~sprImage() {
	delete[] widths;
	delete[] heights;
	delete[] buffers;
	delete[] offsets;
}

bool sprImage::transparentAt(unsigned int frame, unsigned int x, unsigned int y) {
	unsigned int offset = (y * widths[frame]) + x;
	unsigned char *buffer = (unsigned char *)buffers[frame];
	return (buffer[offset] == 0);
}

/* vim: set noet: */
