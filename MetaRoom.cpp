/*
 *  MetaRoom.cpp
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

#include "MetaRoom.h"
#include "World.h"
#include "creaturesImage.h"
#include <assert.h>
#include "SDLBackend.h"

MetaRoom::MetaRoom(int _x, int _y, int _width, int _height, std::string back) {
	xloc = _x; yloc = _y; wid = _width; hei = _height;
	if (back.empty()) {
		background = 0;
		backsurfs = 0;
	} else {
		background = (blkImage *)world.gallery.getImage(back + ".blk");
		caos_assert(background);

		// TODO: This probably doesn't belong in the constructor. Or, perhaps, in MetaRoom at all.
		unsigned int rmask, gmask, bmask;
		if (background->is565()) {
			rmask = 0xF800; gmask = 0x07E0; bmask = 0x001F;
		} else {
			rmask = 0x7C00; gmask = 0x03E0; bmask = 0x001F;
		}

		backsurfs = new SDL_Surface *[background->numframes()];
		assert(backsurfs);
		for (unsigned int i = 0; i < background->numframes(); i++) {
			backsurfs[i] = SDL_CreateRGBSurfaceFrom(background->data(i), background->width(i),
					background->height(i), 16 /* depth */, background->width(i) * 2 /* pitch */,
					rmask, gmask, bmask, 0);
			assert(backsurfs[i]);
		}
	}
}

MetaRoom::~MetaRoom() {
	for (std::vector<Room *>::iterator i = rooms.begin(); i != rooms.end(); i++) {
		delete *i;
	}
	world.gallery.delImage(background);
}

unsigned int MetaRoom::addRoom(Room *r) {
	rooms.push_back(r);
	world.map.rooms.push_back(r);
	r->id = world.map.room_base++;
	return r->id;
}
/* vim: set noet: */
