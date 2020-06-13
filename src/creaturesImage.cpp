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

#include "creaturesImage.h"
#include "creaturesException.h"
#include <memory>

creaturesImage::creaturesImage(std::string name_, imageformat imgformat_,
                               std::vector<std::vector<unsigned char>> buffers_,
                               std::vector<uint16_t> widths_,
                               std::vector<uint16_t> heights_)
: m_numframes(buffers_.size()), widths(widths_), heights(heights_),
  buffers(buffers_), imgformat(imgformat_), name(name_)
{
	if (buffers.size() != widths.size() || buffers.size() != heights.size()) {
		throw creaturesException("Internal error: Tried to construct a creaturesImage but number of frames doesn't match");
	}
}

uint8_t *creaturesImage::getCustomPalette() {
	throw creaturesException("Internal error: Tried to get a custom palette of a sprite which doesn't support that.");
}
	
bool creaturesImage::transparentAt(unsigned int frame, unsigned int x, unsigned int y) {
	if (imgformat == if_16bit_565 || imgformat == if_16bit_555) {
		size_t offset = (y * widths[frame]) + x;
		uint16_t *buffer = (uint16_t *)buffers[frame].data();
		return buffer[offset] == 0;
	} else if (imgformat == if_paletted && !hasCustomPalette()) {
		size_t offset = (y * widths[frame]) + x;
		uint8_t *buffer = (uint8_t *)buffers[frame].data();
		return buffer[offset] == 0;
	}
	return false;
}

/* vim: set noet: */
