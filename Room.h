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

#include <vector>

class Room {
public:
	std::vector<Room *> neighbours;
	unsigned int x_left, x_right, y_left_ceiling, y_right_ceiling, y_left_floor, y_right_floor;
	unsigned int type;
	unsigned int id;
};

#endif
