/*
 *  c16Image.h
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

#include "creaturesImage.h"
#include <istream>
#include "mmapifstream.h"
#include "endianlove.h"

class c16Image : public creaturesImage {
private:
	unsigned int **lineoffsets;

public:
	c16Image() { }
	c16Image(mmapifstream *, std::string n);
	~c16Image();
	void readHeader(std::istream &in);
	std::shared_ptr<creaturesImage> mutableCopy();
	bool transparentAt(unsigned int frame, unsigned int x, unsigned int y);
};

class s16Image : public creaturesImage {
private:
	uint32 *offsets;

public:
	s16Image() { }
	s16Image(mmapifstream *, std::string n);
	~s16Image();
	void readHeader(std::istream &in);
	void writeHeader(std::ostream &out);
	std::shared_ptr<creaturesImage> mutableCopy();
	void tint(unsigned char r, unsigned char g, unsigned char b, unsigned char rotation, unsigned char swap);
	bool transparentAt(unsigned int frame, unsigned int x, unsigned int y);

	friend class c16Image;
	friend class fileSwapper;
};

/* vim: set noet: */
