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

	Point ul, ur, bl, br;
	
	Line left, right, top, bot;
	
	unsigned int type;
	unsigned int id;
	class MetaRoom *metaroom;

	float ca[CA_COUNT];

	bool containsPoint(float x, float y) {	
		if (x > (float)x_right || x < (float)x_left) { return false; }
		if (bot.pointAtX(x).y < y) { return false; }
		if (top.pointAtX(x).y > y) { return false; }
		return true;
	}
	
	Room(
			unsigned int x_l, 
			unsigned int x_r,
			unsigned int y_l_t,
			unsigned int y_r_t,
			unsigned int y_l_b,
			unsigned int y_r_b
			) {

		if (x_l > x_r) std::swap(x_l, x_r);
		x_left = x_l;
		x_right = x_r;
		if (y_l_b < y_l_t) std::swap(y_l_b, y_l_t);
		if (y_r_b < y_r_t) std::swap(y_r_b, y_r_t);
		y_left_ceiling = y_l_t;
		y_right_ceiling = y_r_t;
		y_left_floor = y_l_b;
		y_right_floor = y_r_b;

		ul = Point(x_l, y_l_t);
		ur = Point(x_r, y_r_t);
		bl = Point(x_l, y_l_b);
		br = Point(x_r, y_r_b);

		left = Line(ul, bl);
		right = Line(ur, br);
		top = Line(ul, ur);
		bot = Line(bl, br);

		for (int i = 0; i < CA_COUNT; i++)
			ca[i] = 0;
	}

	Room();
	void tick();
};

#endif
/* vim: set noet: */
