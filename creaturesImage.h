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
#include <boost/iostreams/device/mapped_file.hpp>
#include <boost/smart_ptr.hpp>

using boost::iostreams::mapped_file;
using boost::shared_ptr;

class creaturesImage {
private:
	unsigned int refcount;

protected:
	unsigned int m_numframes;
	unsigned short *widths, *heights;
	void **buffers;
	bool is_565;
	
	shared_ptr<mapped_file> map_file;
  
public:
	std::string name;

	creaturesImage() { refcount = 0; }
	creaturesImage(shared_ptr<mapped_file> f) : map_file(f) { refcount = 0; }
	virtual ~creaturesImage() { assert(!refcount); }
	bool is565() { return is_565; }
	unsigned int numframes() { return m_numframes; }
	virtual unsigned int width(unsigned int frame) { return widths[frame]; }
	virtual unsigned int height(unsigned int frame) { return heights[frame]; }
	virtual void *data(unsigned int frame) { return buffers[frame]; }
	void addRef() { refcount++; }
	void delRef() { refcount--; }
	unsigned int refCount() { return refcount; }
	
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

extern imageGallery gallery;

#endif
/* vim: set noet: */
