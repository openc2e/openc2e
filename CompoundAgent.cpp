/*
 *  CompoundAgent.cpp
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

#include "CompoundAgent.h"
#include "openc2e.h"
#include "c16Image.h"
#include <algorithm> // sort

void CompoundAgent::render(SDLBackend *renderer, int xoffset, int yoffset) {
	for (std::vector<CompoundPart *>::iterator i = parts.begin(); i != parts.end(); i++) {
		(*i)->render(renderer, xoffset + (int)x, yoffset + (int)y);
	}

	// draw core
	int xoff = xoffset + x;
	int yoff = yoffset + y;
	renderer->renderLine(xoff + (getWidth() / 2), yoff, xoff + getWidth(), yoff + (getHeight() / 2), 0xFF0000CC);
	renderer->renderLine(xoff + getWidth(), yoff + (getHeight() / 2), xoff + (getWidth() / 2), yoff + getHeight(), 0xFF0000CC);
	renderer->renderLine(xoff + (getWidth() / 2), yoff + getHeight(), xoff, yoff + (getHeight() / 2), 0xFF0000CC);
	renderer->renderLine(xoff, yoff + (getHeight() / 2), xoff + (getWidth() / 2), yoff, 0xFF0000CC);
}

void CompoundAgent::addPart(CompoundPart *p) {
	assert(!part(p->id)); // todo: handle better

	// todo: we should prbly insert at the right place, not call sort
	parts.push_back(p);
	std::sort(parts.begin(), parts.end());

	if (width < p->getWidth()) width = p->getWidth();
	if (height < p->getHeight()) height = p->getHeight();
}

void CompoundAgent::delPart(unsigned int id) {
	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		if ((*x)->id == id) { parts.erase(x); return; }
	}
	
	throw "oops"; // TODO: handle this exception properly
}

CompoundPart *CompoundAgent::part(unsigned int id) {
	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		if ((*x)->id == id) return *x;
	}
	return 0;
}

void CompoundPart::render(SDLBackend *renderer, int xoffset, int yoffset) {
	renderer->render(getSprite(), getCurrentSprite(), xoffset + x, yoffset + y);							
}

CompoundAgent::CompoundAgent(unsigned char _family, unsigned char _genus, unsigned short _species, unsigned int plane,
				std::string spritefile, unsigned int firstimage, unsigned int imagecount) :
				Agent(_family, _genus, _species, plane) {
	// CAOS docs seem to imply initial part is part 1 in NEW: COMP, but rest of docs call it part 0
	// TODO: we ignore image count acos it sucks
	CompoundPart *p = new DullPart(0, spritefile, firstimage, 0, 0, 0);
	addPart(p);

	width = p->getWidth();
	height = p->getHeight();
}

CompoundPart::CompoundPart(unsigned int _id, std::string spritefile, unsigned int fimg,
						unsigned int _x, unsigned int _y, unsigned int _z) {
	id = _id;
	firstimg = fimg;
	x = _x;
	y = _y;
	zorder = _z;
	sprite = gallery.getImage(spritefile);
	assert(sprite);
	pose = 0;
}

DullPart::DullPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			 unsigned int _z) : CompoundPart(_id, spritefile, fimg, _x, _y, _z) {
}

ButtonPart::ButtonPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			               unsigned int _z, std::string animhover, int msgid, int option) : CompoundPart(_id, spritefile, fimg, _x, _y, _z) {
}

// TODO: combine identical code from SimpleAgent/CompoundAgent
// TODO: implement set/getAttributes for Vehicle/Creature(?)
void CompoundAgent::setAttributes(unsigned int attr) {
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

unsigned int CompoundAgent::getAttributes() {
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

void CompoundAgent::tick() {
	Agent::tick();

	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		(*x)->tick();
	}
}

void CompoundPart::tick() {
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

