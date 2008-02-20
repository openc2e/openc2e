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

class bmpImage : public creaturesImage {
public:
	bmpImage(mmapifstream *, std::string n);
	~bmpImage();
};

#endif
/* vim: set noet: */
