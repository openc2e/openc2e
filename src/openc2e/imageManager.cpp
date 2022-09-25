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

#include "Engine.h"
#include "PathResolver.h"
#include "World.h"
#include "common/Exception.h"
#include "common/backend/Backend.h"
#include "fileformats/ImageUtils.h"
#include "fileformats/hedfile.h"
#include "fileformats/paletteFile.h"

#include <array>
#include <fmt/format.h>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <iostream>
#include <memory>

using namespace ghc::filesystem;

std::shared_ptr<creaturesImage> tryOpenImage(std::string fname) {
	path realfile(findImageFile(fname));
	std::string basename = realfile.filename().stem();

	if (exists(realfile)) {
		auto img = std::make_shared<creaturesImage>(basename);
		img->images = ImageUtils::ReadImage(realfile);
		return img;
	}
	return {};
}

std::shared_ptr<creaturesImage> tryOpenBackground(std::string fname) {
	path realfile(findBackgroundFile(fname));
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
		path palpath(findMainDirectoryFile("Palettes/palette.dta"));
		if (exists(palpath) && !is_directory(palpath)) {
			default_palette = ReadPaletteFile(palpath);
		} else {
			throw Exception("Couldn't find C1 palette data!");
		}
	}
}

shared_array<Color> imageManager::getDefaultPalette() {
	return default_palette;
}

/*
 * Add an image to the gallery. Useful mainly for testing situations.
 */
void imageManager::addImage(std::shared_ptr<creaturesImage> image) {
	if (!image) {
		throw Exception("Can't add null image");
	}
	if (image->getName() == "") {
		throw Exception("Can't add image with no name");
	}
	std::map<std::string, std::weak_ptr<creaturesImage> >::iterator i = images.find(image->getName());
	if (i != images.end() && i->second.lock()) {
		throw Exception(fmt::format("Can't add image '{}' which already exists", image->getName()));
	}

	images[image->getName()] = image;
}

std::shared_ptr<creaturesImage> imageManager::getBackground(const std::string& name, unsigned int metaroom_width, unsigned int metaroom_height) {
	if (name.empty())
		return std::shared_ptr<creaturesImage>(); // empty sprites definitely don't exist

	// TODO: cache backgrounds
	std::shared_ptr<creaturesImage> img;
	if (engine.version == 1) {
		img = tryOpenImage(name + ".spr");
		// TODO: do any C1 metarooms have non-standard sizes?
		if (metaroom_width != 8352 || metaroom_height != 1200) {
			throw Exception(fmt::format("Expected Creatures 1 metaroom size to be 5x5 but got {}x{}", metaroom_width, metaroom_height));
		}
		if (img) {
			for (auto& i : img->images) {
				if (i.format == if_index8 && !i.palette) {
					i.palette = default_palette;
				}
			}
			if (img->images.size() != 464) {
				throw Exception(fmt::format("'{}.spr' is not a valid Creatures 1 background, expected 464 frames but got {}", name, img->images.size()));
			}
			for (const auto& frame : img->images) {
				if (frame.width != 144 || frame.height != 150) {
					throw Exception(fmt::format("'{}.spr' is not a valid Creatures 1 background, expected frame size to be 144x150 but got {}x{}", name, frame.width, frame.height));
				}
			}
			img->images = {ImageUtils::StitchBackground(img->images)};
		}
	} else if (engine.version == 2) {
		img = tryOpenImage(name + ".s16");
		// TODO: do any C2 metarooms have non-standard sizes?
		if (metaroom_width != 8352 || metaroom_height != 2400) {
			throw Exception(fmt::format("Expected Creatures 1 metaroom size to be 5x5 but got {}x{}", metaroom_width, metaroom_height));
		}
		if (img) {
			if (img->images.size() != 928) {
				throw Exception(fmt::format("'{}.s16' is not a valid Creatures 2 background, expected 928 frames but got {}", name, img->images.size()));
			}
			for (const auto& frame : img->images) {
				if (frame.width != 144 || frame.height != 150) {
					throw Exception(fmt::format("'{}.s16' is not a valid Creatures 2 background, expected frame size to be 144x150 but got {}x{}", name, frame.width, frame.height));
				}
			}
			img->images = {ImageUtils::StitchBackground(img->images)};
		}
	} else if (engine.bmprenderer) {
		img = tryOpenBackground(name + ".bmp");
	} else if (engine.version == 3) {
		img = tryOpenBackground(name + ".blk");
	} else {
		throw Exception("Don't know how to load backgrounds for current engine type");
	}
	if (!img) {
		std::cerr << "imageGallery couldn't find the background '" << name << "'" << std::endl;
	}
	if (img) {
		assert(img->images.size() == 1);
		if (img->width(0) < metaroom_width || img->height(0) < metaroom_height) {
			// Sea-Monkeys triggers this, because of course it does
			fmt::print(stderr, "warning: background '{}' has size {}x{} but metaroom is size {}x{}\n", name, img->width(0), img->height(0), metaroom_width, metaroom_height);
		}
	}
	return img;
}

/*
 * Retrieve an image for rendering use. To retrieve a sprite, pass the name without
 * extension. To retrieve a background, pass the full filename (ie, with .blk).
 */
std::shared_ptr<creaturesImage> imageManager::getImage(const std::string& name) {
	if (name.empty())
		return std::shared_ptr<creaturesImage>(); // empty sprites definitely don't exist

	// step one: see if the image is already in the gallery
	std::map<std::string, std::weak_ptr<creaturesImage> >::iterator i = images.find(name);
	if (i != images.end() && i->second.lock()) {
		return i->second.lock();
	}

	// step two: try opening it in .c16 form first, then try .s16 form
	std::shared_ptr<creaturesImage> img;
	if (engine.version == 1) {
		img = tryOpenImage(name + ".spr");
	} else if (engine.version == 2) {
		img = tryOpenImage(name + ".s16");
	} else if (engine.bmprenderer) {
		img = tryOpenImage(name + ".bmp");
		if (img) {
			path hedfilename(findImageFile(name + ".hed"));
			if (!hedfilename.empty()) {
				hedfile hed = read_hedfile(hedfilename);
				// TODO: hmm. should block size be per agent/part, instead of per loaded sprite?
				img->setBlockSize(hed.frame_width, hed.frame_height);
				if (img->numframes() != hed.numframes) {
					std::cerr << hedfilename.string() << ": number of frames doesn't match\n";
				}
			}
		}
	} else if (engine.version == 3) {
		img = tryOpenImage(name + ".c16");
		if (!img) {
			img = tryOpenImage(name + ".s16");
		}
	} else {
		throw Exception("Don't know how to load sprites for current engine type");
	}

	if (img) {
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

	std::string filename = findImageFile("EuroCharset.dta");
	if (filename.empty()) {
		filename = findImageFile("CHARSET.DTA");
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
				image.palette = default_palette;
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
			throw Exception("Unimplemented image format when loading charset.dta");
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
