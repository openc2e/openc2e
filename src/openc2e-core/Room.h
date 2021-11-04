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

#pragma once

#include "physics.h"

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

constexpr int CA_COUNT = 20;

class Map;
class Room;
class RenderTarget;

struct RoomDoor {
	int perm;
};

class Room {
  private:
	friend Map;
	std::map<std::weak_ptr<Room>, RoomDoor, std::owner_less<std::weak_ptr<Room>>> doors;

  public:
	unsigned int x_left, x_right, y_left_ceiling, y_right_ceiling;
	unsigned int y_left_floor, y_right_floor;

	std::vector<std::pair<unsigned int, unsigned int>> floorpoints;

	Line left, right, top, bottom;
	int type = 0;

	// Creatures 2
	int temp = 0, lite = 0, radn = 0, ontr = 0, intr = 0, pres = 0, hsrc = 0, lsrc = 0, rsrc = 0, psrc = 0;
	int floorvalue = 0, dropstatus = 0;
	int windx, windy;

	std::string music;

	unsigned int id;

	float ca[CA_COUNT], catemp[CA_COUNT];

	bool containsPoint(float x, float y);
	bool containsPoint(Point p);

	float floorYatX(float x);
	std::vector<std::pair<std::shared_ptr<Room>, RoomDoor>> getDoors();

	Room();
	Room(unsigned int x_l, unsigned int x_r, unsigned int y_l_t, unsigned int y_r_t, unsigned int y_l_b, unsigned int y_r_b);
	void tick();
	void postTick();
	void resetTick();

	void renderBorders(RenderTarget* surf, int xoffset, int yoffset, unsigned int col);
};

/* vim: set noet: */
