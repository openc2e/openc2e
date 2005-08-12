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
#include "World.h"
#include "creaturesImage.h"
#include <assert.h>

MetaRoom::MetaRoom(int _x, int _y, int _width, int _height, std::string back) {
	xloc = _x; yloc = _y; wid = _width; hei = _height;
	background = (blkImage *)gallery.getImage(back + ".blk");
	assert(background);
}

MetaRoom::~MetaRoom() {
	for (std::vector<Room *>::iterator i = rooms.begin(); i != rooms.end(); i++) {
		delete *i;
	}
	gallery.delImage(background);
}

unsigned int MetaRoom::addRoom(Room *r) {
	rooms.push_back(r);
	world.map.rooms.push_back(r);
	r->id = world.map.rooms.size() - 1;
	return r->id;
}
/* vim: set noet: */
