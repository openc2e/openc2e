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

#include "fileformats/blkImage.h"

#include "caos_assert.h"
#include "utils/endianlove.h"

#include <iostream>

Image ReadBlkFile(std::istream& in) {
	uint32_t flags = read32le(in);
	bool is_565 = (flags & 0x01);
	imageformat imgformat = is_565 ? if_rgb565 : if_rgb555;

	uint16_t width = read16le(in);
	uint16_t height = read16le(in);
	uint16_t totalwidth = width * 128;
	uint16_t totalheight = height * 128;

	uint16_t numsprites = read16le(in);
	caos_assert(numsprites == (unsigned int)(width * height));

	std::vector<uint32_t> offsets(numsprites);
	for (unsigned int i = 0; i < numsprites; i++) {
		offsets[i] = read32le(in) + 4;
		uint16_t framewidth = read16le(in);
		caos_assert(framewidth == 128);
		uint16_t frameheight = read16le(in);
		caos_assert(frameheight == 128);
	}

	shared_array<uint8_t> buffer(totalwidth * totalheight * 2);
	const size_t sprheight = 128, sprwidth = 128;
	const size_t heightinsprites = totalheight / sprheight;
	const size_t widthinsprites = totalwidth / sprwidth;

	for (size_t i = 0; i < heightinsprites; i++) {
		for (size_t j = 0; j < widthinsprites; j++) {
			const unsigned int whereweare = j * heightinsprites + i;
			// TODO: don't seek, it's slow
			in.seekg(offsets[whereweare]);
			for (int blocky = 0; blocky < 128; blocky++) {
				readmany16le(in, (uint16_t*)&(buffer[(i * 128 + blocky) * totalwidth * 2 + j * 128 * 2]), 128);
			}
		}
	}

	Image image;
	image.width = totalwidth;
	image.height = totalheight;
	image.format = imgformat;
	image.data = buffer;
	return image;
}
