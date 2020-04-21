/*
 *  blkImage.h
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

#ifndef _BLKIMAGE_H
#define _BLKIMAGE_H

#include "creaturesImage.h"

class blkImage : public creaturesImage {
private:
	uint32_t *offsets;

public:
	unsigned int totalwidth, totalheight;

	blkImage() { }
	blkImage(std::ifstream *, std::string n);
	~blkImage();
	void readHeader(std::istream &in);
	void writeHeader(std::ostream &s);
	virtual std::string serializedName() { return name + ".blk"; }
	bool transparentAt(unsigned int frame, unsigned int x, unsigned int y) { (void)frame; (void)x; (void)y; return false; }

	friend class fileSwapper;
};

#endif
/* vim: set noet: */
