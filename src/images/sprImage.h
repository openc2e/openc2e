/*
 *  sprImage.h
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

#ifndef _SPRIMAGE_H
#define _SPRIMAGE_H

#include "creaturesImage.h"

class sprImage : public creaturesImage {
private:
	uint32_t *offsets;

public:
	sprImage() { }
	sprImage(std::ifstream &in, std::string n);
	~sprImage();
	virtual unsigned int bitdepth() { return 8; }
	bool transparentAt(unsigned int frame, unsigned int x, unsigned int y);
};

#endif
/* vim: set noet: */
