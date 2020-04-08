/*
 *  bmpImage.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed Feb 20 2008.
 *  Copyright (c) 2008 Alyssa Milburn. All rights reserved.
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

#ifndef _BMPIMAGE_H
#define _BMPIMAGE_H

#include "creaturesImage.h"
#include "mmapifstream.h"
#include "endianlove.h"

class bmpImage;

class bmpData {
private:
	friend class bmpImage;

	bmpData(mmapifstream *, std::string n);

	uint32 biWidth, biHeight;
	uint32 biCompression;
	void *bmpdata;
	bool copied_data;
	uint8 *palette;
	imageformat imgformat;
	mmapifstream *stream;

public:
	~bmpData(); // shared_ptr needs to be able to call the destructor
};

class bmpImage : public creaturesImage {
protected:
	std::shared_ptr<bmpData> bmpdata;
	void freeData();

public:
	bmpImage(mmapifstream *, std::string n);
	~bmpImage();

	void setBlockSize(unsigned int blockwidth, unsigned int blockheight);

	bool hasCustomPalette() { return imgformat == if_paletted; }
	uint8 *getCustomPalette() { return bmpdata->palette; }
};

#endif
/* vim: set noet: */
