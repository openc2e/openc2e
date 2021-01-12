/*
 *  imageManager.h
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

#ifndef _IMAGEMANAGER_H
#define _IMAGEMANAGER_H

#include "creaturesImage.h"

#include <map>
#include <memory>
#include <string>

class imageManager {
  protected:
	friend class Openc2eTestHelper;
	std::map<std::string, std::weak_ptr<creaturesImage> > images;
	void addImage(std::shared_ptr<creaturesImage>);
	shared_array<Color> palette;

  public:
	void loadDefaultPalette();
	shared_array<Color> getDefaultPalette();
	std::shared_ptr<creaturesImage> getImage(std::string name, bool is_background = false);
	std::shared_ptr<creaturesImage> getCharsetDta(imageformat format, uint32_t bgcolor,
		uint32_t textcolor, uint32_t aliascolor);
	std::shared_ptr<creaturesImage> tint(const std::shared_ptr<creaturesImage>& image,
		unsigned char r, unsigned char g, unsigned char b,
		unsigned char rotation, unsigned char swap);
};

#endif
/* vim: set noet: */
