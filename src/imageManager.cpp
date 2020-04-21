/*
 *  imageManager.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Jun 06 2004.
 *  Copyright (c) 2004-2008 Alyssa Milburn. All rights reserved.
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

#include "imageManager.h"
#include "images/c16Image.h"
#include "images/sprImage.h"
#include "images/blkImage.h"
#include "images/bmpImage.h"
#include "openc2e.h"
#include "World.h"
#include "Engine.h"

#include "PathResolver.h"

#include <iostream>
#include <fstream>
#include <memory>

#include <ghc/filesystem.hpp>

using namespace ghc::filesystem;

enum filetype { blk, s16, c16, spr, bmp };

bool tryOpen(std::ifstream *in, shared_ptr<creaturesImage> &img, std::string fname, filetype ft) {
	path realfile;
	if (fname.size() < 5) return false; // not enough chars for an extension and filename..

	path realfile(world.findFile(fname));
	// if it doesn't exist, too bad, give up.
	if (!exists(realfile)) return false;

	std::string basename = realfile.filename().string(); basename.erase(basename.end() - 4, basename.end());

	in->clear();
	in->open(realfile.string());

	if (in->is_open()) {
		switch (ft) {
			case blk: img = shared_ptr<creaturesImage>(new blkImage(in, basename)); break;
			case c16: img = shared_ptr<creaturesImage>(new c16Image(in, basename)); break;
			case s16: img = shared_ptr<creaturesImage>(new s16Image(in, basename)); break;
			case spr: img = shared_ptr<creaturesImage>(new sprImage(in, basename)); break;
			case bmp: img = shared_ptr<creaturesImage>(new bmpImage(in, basename)); break; // TODO: don't commit this ;p
		}
	}
	return in->is_open();
}

/*
 * Retrieve an image for rendering use. To retrieve a sprite, pass the name without
 * extension. To retrieve a background, pass the full filename (ie, with .blk).
 */
shared_ptr<creaturesImage> imageManager::getImage(std::string name, bool is_background) {
	if (name.empty()) return shared_ptr<creaturesImage>(); // empty sprites definitely don't exist

	// step one: see if the image is already in the gallery
	std::map<std::string, std::weak_ptr<creaturesImage> >::iterator i = images.find(name);
	if (i != images.end() && i->second.lock()) {
		if (!is_background) return i->second.lock(); // TODO: handle backgrounds
	}

	// step two: try opening it in .c16 form first, then try .s16 form
	std::ifstream *in = new std::ifstream();
	shared_ptr<creaturesImage> img;

	std::string fname;
	if (is_background) {
		fname = std::string("Backgrounds/") + name;
	} else {
		fname = std::string("Images/") + name;
	}

	// TODO: try/catch to free the std::ifstream
	bool successful = true;
	if (engine.bmprenderer) {
		successful = tryOpen(in, img, fname + ".bmp", bmp);
	} else {
		if (is_background) {
			successful = tryOpen(in, img, fname + ".blk", blk);
		} else {
			if (!tryOpen(in, img, fname + ".s16", s16)) {
				if (!tryOpen(in, img, fname + ".c16", c16)) {
					if (!tryOpen(in, img, fname + ".spr", spr)) {
						successful = false;
					}
				}
			}
		}
	}

	if (successful) {
		if (!is_background) // TODO: handle backgrounds
			images[name] = img;
	} else {
		std::cerr << "imageGallery couldn't find the sprite '" << name << "'" << std::endl;
		delete in;
		return shared_ptr<creaturesImage>();
	}

	in->close(); // doesn't close the mmap, which we still need :)

	return img;
}

/* vim: set noet: */
