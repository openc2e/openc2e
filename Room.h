/*
 *  Room.h
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

#ifndef _C2E_ROOM_H
#define _C2E_ROOM_H

#include <map>
#include <set>
#include <utility>
#include "physics.h"
#include <iostream>
#include <algorithm>

using std::cerr;

#define CA_COUNT 20

struct RoomDoor {
	class Room *first, *second;
	unsigned short perm;
};

class Room {
public:
	std::map<Room *,RoomDoor *> doors;
	std::set<Room *> nearby;
	unsigned int x_left, x_right, y_left_ceiling, y_right_ceiling;
	unsigned int y_left_floor, y_right_floor;
	
	std::vector<std::pair<unsigned int, unsigned int> > floorpoints;

	Line left, right, top, bot;
	
	unsigned int type;
	unsigned int id;
	class MetaRoom *metaroom;

	float ca[CA_COUNT], catemp[CA_COUNT];

	bool containsPoint(float x, float y) {	
		if (x > (float)x_right || x < (float)x_left) { return false; }
		if (bot.pointAtX(x).y < y) { return false; }
		if (top.pointAtX(x).y > y) { return false; }
		return true;
	}

	bool containsPoint(Point p) { return containsPoint(p.x, p.y); }
	
	Room();
	Room(unsigned int x_l, unsigned int x_r, unsigned int y_l_t, unsigned int y_r_t, unsigned int y_l_b, unsigned int y_r_b);
	void tick();
	void postTick();
	void resetTick();

	void renderBorders(class Surface *surf, int xoffset, int yoffset, unsigned int col);
};

#endif
/* vim: set noet: */
