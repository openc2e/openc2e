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
#include "endianlove.h"
#include <cassert>
#include <iostream>

blkImage::blkImage(std::ifstream &in, std::string n) : creaturesImage(n) {
	imgformat = if_16bit;

	uint32_t flags = read32le(in);
	is_565 = (flags & 0x01);
	uint16_t width = read16le(in);
	uint16_t height = read16le(in);
	totalwidth = width * 128; totalheight = height * 128;
	m_numframes = read16le(in);
	
	assert(m_numframes == (unsigned int) (width * height));	

	widths.resize(m_numframes);
	heights.resize(m_numframes);
	std::vector<uint32_t> offsets(m_numframes);

	for (unsigned int i = 0; i < m_numframes; i++) {
		offsets[i] = read32le(in);
		widths[i] = read16le(in); assert(widths[i] == 128);
		heights[i] = read16le(in); assert(heights[i] == 128);
	}
	
	buffers.resize(m_numframes);
	
	for (unsigned int i = 0; i < m_numframes; i++) {
		buffers[i].resize(2 * widths[i] * heights[i]);
		in.seekg(offsets[i]);
		readmany16le(in, (uint16_t*)buffers[i].data(), widths[i] * heights[i]);
	}
}

blkImage::~blkImage() {}

/* vim: set noet: */
