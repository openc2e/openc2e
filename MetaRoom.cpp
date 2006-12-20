/*
 *  MetaRoom.cpp
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

#include "MetaRoom.h"
#include "Room.h"
#include "World.h"
#include "creaturesImage.h"
#include <assert.h>
#include "Backend.h"

MetaRoom::MetaRoom(int _x, int _y, int _width, int _height, const std::string &back, creaturesImage *spr, bool wrap) {
	xloc = _x; yloc = _y; wid = _width; hei = _height; firstback = 0; wraps = wrap;

	// if we were provided with a background, add it
	if (!back.empty()) {
		if (spr) {
			addBackground(back, spr);
		} else {
			addBackground(back);
		}
	}
}

void MetaRoom::addBackground(std::string back, creaturesImage *spr) {
	creaturesImage *backsprite;
	unsigned int totalwidth, totalheight;

	caos_assert(!back.empty());
	// TODO: cadv adds backgrounds which have already been added as the default, look into this,
	// should we preserve the default once extra backgrounds have been added and change this to
	// a caos_assert?
	if (backgrounds.find(back) != backgrounds.end()) return;
	
	if (!spr) {
		// we weren't passed a sprite, so we need to load one
		backsprite = world.gallery.getImage(back + ".blk");
		blkImage *background = dynamic_cast<blkImage *>(backsprite);
		caos_assert(background);

		totalwidth = background->totalwidth;
		totalheight = background->totalheight;
	} else {
		// we were provided with a sprite, so use it
		backsprite = spr;
		totalwidth = wid;
		totalheight = hei;
	}

	// store the background
	backgrounds[back] = backsprite;
	if (!firstback) {
		// set the first background
		firstback = backsprite;
		fullwid = totalwidth;
		fullhei = totalheight;
	} else {
		// make sure other backgrounds are the same size
		assert(totalwidth == fullwid);
		assert(totalheight == fullhei);
	}
}

std::vector<std::string> MetaRoom::backgroundList() {
	// construct a temporary vector from our std::map

	std::vector<std::string> b;
	for (std::map<std::string, creaturesImage *>::iterator i = backgrounds.begin(); i != backgrounds.end(); i++)
		b.push_back(i->first);
	return b;
}

creaturesImage *MetaRoom::getBackground(std::string back) {
	// return the first background by default
	if (back.empty()) {
		return firstback;
	}
	
	// if this background name isn't found, return null
	if (backgrounds.find(back) != backgrounds.end()) return 0;
	
	// otherwise, return the relevant background
	return backgrounds[back];
}
	
MetaRoom::~MetaRoom() {
	// delete the rooms
	for (std::vector<Room *>::iterator i = rooms.begin(); i != rooms.end(); i++) {
		delete *i;
	}

	// zap the background image references
	for (std::map<std::string, creaturesImage *>::iterator i = backgrounds.begin(); i != backgrounds.end(); i++) {
		world.gallery.delImage(i->second);
	}
}

unsigned int MetaRoom::addRoom(Room *r) {
	// add to both our local list and the global list
	rooms.push_back(r);
	world.map.rooms.push_back(r);

	// set the id and return
	r->id = world.map.room_base++;
	return r->id;
}

/* vim: set noet: */
