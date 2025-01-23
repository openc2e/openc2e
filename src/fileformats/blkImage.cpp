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

#include "blkImage.h"

#include "common/endianlove.h"
#include "common/io/Reader.h"
#include "common/throw_ifnot.h"

Image ReadBlkFile(Reader& in) {
	uint32_t flags = read32le(in);
	bool is_565 = (flags & 0x01);
	imageformat imgformat = is_565 ? if_rgb565 : if_rgb555;

	uint16_t width_blocks = read16le(in);
	uint16_t height_blocks = read16le(in);
	uint16_t totalwidth = width_blocks * 128;
	uint16_t totalheight = height_blocks * 128;

	uint16_t numsprites = read16le(in);
	THROW_IFNOT(numsprites == (unsigned int)(width_blocks * height_blocks));

	std::vector<uint32_t> offsets(numsprites);
	for (unsigned int i = 0; i < numsprites; i++) {
		offsets[i] = read32le(in) + 4;
		uint16_t framewidth = read16le(in);
		THROW_IFNOT(framewidth == 128);
		uint16_t frameheight = read16le(in);
		THROW_IFNOT(frameheight == 128);
	}

	shared_array<uint8_t> buffer(totalwidth * totalheight * 2);

	for (auto i = 0; i < numsprites; ++i) {
		// TODO: make sure we're at the correct offset
		auto x = i / height_blocks;
		auto y = i % height_blocks;
		for (auto blocky = 0; blocky < 128; ++blocky) {
			auto start = (y * 128 + blocky) * totalwidth * 2 + x * 128 * 2;
			readmany16le(in, (uint16_t*)&buffer[start], 128);
		}
	}

	Image image;
	image.width = totalwidth;
	image.height = totalheight;
	image.format = imgformat;
	image.data = buffer;
	return image;
}
