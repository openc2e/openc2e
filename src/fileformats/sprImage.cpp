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
#include "endianlove.h"

MultiImage ReadSprFile(std::istream& in) {
	int numframes = read16le(in);
	MultiImage images(numframes);

	std::vector<uint32_t> offsets(numframes);
	for (int i = 0; i < numframes; i++) {
		offsets[i] = read32le(in);
		images[i].width = read16le(in);
		images[i].height = read16le(in);
		images[i].format = if_index8;
		images[i].palette = getCreatures1DefaultPalette();
	}

	// // check for Terra Nornia's corrupt background sprite
	// if (n == "buro") {
	// 	// apply stupid hack for corrupt offset tables in SPR files
	// 	// Only works if the file has 'normal' offsets we can predict, but this will only be called
	// 	// on known files anyway.
	// 	// TODO: can't we have a better check, eg checking if offsets are identical?
	// 	std::cout << "Applying hack for probably-corrupt Terra Nornia background." << std::endl;
	// 	unsigned int currpos = 2 + (8 * numframes);
	// 	for (unsigned int i = 0; i < numframes; i++) {
	// 		offsets[i] = currpos;
	// 		currpos += images[i].width * images[i].height;
	// 	}
	// }

	for (int i = 0; i < numframes; i++) {
		// TODO: don't seek
		in.seekg(offsets[i]);
		images[i].data = shared_array<uint8_t>(images[i].width * images[i].height);
		in.read(reinterpret_cast<char*>(images[i].data.data()), images[i].width * images[i].height);
	}

	return images;
}
