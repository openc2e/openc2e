/*
 *  creaturesImage.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat May 22 2004.
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

#pragma once

#include "Texture.h"
#include "common/Image.h"
#include "common/shared_array.h"

#include <cassert>
#include <memory>
#include <string>
#include <vector>


class creaturesImage final {
  protected:
	friend class imageManager;
	std::string name;
	unsigned int block_width = 0;
	unsigned int block_height = 0;

  public:
	creaturesImage(std::string n = std::string());
	imageformat format(unsigned int frame) const;
	unsigned int numframes() const;
	unsigned int width(unsigned int frame) const;
	unsigned int height(unsigned int frame) const;
	const void* data(unsigned int frame) const;
	std::string getName() const;

	const Image& getImageForFrame(unsigned int frame) const;
	Texture& getTextureForFrame(unsigned int frame);

	bool hasCustomPalette(unsigned int frame) const;
	shared_array<Color> getCustomPalette(unsigned int frame) const;

	bool transparentAt(unsigned int frame, unsigned int x, unsigned int y) const;

	void setBlockSize(unsigned int width, unsigned int height);
	unsigned int getXOffsetForFrame(unsigned int frame) const;
	unsigned int getYOffsetForFrame(unsigned int frame) const;

	std::vector<Texture> textures;
	std::vector<Image> images;
};

/* vim: set noet: */
