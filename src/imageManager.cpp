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

shared_ptr<creaturesImage> tryOpen(std::string fname, filetype ft) {
	path realfile(world.findFile(fname));
	std::string basename = realfile.filename().stem();
	std::ifstream in(realfile.string());

	if (in.is_open()) {
		switch (ft) {
			case blk: return shared_ptr<creaturesImage>(new blkImage(in, basename));
			case c16: return shared_ptr<creaturesImage>(new c16Image(in, basename));
			case s16: return shared_ptr<creaturesImage>(new s16Image(in, basename));
			case spr: return shared_ptr<creaturesImage>(new sprImage(in, basename));
			case bmp: return shared_ptr<creaturesImage>(new bmpImage(in, basename)); // TODO: don't commit this ;p
		}
	}
	return {};
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
	std::string fname;
	if (is_background) {
		fname = std::string("Backgrounds/") + name;
	} else {
		fname = std::string("Images/") + name;
	}

	shared_ptr<creaturesImage> img;
	if (engine.bmprenderer) {
		img = tryOpen(fname + ".bmp", bmp);
	} else {
		if (is_background) {
			img = tryOpen(fname + ".blk", blk);
		} else {
			img = tryOpen(fname + ".s16", s16);
			if (!img) img = tryOpen(fname + ".c16", c16);
			if (!img) img = tryOpen(fname + ".spr", spr);
		}
	}

	if (img) {
		if (!is_background) // TODO: handle backgrounds
			images[name] = img;
	} else {
		std::cerr << "imageGallery couldn't find the sprite '" << name << "'" << std::endl;
		return shared_ptr<creaturesImage>();
	}

	return img;
}

/* vim: set noet: */
