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

#include "sprImage.h"

#include "c1defaultpalette.h"
#include "common/Exception.h"
#include "common/endianlove.h"

#include <fmt/core.h>

static MultiImage ReadPrototypeSprFile(Reader& in, int numframes, uint32_t first_offset) {
	MultiImage images(numframes);

	std::vector<uint16_t> offsets(numframes);

	offsets[0] = first_offset & 0xffff;
	images[0].width = (first_offset & 0xff0000) >> 16;
	images[0].height = (first_offset & 0xff000000) >> 24;
	images[0].format = if_index8;
	images[0].colorkey = Color{0, 0, 0, 255};

	for (int i = 1; i < numframes; i++) {
		offsets[i] = read16le(in);
		images[i].width = read8(in);
		images[i].height = read8(in);
		images[i].format = if_index8;
		images[i].colorkey = Color{0, 0, 0, 255};
	}

	for (int i = 0; i < numframes; i++) {
		if (offsets[i] != in.tell()) {
			// we don't care about offsets but the official engine probably does
			fmt::print("WARNING: SPR image offset in header was {} but file position is actually {}\n",
				offsets[i], in.tell());
		}
		images[i].data = shared_array<uint8_t>(images[i].width * images[i].height);
		in.read(reinterpret_cast<char*>(images[i].data.data()), images[i].width * images[i].height);
	}

	return images;
}


SprFileData ReadSprFileWithMetadata(Reader& in, int32_t absolute_base, int32_t image_count) {
	uint16_t total_image_count = read16le(in);
	std::vector<int32_t> offsets(image_count);
	MultiImage images(image_count);

	if (absolute_base + image_count > total_image_count) {
		throw Exception(fmt::format("Tried to load {} images starting at {}, but file only has {} total",
			image_count, absolute_base, total_image_count));
	}

	// skip metadata up until absolute base
	// each metadata block is 8 bytes - 4 byte offset, 2 byte width, 2 byte height
	in.seek_relative(8 * absolute_base);

	// read metadata
	for (uint16_t i = 0; i < image_count; ++i) {
		offsets[i] = readsigned32le(in);
		images[i].width = read16le(in);
		images[i].height = read16le(in);
		images[i].format = if_index8;
		images[i].palette = getCreatures1DefaultPalette();
		images[i].colorkey = Color{0, 0, 0, 255};
	}

	// skip remaining metadata and images
	in.seek_absolute(offsets[0]);

	// now read actual data
	for (int32_t i = 0; i < image_count; i++) {
		if (static_cast<uint32_t>(offsets[i]) != in.tell()) {
			// we don't care about offsets but the official engine does
			fmt::print("WARNING: SPR image offset in header was {} but file position is actually {}\n",
				offsets[i], in.tell());
		}
		images[i].data = shared_array<uint8_t>(images[i].width * images[i].height);
		in.read(reinterpret_cast<char*>(images[i].data.data()), images[i].width * images[i].height);
	}
	return SprFileData{images, offsets};
}

MultiImage ReadSprFile(Reader& in) {
	int numframes = read16le(in);
	MultiImage images(numframes);

	std::vector<uint32_t> offsets(numframes);
	for (int i = 0; i < numframes; i++) {
		offsets[i] = read32le(in);
		// check if it's a prototype-style SPR file...
		if (i == 0 && offsets[0] >= 10000) {
			// TODO: less arbitrary number to check?
			return ReadPrototypeSprFile(in, numframes, offsets[0]);
		}
		images[i].width = read16le(in);
		images[i].height = read16le(in);
		images[i].format = if_index8;
		images[i].palette = getCreatures1DefaultPalette();
		images[i].colorkey = Color{0, 0, 0, 255};
	}

	// // check for Terra Nornia's corrupt background sprite
	// if (n == "buro") {
	// 	// apply stupid hack for corrupt offset tables in SPR files
	// 	// Only works if the file has 'normal' offsets we can predict, but this will only be called
	// 	// on known files anyway.
	// 	// TODO: can't we have a better check, eg checking if offsets are identical?
	// 	fmt::print("Applying hack for probably-corrupt Terra Nornia background.\n");
	// 	unsigned int currpos = 2 + (8 * numframes);
	// 	for (unsigned int i = 0; i < numframes; i++) {
	// 		offsets[i] = currpos;
	// 		currpos += images[i].width * images[i].height;
	// 	}
	// }

	for (int i = 0; i < numframes; i++) {
		if (offsets[i] != in.tell()) {
			// we don't care about offsets but the official engine does
			fmt::print("WARNING: SPR image offset in header was {} but file position is actually {}\n",
				offsets[i], in.tell());
		}
		images[i].data = shared_array<uint8_t>(images[i].width * images[i].height);
		in.read(reinterpret_cast<char*>(images[i].data.data()), images[i].width * images[i].height);
	}

	return images;
}
