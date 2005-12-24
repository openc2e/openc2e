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

class duppableImage : public creaturesImage {
public:
	virtual void duplicateTo(class s16Image *) = 0;
};

class c16Image : public duppableImage {
private:
	unsigned int **lineoffsets;

public:
	c16Image() { }
	c16Image(mmapifstream *);
	~c16Image();
	void readHeader(std::istream &in);
	void duplicateTo(class s16Image *);
};

class s16Image : public duppableImage {
private:
	uint32 *offsets;

public:
	s16Image() { }
	s16Image(mmapifstream *);
	~s16Image();
	void readHeader(std::istream &in);
	void writeHeader(std::ostream &out);
	void duplicateTo(class s16Image *);
	void tint(unsigned char r, unsigned char g, unsigned char b, unsigned char rotation, unsigned char swap);

	friend class c16Image;
	friend class fileSwapper;
};

/* vim: set noet: */
