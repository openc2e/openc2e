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

#include "openc2e.h"
#include <string>
#include <vector>
#include <map>

class creaturesImage;

class MetaRoom {
protected:
	FRIEND_SERIALIZE(MetaRoom)
	unsigned int xloc, yloc, wid, hei, fullwid, fullhei;
	std::map<std::string, shared_ptr<creaturesImage> > backgrounds;
	shared_ptr<creaturesImage> firstback;
	bool wraps;
	
	MetaRoom() { }

public:
	std::vector<shared_ptr<class Room> > rooms;

	unsigned int x() { return xloc; }
	unsigned int y() { return yloc; }
	unsigned int width() { return wid; }
	unsigned int height() { return hei; }
	unsigned int fullwidth() { return fullwid; }
	unsigned int fullheight() { return fullhei; }
	bool wraparound() { return wraps; }
	void setWraparound(bool w) { wraps = !!w; }

	unsigned int addRoom(shared_ptr<class Room>);
	void addBackground(std::string, shared_ptr<creaturesImage> = shared_ptr<creaturesImage>());
	shared_ptr<creaturesImage> getBackground(std::string);
	std::vector<std::string> backgroundList();

	shared_ptr<Room> nextFloorFromPoint(float x, float y);

	shared_ptr<Room> roomAt(float x, float y);
	std::vector<shared_ptr<Room> > roomsAt(float x, float y);

	std::string music;

	unsigned int id;

	MetaRoom(int _x, int _y, int width, int height, const std::string &back, shared_ptr<creaturesImage> = shared_ptr<creaturesImage>(), bool wrap = false);
	~MetaRoom();
};

#endif
/* vim: set noet: */
