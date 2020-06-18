/*
 *  creaturesImage.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat May 22 2004.
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

#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <vector>

enum imageformat { if_paletted, if_16bit_555, if_16bit_565, if_24bit };

unsigned int bitDepthOf(imageformat f);

class creaturesImage {
protected:
	friend class imageManager;
	unsigned int m_numframes;
	std::vector<uint16_t> widths, heights;
	std::vector<std::vector<unsigned char>> buffers;
	imageformat imgformat;
	std::string name;

	creaturesImage(std::string n = std::string()) { name = n; }

public:
	creaturesImage(std::string name, imageformat format,
	               std::vector<std::vector<unsigned char>> buffers,
				   std::vector<uint16_t> widths, std::vector<uint16_t> heights);
	virtual ~creaturesImage() { }
	imageformat format() { return imgformat; }
	unsigned int numframes() { return m_numframes; }
	unsigned int width(unsigned int frame) { return widths[frame]; }
	unsigned int height(unsigned int frame) { return heights[frame]; }
	void *data(unsigned int frame) { return buffers[frame].data(); }
	std::string getName() { return name; }

	virtual bool hasCustomPalette() { return false; }
	virtual uint8_t *getCustomPalette();
	
	virtual bool transparentAt(unsigned int frame, unsigned int x, unsigned int y);
};

/* vim: set noet: */
