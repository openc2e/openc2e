/*
 *  creaturesImage.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Jun 06 2004.
 *  Copyright (c) 2004-2008 Alyssa Milburn. All rights reserved.
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

#include "common/creaturesImage.h"

#include "common/Exception.h"

#include <memory>
#include <stdio.h>

creaturesImage::creaturesImage(std::string n) {
	name = n;
}

imageformat creaturesImage::format(unsigned int frame) const {
	return images[frame].format;
}

unsigned int creaturesImage::numframes() const {
	if (block_width || block_height) {
		auto val = images[0].width / block_width * images[0].height / block_height;
		return val;
	}
	return images.size();
}

unsigned int creaturesImage::width(unsigned int frame) const {
	if (block_width) {
		return block_width;
	}
	return images[frame].width;
}

unsigned int creaturesImage::height(unsigned int frame) const {
	if (block_height) {
		return block_height;
	}
	return images[frame].height;
}

const void* creaturesImage::data(unsigned int frame) const {
	return images[frame].data.data();
}

std::string creaturesImage::getName() const {
	return name;
}

bool creaturesImage::hasCustomPalette(unsigned int frame) const {
	return images[frame].palette.data() != nullptr;
}
shared_array<Color> creaturesImage::getCustomPalette(unsigned int frame) const {
	return images[frame].palette;
}

const Image& creaturesImage::getImageForFrame(unsigned int frame) const {
	if (block_width || block_height) {
		return images[0];
	}
	return images[frame];
}

Texture& creaturesImage::getTextureForFrame(unsigned int frame) {
	if (block_width || block_height) {
		if (textures.size() <= 0)
			textures.resize(1);
		return textures[0];
	}
	if (textures.size() <= frame)
		textures.resize(frame + 1);
	return textures[frame];
}

unsigned int creaturesImage::getXOffsetForFrame(unsigned int frame) const {
	if (block_width) {
		return block_width * (frame % (images[0].width / block_width));
	}
	return 0;
}

unsigned int creaturesImage::getYOffsetForFrame(unsigned int frame) const {
	if (block_width && block_height) {
		return block_height * (frame / (images[0].width / block_width));
	}
	return 0;
}

bool creaturesImage::transparentAt(unsigned int frame, unsigned int x, unsigned int y) const {
	if (block_width || block_height) {
		// TODO: implement this when we figure out to get seamonkeys past the load screen
		return false;
	}

	// Docking Station's snotrock triggers this, as does something in Creatures Village.
	// TODO: why does it happen??
	if (x >= images[frame].width || y >= images[frame].height)
		return true;

	imageformat imgformat = format(frame);
	if (imgformat == if_rgb565 || imgformat == if_rgb555) {
		size_t offset = (y * width(frame)) + x;
		uint16_t* buffer = (uint16_t*)data(frame);
		return buffer[offset] == 0;
	} else if (imgformat == if_index8 && !hasCustomPalette(frame)) {
		size_t offset = (y * width(frame)) + x;
		uint8_t* buffer = (uint8_t*)data(frame);
		return buffer[offset] == 0;
	}
	return false;
}

void creaturesImage::setBlockSize(unsigned int width, unsigned int height) {
	if (images.size() != 1) {
		throw Exception("Can't set block size on image with more than one frame");
	}
	if ((block_width || block_height) && (block_width != width || block_height != height)) {
		printf("Warning: setting block size %ix%i on image '%s' that already has block size %ix%i\n",
			width, height, name.c_str(), block_width, block_height);
	}
	block_width = width;
	block_height = height;
}

/* vim: set noet: */
