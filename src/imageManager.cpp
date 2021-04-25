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

#include "Backend.h"
#include "Engine.h"
#include "PathResolver.h"
#include "World.h"
#include "creaturesException.h"
#include "fileformats/ImageUtils.h"
#include "fileformats/hedfile.h"
#include "utils/mmapifstream.h"

#include <array>
#include <fmt/format.h>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <iostream>
#include <memory>

using namespace ghc::filesystem;

std::shared_ptr<creaturesImage> tryOpen(std::string fname) {
	path realfile(world.findFile(fname));
	std::string basename = realfile.filename().stem();

	if (exists(realfile)) {
		auto img = std::make_shared<creaturesImage>(basename);
		img->images = ImageUtils::ReadImage(realfile);
		return img;
	}
	return {};
}

void imageManager::loadDefaultPalette() {
	if (engine.gametype == "c1") {
		std::cout << "* Loading palette.dta..." << std::endl;
		// TODO: case-sensitivity for the lose
		path palpath(world.findFile("Palettes/palette.dta"));
		if (exists(palpath) && !is_directory(palpath)) {
			std::ifstream f(palpath.string().c_str(), std::ios::binary);
			f >> std::noskipws;
			std::array<uint8_t, 768> palette_data;
			f.read((char*)palette_data.data(), 768);

			palette = shared_array<Color>(256);
			for (unsigned int i = 0; i < 256; i++) {
				palette[i].r = palette_data[i * 3] * 4;
				palette[i].g = palette_data[i * 3 + 1] * 4;
				palette[i].b = palette_data[i * 3 + 2] * 4;
				palette[i].a = 0xff;
			}

			engine.backend->setDefaultPalette(palette);
		} else
			throw creaturesException("Couldn't find C1 palette data!");
	}
}

shared_array<Color> imageManager::getDefaultPalette() {
	return palette;
}

/*
 * Add an image to the gallery. Useful mainly for testing situations.
 */
void imageManager::addImage(std::shared_ptr<creaturesImage> image) {
	if (!image) {
		throw creaturesException("Can't add null image");
	}
	if (image->getName() == "") {
		throw creaturesException("Can't add image with no name");
	}
	std::map<std::string, std::weak_ptr<creaturesImage> >::iterator i = images.find(image->getName());
	if (i != images.end() && i->second.lock()) {
		throw creaturesException(fmt::format("Can't add image '{}' which already exists", image->getName()));
	}

	images[image->getName()] = image;
}

/*
 * Retrieve an image for rendering use. To retrieve a sprite, pass the name without
 * extension. To retrieve a background, pass the full filename (ie, with .blk).
 */
std::shared_ptr<creaturesImage> imageManager::getImage(std::string name, bool is_background) {
	if (name.empty())
		return std::shared_ptr<creaturesImage>(); // empty sprites definitely don't exist

	// step one: see if the image is already in the gallery
	std::map<std::string, std::weak_ptr<creaturesImage> >::iterator i = images.find(name);
	if (i != images.end() && i->second.lock()) {
		if (!is_background)
			return i->second.lock(); // TODO: handle backgrounds
	}

	// step two: try opening it in .c16 form first, then try .s16 form
	std::string fname;
	if (is_background && engine.version == 3) {
		fname = std::string("Backgrounds/") + name;
	} else {
		fname = std::string("Images/") + name;
	}

	std::shared_ptr<creaturesImage> img;
	if (engine.bmprenderer) {
		img = tryOpen(fname + ".bmp");
		if (img && !is_background) {
			path hedfilename(world.findFile("Images/" + name + ".hed"));
			if (!hedfilename.empty()) {
				hedfile hed = read_hedfile(hedfilename);
				// TODO: hmm. should block size be per agent/part, instead of per loaded sprite?
				img->setBlockSize(hed.frame_width, hed.frame_height);
				if (img->numframes() != hed.numframes) {
					std::cerr << hedfilename.string() << ": number of frames doesn't match\n";
				}
			}
		}
	} else {
		if (is_background && engine.version == 3) {
			img = tryOpen(fname + ".blk");
		} else {
			img = tryOpen(fname + ".s16");
			if (!img)
				img = tryOpen(fname + ".c16");
			if (!img)
				img = tryOpen(fname + ".spr");
		}
	}

	if (img) {
		if (!is_background) // TODO: handle backgrounds
			images[name] = img;
	} else {
		std::cerr << "imageGallery couldn't find the sprite '" << name << "'" << std::endl;
		return std::shared_ptr<creaturesImage>();
	}

	return img;
}

std::shared_ptr<creaturesImage> imageManager::getCharsetDta(imageformat format,
	uint32_t bgcolor,
	uint32_t textcolor,
	uint32_t aliascolor) {
	// TODO: cache this?

	// TODO: use bgcolor and aliascolor
	(void)bgcolor;
	(void)aliascolor;

	std::string filename = world.findFile("Images/EuroCharset.dta");
	if (filename.empty()) {
		filename = world.findFile("Images/CHARSET.DTA");
	}
	if (filename.empty()) {
		return {};
	}

	MultiImage images = ImageUtils::ReadImage(filename);

	// TODO: how do the values in the CHARSET.DTA map to actual color values?
	// just setting them all to the textcolor right now, but the real engines
	// do some shading/aliasing
	switch (format) {
		case if_index8:
			for (auto& image : images) {
				image.palette = palette;
				for (size_t j = 0; j < image.data.size(); ++j) {
					if (image.data[j] != 0) {
						image.data[j] = textcolor;
					}
				}
			}
			break;
		case if_bgr24: {
			shared_array<Color> palette(256);
			palette[0] = Color{0, 0, 0, 0xff}; // black is set as the transparent color
			for (int i = 1; i < 256; i++) {
				palette[i].r = (textcolor >> 16) & 0xff;
				palette[i].g = (textcolor >> 8) & 0xff;
				palette[i].b = textcolor & 0xff;
				palette[i].a = 0xff;
			}
			for (auto& image : images) {
				image.palette = palette;
			}
		} break;
		default:
			throw creaturesException("Unimplemented image format when loading charset.dta");
	}

	auto img = std::make_shared<creaturesImage>(path(filename).stem());
	img->images = images;
	return img;
}

std::shared_ptr<creaturesImage> imageManager::tint(const std::shared_ptr<creaturesImage>& oldimage,
	unsigned char r, unsigned char g, unsigned char b,
	unsigned char rotation, unsigned char swap) {
	auto img = std::make_shared<creaturesImage>(oldimage->getName());
	img->images.resize(oldimage->images.size());
	for (size_t i = 0; i < img->images.size(); ++i) {
		img->images[i] = ImageUtils::Tint(oldimage->images[i], r, g, b, rotation, swap);
	}
	return img;
}

/* vim: set noet: */
