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
#include "images/charsetdta.h"
#include "openc2e.h"
#include "World.h"
#include "Engine.h"
#include "PathResolver.h"

#include <iostream>
#include <fmt/format.h>
#include <fstream>
#include <memory>

#include <ghc/filesystem.hpp>

using namespace ghc::filesystem;

enum filetype { blk, s16, c16, spr, bmp };

shared_ptr<creaturesImage> tryOpen(std::string fname, filetype ft) {
	path realfile(world.findFile(fname));
	std::string basename = realfile.filename().stem();
	std::ifstream in(realfile.string(), std::ios_base::binary);

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
 * Add an image to the gallery. Useful mainly for testing situations.
 */
void imageManager::addImage(shared_ptr<creaturesImage> image) {
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

std::shared_ptr<creaturesImage> imageManager::getCharsetDta(imageformat format, unsigned int textcolor, unsigned int bgcolor) {
	// TODO: cache this?

	std::string filename = world.findFile("Images/EuroCharset.dta");
	if (filename.empty()) {
		filename = world.findFile("Images/CHARSET.DTA");
	}
	if (filename.empty()) {
		return {};
	}

	std::ifstream in(filename, std::ios::binary);
	if (!in) {
		return {};
	}

	std::vector<uint8_t> filedata{std::istreambuf_iterator<char>(in), {}};
	CharsetDtaReader reader(filedata);

	std::vector<std::vector<uint8_t>> buffers(reader.getNumCharacters());
	std::vector<uint16_t> widths(reader.getNumCharacters());
	std::vector<uint16_t> heights(reader.getNumCharacters());
	for (size_t i = 0; i < reader.getNumCharacters(); ++i) {
		std::vector<uint8_t> chardata = reader.getCharData(i);
		widths[i] = reader.getCharWidth(i);
		heights[i] = reader.getCharHeight(i);

		// TODO: how do the values in the CHARSET.DTA map to actual color values?
		// just setting them all to the textcolor right now, but the real engines
		// do some shading/aliasing
		switch (format) {
			case if_paletted:
				for (size_t j = 0; j < reader.getCharWidth(i) * reader.getCharHeight(i); ++j) {
					if (chardata[j] != 0) {
						chardata[j] = textcolor;
					}
				}
				buffers[i] = std::move(chardata);
				break;
			case if_16bit_565:
				buffers[i].resize(chardata.size() * 2, 0);
				for (size_t j = 0; j < reader.getCharWidth(i) * reader.getCharHeight(i); ++j) {
					if (chardata[j] != 0) {
						((uint16_t*)buffers[i].data())[j] = textcolor;
					}
				}
				break;
			case if_16bit_555: // TODO: how to tell what format the color is in?
				throw creaturesException("Unimplemented format if_16bit_555 when loading charset.dta");
			case if_24bit:
				throw creaturesException("Unimplemented format if_24bit when loading charset.dta");
		}
	}

	return std::make_shared<creaturesImage>(path(filename).stem(), format, buffers, widths, heights);
}

std::shared_ptr<creaturesImage> imageManager::tint(const std::shared_ptr<creaturesImage>& oldimage,
                                                   unsigned char r, unsigned char g, unsigned char b,
                                                   unsigned char rotation, unsigned char swap) {
	if (!(oldimage->format() == if_16bit_565 || oldimage->format() == if_16bit_555)) {
		throw creaturesException(fmt::format(
			"Internal error: Tried to tint a sprite \"{}\" which doesn't support that.",
			oldimage->getName()
		));
	}

	if (128 == r && 128 == g && 128  == b && 128  == rotation && 128 == swap) return oldimage; // duh

	auto buffers = oldimage->buffers;

	/*
	 * CDN:
	 * if rotation >= 128
	 * absRot = rotation-128
	 * else
	 * absRot = 128 - rotation
	 * endif
	 * invRot = 127-absRot
	 */
	int absRot;
	if (rotation >= 128)
		absRot = (int)rotation - 128;
	else
		absRot = 128 - (int)rotation;
	int invRot = 127 - absRot;

	/*
	 * CDN:
	 * if swap >= 128
	 * absSwap = swap - 128
	 * else
	 * absSwap = 128 - swap
	 * endif
	 * invSwap = 127-absSwap
	 */
	int absSwap;
	if (swap >= 128)
		absSwap = (int)swap - 128;
	else
		absSwap = 128 - (int)swap;
	int invSwap = 127 - absSwap;

	/*
	 * CDN:
	 * redTint = red-128
	 * greenTint = green-128
	 * blueTint = blue-128
	 */

	int redTint = (int)r - 128;
	int greenTint = (int)g - 128;
	int blueTint = (int)b - 128;

	for (unsigned int i = 0; i < oldimage->m_numframes; i++) {
		for (unsigned int j = 0; j < oldimage->heights[i]; j++) {
			for (unsigned int k = 0; k < oldimage->widths[i]; k++) {
				unsigned short v = ((unsigned short *)buffers[i].data())[(j * oldimage->widths[i]) + k];
				if (v == 0) continue;

				/*
				 * CDN:
				 * tempRed = RedValue + redTint;
				 * tempGreen = GreenValue + greenTint;
				 * tempBlue = BlueValue + blueTint;
				 */
				// TODO: should this work differently for 565 vs 555 color?
				int red = (((uint32_t)(v) & 0xf800) >> 8) + redTint;
				if (red < 0) red = 0; else if (red > 255) red = 255;
				int green = (((uint32_t)(v) & 0x07e0) >> 3) + greenTint;
				if (green < 0) green = 0; else if (green > 255) green = 255;
				int blue = (((uint32_t)(v) & 0x001f) << 3) + blueTint;
				if (blue < 0) blue = 0; else if (blue > 255) blue = 255;

				/*
				 * CDN:
				 * if (rotation < 128)
				 * rotRed = ((absRot * tempBlue) + (invRot * tempRed)) / 256
				 * rotGreen = ((absRot * tempRed) + (invRot * tempGreen)) / 256
				 * rotBlue = ((absRot * tempGreen) + (invRot * tempBlue)) / 256
				 * endif
				 */

				int rotRed, rotGreen, rotBlue;
				rotRed = ((blue * absRot) + (red * invRot)) / 128;
				rotGreen = ((red * absRot) + (green * invRot)) / 128;
				rotBlue = ((green * absRot) + (blue * invRot)) / 128;


				/*
				 * CDN:
				 * swappedRed = ((absSwap * rotBlue) + (invSwap * rotRed))/256
				 * swappedBlue = ((absSwap * rotRed) + (invSwap * rotBlue))/256
				 *
				 * fuzzie notes that this doesn't seem to be a no-op for swap=128..
				 */
				int swappedRed = ((absSwap * blue) + (invSwap * red)) / 128;
				int swappedBlue = ((absSwap * red) + (invSwap * blue)) / 128;

				/*
				 * SetColour(definedcolour to (swappedRed,rotGreen,swappedBlue))
				 */
				swappedRed = (swappedRed << 8) & 0xf800;
				rotGreen = (rotGreen << 3) & 0x7e0;
				swappedBlue = (swappedBlue >> 3) & 0x1f;
				v = (swappedRed | rotGreen | swappedBlue);
				/*
				 * if definedcolour ==0 SetColour(definedcolour to (1,1,1))
				 */
				if (v == 0)
					v = (1 << 11 | 1 << 5 | 1);
				((unsigned short *)buffers[i].data())[(j * oldimage->widths[i]) + k] = v;
			}
		}
	}

	std::shared_ptr<creaturesImage> img(new creaturesImage(
		oldimage->getName(),
		oldimage->imgformat,
		buffers,
		oldimage->widths,
		oldimage->heights
	));
	return img;
}

/* vim: set noet: */
