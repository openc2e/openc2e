/*
 *  Map.cpp
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

#include "Map.h"
#include "openc2e.h"
#include <iostream>

void Map::Reset() {
	currroom = 0;
	for (std::vector<MetaRoom *>::iterator i = metarooms.begin(); i != metarooms.end(); i++) {
		delete *i;
	}
	metarooms.clear();
	// todo: metarooms should be responsible for deleting rooms, so use the following instead of clear:
	// assert(rooms.empty());
	rooms.clear();
}

void Map::SetMapDimensions(unsigned int w, unsigned int h) {
	// todo: check for outlying metarooms
	width = w;
	height = h;
}

void Map::SetCurrentMetaRoom(unsigned int room) {
	assert(room < metarooms.size());
	currroom = room;
}

int Map::addMetaRoom(MetaRoom *m) {
	// todo: check if it's outlying
	metarooms.push_back(m);
	m->id = metarooms.size() - 1;
	return m->id;
}

MetaRoom *Map::getMetaRoom(unsigned int room) {
	assert(room < metarooms.size());
	return metarooms[room];
}

MetaRoom *Map::getCurrentMetaRoom() {
	assert(currroom < metarooms.size());
	return metarooms[currroom];
}

Room *Map::getRoom(unsigned int r) {
	assert(r < rooms.size());
	return rooms[r];
}

unsigned int Map::getMetaRoomCount() {
	return metarooms.size();
}

unsigned int Map::getRoomCount() {
	return rooms.size();
}

MetaRoom *Map::metaRoomAt(unsigned int _x, unsigned int _y) {
	for (std::vector<MetaRoom *>::iterator i = metarooms.begin(); i != metarooms.end(); i++) {
		MetaRoom *r = *i;
		if ((_x >= r->x()) && (_y >= r->y()))
			if ((_x <= (r->x() + r->width())) && (_y <= (r->y() + r->height())))
				return r;
	}
	return 0;
}

Room *Map::roomAt(unsigned int _x, unsigned int _y) {
	for (std::vector<Room *>::iterator i = rooms.begin(); i != rooms.end(); i++) {
		Room *r = *i;
		if ((_x >= r->x_left) && (_x <= r->x_right)) {
			// TODO: we should be able to catch most stuff by checking bounding box
			unsigned int xdiff = r->x_left - r->x_right;
			unsigned int xoffset = _x - r->x_left;
		
			// floors
			unsigned int ydiff = r->y_right_floor - r->y_left_floor;
			float m = ydiff / xdiff;
			unsigned int ypoint = (xoffset * m) + r->y_left_floor;
			//if (ypoint > _y) {
				// ceiling
				ydiff = r->y_left_ceiling - r->y_right_ceiling;
				m = ydiff / xdiff;
				ypoint = (xoffset * m) + r->y_left_ceiling;
				if (ypoint < _y)
					return r;
			//}
		}
	}
	return 0;
}

