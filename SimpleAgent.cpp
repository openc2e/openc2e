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

SimpleAgent::SimpleAgent(unsigned int family, unsigned int genus, unsigned int species, unsigned int plane,
												unsigned int firstimage, unsigned int imagecount) :
Agent(family, genus, species, plane) {
	first_image = firstimage;
	image_count = imagecount;
	setAttributes(0);
}

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

void SimpleAgent::setAttributes(unsigned int attr) {
	carryable = (attr & 1);
	mouseable = (attr & 2);
	activateable = (attr & 4);
	invisible = (attr & 16);
	floatable = (attr & 32);
	suffercollisions = (attr & 64);
	sufferphysics = (attr & 128);
	camerashy = (attr & 256);
	rotatable = (attr & 1024);
	presence = (attr & 2048);
}

unsigned int SimpleAgent::getAttributes() {
	unsigned int a = (carryable ? 1 : 0);
	a += (mouseable ? 2: 0);
	a += (activateable ? 4: 0);
	a += (invisible ? 16: 0);
	a += (floatable ? 32: 0);
	a += (suffercollisions ? 64: 0);
	a += (sufferphysics ? 128: 0);
	a += (camerashy ? 256: 0);
	a += (rotatable ? 1024: 0);
	return a + (presence ? 2048: 0);
}

void SimpleAgent::setFrameNo(unsigned int f) {
	assert(f < animation.size());
	frameno = f;
}

unsigned int SimpleAgent::getCurrentSprite() {
	if (!animation.empty()) {
		assert(frameno < animation.size());
		return getFirstImage() + animation[frameno];
	} else {
		return getFirstImage() + 0; // TODO!!!
	}
}

void SimpleAgent::tick() {
	if (!animation.empty()) {
		unsigned int f = frameno + 1;
		if (f == animation.size()) return;
		if (animation[f] == 255) {
			if (f == (animation.size() - 1)) f = 0;
			else f = animation[f + 1];
		}
		setFrameNo(f);
	}
}
