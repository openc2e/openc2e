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
#include "openc2e.h"
#include <iostream>
#include <fstream>

imageGallery gallery;

creaturesImage *imageGallery::getImage(std::string name) {
	std::map<std::string, creaturesImage *>::iterator i = gallery.find(name);
	if (i != gallery.end()) {
		creaturesImage *img = i->second;
		img->addRef();
		return img;
	}
	// todo: we need to check for c16 first, then s16, then spr(?)
	std::string filename = "./data/Images/" + name + ".c16";
	std::cout << "imageGallery: opening " << filename << "\n";
	std::ifstream in(filename.c_str());
	assert(in.is_open());
	gallery[name] = new c16Image(in);
	return gallery[name];
}

void imageGallery::delImage(creaturesImage *in) {
	in->delRef();
	if (in->refCount() == 0) {
		delete in;
		for (std::map<std::string, creaturesImage *>::iterator i = gallery.begin(); i != gallery.end(); i++) {
			if (i->second == in) { gallery.erase(i); return; }
		}
		std::cout << "imageGallery warning: delImage got a newly unreferenced image but it isn't in the gallery\n";
	}
}
