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

#ifndef _CREATURESIMAGE_H
#define _CREATURESIMAGE_H

#include <memory>
#include <string>
#include <fstream>
#include <cassert>

enum imageformat { if_paletted, if_16bit, if_16bitcompressed, if_24bit };

unsigned int bitDepthOf(imageformat f);

class creaturesImage {
protected:
	unsigned int m_numframes;
	unsigned short *widths, *heights;
	void **buffers;
	bool is_565;
	imageformat imgformat;
	
	std::string name;
  
public:
	creaturesImage(std::string n = std::string()) { name = n; }
	virtual ~creaturesImage() { }
	bool is565() { return is_565; }
	imageformat format() { return imgformat; }
	unsigned int numframes() { return m_numframes; }
	unsigned int width(unsigned int frame) { return widths[frame]; }
	unsigned int height(unsigned int frame) { return heights[frame]; }
	void *data(unsigned int frame) { return buffers[frame]; }
	std::string getName() { return name; }

	virtual bool hasCustomPalette() { return false; }
	virtual uint8_t *getCustomPalette();
	
	virtual bool transparentAt(unsigned int frame, unsigned int x, unsigned int y);
	virtual std::shared_ptr<creaturesImage> mutableCopy();
	virtual void tint(unsigned char r, unsigned char g, unsigned char b, unsigned char rotation, unsigned char swap);
};

#endif
/* vim: set noet: */
