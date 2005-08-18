/*
 *  creaturesImage.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Jun 06 2004.
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
#include "c16Image.h"
#include "blkImage.h"
#include "openc2e.h"
#include "fileSwapper.h"
#include "PathResolver.h"
#include <iostream>
#include <fstream>

imageGallery gallery;

enum filetype { blk, s16, c16 };

bool tryOpen(mmapifstream *in, std::string dir, std::string filename, filetype ft) {
	std::string fname = dir + filename;
#ifdef __C2E_BIGENDIAN
  if (ft != c16) fname = fname + ".big";
#endif
	in->clear();
	resolveFile(fname);
	in->mmapopen(fname.c_str());
#ifdef __C2E_BIGENDIAN
	if (!in->is_open()) {
		// todo: work out whether the to-be-converted from file exists here
		// todo: spout a "trying to convert file" message here
		fileSwapper f;
		switch (ft) {
			case blk:
				f.convertblk(dir, filename);
				break;
			case s16:
				f.converts16(dir, filename);
				break;
			case c16:
				// f.convertc16(dir, filename);
				break;
		}
		in->clear();
		resolveFile(fname);
		in->mmapopen(fname.c_str());
	}
#endif
	return in->is_open();
}

creaturesImage *imageGallery::getImage(std::string name) {
	// step one: see if the image is already in the gallery
	std::map<std::string, creaturesImage *>::iterator i = gallery.find(name);
	if (i != gallery.end()) {
		creaturesImage *img = i->second;
		img->addRef();
		return img;
	}

	// step two: try opening it in .c16 form first, then try .s16 form
	mmapifstream *in = new mmapifstream();

	// std::cout << "imageGallery is trying to open '" << name << "'" << std::endl;

	if (!tryOpen(in, datapath + "/Images/", name + ".s16", s16)) {
		if (!tryOpen(in, datapath + "/Images/", name + ".c16", c16)) {
			bool lasttry = tryOpen(in, datapath + "/Backgrounds/", name, blk);
			if (!lasttry) return 0;
			gallery[name] = new blkImage(in);
		} else {
			gallery[name] = new c16Image(in);
		}
	} else {
		gallery[name] = new s16Image(in);
	}
	
	in->close(); // doesn't close the mmap, which we still need :)

	return gallery[name];
}

void imageGallery::delImage(creaturesImage *in) {
	in->delRef();
	if (in->refCount() == 0) {
		delete in;
		for (std::map<std::string, creaturesImage *>::iterator i = gallery.begin(); i != gallery.end(); i++) {
			if (i->second == in) { gallery.erase(i); return; }
		}
		std::cerr << "imageGallery warning: delImage got a newly unreferenced image but it isn't in the gallery\n";
	}
}

/* vim: set noet: */
