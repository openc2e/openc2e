/*
 *  MetaRoom.h
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

#ifndef _C2E_METAROOM_H
#define _C2E_METAROOM_H

#include "Room.h"
#include "blkImage.h"
#include <string>
#include <vector>

class MetaRoom {
protected:
	unsigned int xloc, yloc, wid, hei;
	blkImage *background;
	
public:
	std::vector<Room *> rooms;

	unsigned int x() { return xloc; }
	unsigned int y() { return yloc; }
	unsigned int width() { return wid; }
	unsigned int height() { return hei; }
	blkImage *backImage() { return background; }

	unsigned int addRoom(Room *);

	unsigned int id;

	MetaRoom(int _x, int _y, int width, int height, std::string back);
	~MetaRoom();
};

#endif
/* vim: set noet: */
