/*
 *  Map.h
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

#ifndef _OPENC2E_MAP_H
#define _OPENC2E_MAP_H

#include "physics.h"
#include "openc2e.h"
#include <vector>

class Room;
class MetaRoom;

class Map {
protected:
	FRIEND_SERIALIZE(Map);
	unsigned int width, height;
	std::vector<MetaRoom *> metarooms;
	std::vector<shared_ptr<Room> > rooms;

	friend class MetaRoom;

public:
	/* Get a room, any room.
	 *
	 * For those times when you really, really need a room.
	 */
	MetaRoom *getFallbackMetaroom() {
		return metarooms.size() == 0 ? NULL : metarooms[0];
	}
	
	unsigned int room_base, metaroom_base;
	
	Map() { width = 0; height = 0; room_base = 0; metaroom_base = 0; }

	void Reset();
	void SetMapDimensions(unsigned int, unsigned int);
	unsigned int getWidth() { return width; }
	unsigned int getHeight() { return height; }

	int addMetaRoom(MetaRoom *);
	MetaRoom *getMetaRoom(unsigned int);
	MetaRoom *getArrayMetaRoom(unsigned int i) { return metarooms[i]; } // TODO: hack!
	
	unsigned int getMetaRoomCount();
	shared_ptr<Room> getRoom(unsigned int);
	unsigned int getRoomCount();

	MetaRoom *metaRoomAt(unsigned int, unsigned int);
	shared_ptr<Room> roomAt(float, float);
	std::vector<shared_ptr<Room> > roomsAt(float, float);

	bool collideLineWithRoomSystem(Point src, Point dest, shared_ptr<Room> &room, Point &where, Line &wall, unsigned int &walldir, int perm);
	bool collideLineWithRoomBoundaries(Point src, Point dest, shared_ptr<Room> room, shared_ptr<Room> &newroom, Point &where, Line &wall, unsigned int &walldir, int perm);

	void tick();
};

#endif
/* vim: set noet: */
