/*
 *  SimpleAgent.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
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

#include "SimpleAgent.h"
#include <string>
#include <fstream>
#include <iostream>
#include "c16Image.h"
#include "openc2e.h"

unsigned int SimpleAgent::getFirstImage() {
	// todo: adjust for BASE
	return first_image;
}

unsigned int SimpleAgent::getImageCount() {
	return image_count;
}

void SimpleAgent::setImage(std::string img) {
	std::string filename = "./data/Images/" + img + ".c16";
	std::cerr << "opening " << filename << "\n";
	std::ifstream i(filename.c_str());
	assert(i.is_open());
	sprite = new c16Image(i);
}
