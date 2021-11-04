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

#include "serfwd.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

class creaturesImage;
class Map;

class MetaRoom {
  protected:
	friend Map;
	FRIEND_SERIALIZE(MetaRoom)
	unsigned int xloc, yloc, wid, hei, fullwid, fullhei;
	std::map<std::string, std::shared_ptr<creaturesImage> > backgrounds;
	std::string firstback;
	std::string current_background;
	bool wraps;

	MetaRoom() {}

  public:
	std::vector<std::shared_ptr<class Room> > rooms;

	unsigned int x() { return xloc; }
	unsigned int y() { return yloc; }
	unsigned int width() { return wid; }
	unsigned int height() { return hei; }
	unsigned int fullwidth() { return fullwid; }
	unsigned int fullheight() { return fullhei; }
	bool wraparound() { return wraps; }
	void setWraparound(bool w) { wraps = !!w; }

	unsigned int addRoom(std::shared_ptr<class Room>);

	void addBackground(std::string);
	bool hasBackground(std::string) const;
	void setBackground(std::string);
	std::shared_ptr<creaturesImage> getCurrentBackground();
	std::string getCurrentBackgroundName() const;
	std::vector<std::string> backgroundList();

	std::shared_ptr<Room> nextFloorFromPoint(float x, float y);

	std::shared_ptr<Room> roomAt(float x, float y);
	std::vector<std::shared_ptr<Room> > roomsAt(float x, float y);

	std::string music;

	unsigned int id;

	~MetaRoom();
};

#endif
/* vim: set noet: */
