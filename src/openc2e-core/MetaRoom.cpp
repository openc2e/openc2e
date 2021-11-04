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

#include "Backend.h"
#include "Engine.h"
#include "Map.h"
#include "Room.h"
#include "World.h"
#include "common/throw_ifnot.h"
#include "fileformats/blkImage.h"
#include "imageManager.h"

#include <assert.h>
#include <memory>

void MetaRoom::addBackground(std::string back) {
	THROW_IFNOT(!back.empty());
	// TODO: cadv adds backgrounds which have already been added as the default, look into this,
	// should we preserve the default once extra backgrounds have been added and change this to
	// a throw_ifnot?
	if (backgrounds.find(back) != backgrounds.end())
		return;

	// load the background sprite
	std::shared_ptr<creaturesImage> backsprite = world.gallery->getBackground(back, wid, hei);
	unsigned int totalwidth = std::max(wid, backsprite->width(0));
	unsigned int totalheight = std::max(hei, backsprite->height(0));

	// store the background
	backgrounds[back] = backsprite;
	if (!firstback.size()) {
		// set the first background
		firstback = back;
		fullwid = totalwidth;
		fullhei = totalheight;
		current_background = back;
	} else {
		// make sure other backgrounds are the same size
		if (engine.gametype == "sm")
			return; // TODO: seamonkeys fails the background size checks :/
		assert(totalwidth == fullwid);
		assert(totalheight == fullhei);
	}
}

std::vector<std::string> MetaRoom::backgroundList() {
	// construct a temporary vector from our std::map

	std::vector<std::string> b;
	for (auto& background : backgrounds)
		b.push_back(background.first);
	return b;
}

std::shared_ptr<creaturesImage> MetaRoom::getCurrentBackground() {
	return backgrounds[current_background];
}

std::string MetaRoom::getCurrentBackgroundName() const {
	return current_background;
}

bool MetaRoom::hasBackground(std::string back) const {
	return backgrounds.find(back) != backgrounds.end();
}

void MetaRoom::setBackground(std::string back) {
	// TODO: supposedly the current background is per-camera
	// TODO: camera transition effects??
	assert(hasBackground(back));
	current_background = back;
}

MetaRoom::~MetaRoom() {
	// we hold the only strong reference to our contained rooms, so they'll be auto-deleted
}

std::shared_ptr<Room> MetaRoom::nextFloorFromPoint(float x, float y) {
	std::shared_ptr<Room> closest_up, closest_down;
	float dist_down = -1, dist_up = -1;
	for (auto& room : rooms) {
		if (!room->bottom.containsX(x))
			continue;
		float dist = room->bottom.pointAtX(x).y - y; // down is positive
		float absdist = fabs(dist);
		if (dist >= 0 && (absdist < dist_down || dist_down < 0)) {
			dist_down = absdist;
			closest_down = room;
		} else if (dist < 0 && (absdist < dist_up || dist_up < 0)) {
			dist_up = absdist;
			closest_up = room;
		}
	}
	if (closest_down)
		return closest_down;
	if (closest_up)
		return closest_up;
	return std::shared_ptr<Room>();
}

unsigned int MetaRoom::addRoom(std::shared_ptr<Room> r) {
	// add to both our local list and the global list
	rooms.push_back(r);
	world.map->rooms.push_back(r);

	// set the id and return
	r->id = world.map->room_base++;
	return r->id;
}

std::shared_ptr<Room> MetaRoom::roomAt(float _x, float _y) {
	if (wraps) {
		if (_x > (int)xloc + (int)wid)
			_x -= wid;
		else if (_x < (int)xloc)
			_x += wid;
	}

	for (auto r : rooms) {
		if (r->containsPoint(_x, _y))
			return r;
	}

	return std::shared_ptr<Room>();
}

std::vector<std::shared_ptr<Room> > MetaRoom::roomsAt(float _x, float _y) {
	if (wraps) {
		if (_x > (int)xloc + (int)wid)
			_x -= wid;
		else if (_x < (int)xloc)
			_x += wid;
	}

	std::vector<std::shared_ptr<Room> > ourlist;

	for (auto r : rooms) {
		if (r->containsPoint(_x, _y))
			ourlist.push_back(r);
	}

	return ourlist;
}

/* vim: set noet: */
