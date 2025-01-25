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

#include "common/endianlove.h"
#include "common/io/Reader.h"
#include "common/throw_ifnot.h"

#include <memory>
#include <string.h>

MultiImage ReadC16File(Reader& in) {
	uint32_t flags = read32le(in);
	bool is_565 = (flags & 0x01);
	THROW_IFNOT(flags & 0x02);
	imageformat imgformat = is_565 ? if_rgb565 : if_rgb555;
	uint16_t numframes = read16le(in);

	MultiImage images(numframes);

	std::vector<std::vector<uint32_t>> lineoffsets(numframes);

	// first, read the headers.
	for (size_t i = 0; i < numframes; i++) {
		uint32_t offset = read32le(in);
		images[i].width = read16le(in);
		images[i].height = read16le(in);
		images[i].format = imgformat;
		images[i].colorkey = Color{0, 0, 0, 255};
		lineoffsets[i].resize(images[i].height);
		if (images[i].height > 0) {
			lineoffsets[i][0] = offset;
		}
		for (int j = 1; j < images[i].height; j++) {
			lineoffsets[i][j] = read32le(in);
		}
	}

	// then, read the image data.
	for (size_t i = 0; i < numframes; i++) {
		images[i].data = shared_array<uint8_t>(images[i].width * images[i].height * 2);
		uint16_t* bufferpos = (uint16_t*)images[i].data.data();
		for (int j = 0; j < images[i].height; j++) {
			if (lineoffsets[i][j] != in.tell()) {
				// TODO: log warning? we don't care but the official engine uses the
				// overall image offset to find the actual sprites, and uses the line
				// offsets for transparency lookups.
				fmt::print("WARNING: C16 line offset in header was {} but file position is actually {}\n",
					lineoffsets[i][j], in.tell());
			}
			while (true) {
				uint16_t tag = read16le(in);
				// TODO: check that this actually matches the correct line length
				if (tag == 0)
					break;
				bool transparentrun = ((tag & 0x0001) == 0);
				uint16_t runlength = (tag & 0xFFFE) >> 1;
				if (transparentrun) {
					memset((char*)bufferpos, 0, (runlength * 2));
				} else {
					readmany16le(in, bufferpos, runlength);
				}
				bufferpos += runlength;
			}
		}
		uint16_t endofimagemarker = read16le(in);
		THROW_IFNOT(endofimagemarker == 0);
	}

	return images;
}