/*
 *  creaturesImage.h
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

#ifndef _CREATURESIMAGE_H
#define _CREATURESIMAGE_H

#include <string>
#include <map>
#include <fstream>
#include "endianlove.h"
#include <cassert>

class creaturesImage {
private:
	unsigned int refcount;

protected:
	unsigned int m_numframes;
	unsigned short *widths, *heights;
	void **buffers;
	bool is_565;
	
	std::ifstream *stream;
  
public:
	std::string name;

	creaturesImage() { refcount = 0; stream = 0; }
	virtual ~creaturesImage() { assert(!refcount); if (stream) delete stream; }
	bool is565() { return is_565; }
	unsigned int numframes() { return m_numframes; }
	virtual unsigned int width(unsigned int frame) { return widths[frame]; }
	virtual unsigned int height(unsigned int frame) { return heights[frame]; }
	virtual unsigned int bitdepth() { return 16; }
	virtual void *data(unsigned int frame) { return buffers[frame]; }
	void addRef() { refcount++; }
	void delRef() { refcount--; }
	unsigned int refCount() { return refcount; }
	virtual std::string serializedName() { return name; }
	virtual bool transparentAt(unsigned int frame, unsigned int x, unsigned int y) = 0;
	
	friend class fileSwapper;
	friend class c16Image; // so duplicateTo can create a s16Image as required
};

class imageGallery {
protected:
	std::map<std::string, creaturesImage *> gallery;

public:
	creaturesImage *getImage(std::string name);
	void delImage(creaturesImage *i);
};

#endif
/* vim: set noet: */
