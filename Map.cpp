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
	MetaRoom *m = metaRoomAt(_x, _y);
	if (!m) return 0;
	for (std::vector<Room *>::iterator i = m->rooms.begin(); i != m->rooms.end(); i++) {
		Room *r = *i;
//		std::cerr << (void *)r << " -> contains " << _x << "," << _y << std::endl;
		if (r->containsPoint(_x, _y)) return r;
	}
	return 0;
}

