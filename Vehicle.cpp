/*
 *  Vehicle.cpp
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

#include "Vehicle.h"
#include "Engine.h"
#include "World.h"
#include "MetaRoom.h"

Vehicle::Vehicle(unsigned int family, unsigned int genus, unsigned int species, unsigned int plane,
		std::string spritefile, unsigned int firstimage, unsigned int imagecount) :
		CompoundAgent(family, genus, species, plane, spritefile, firstimage, imagecount) {
	capacity = 0;
	bump = 0;

	cabinleft = (int)x;
	cabinright = (int)x + getWidth();
	cabintop = (int)y;
	cabinbottom = (int)y + getHeight();
}

Vehicle::Vehicle(std::string spritefile, unsigned int firstimage, unsigned int imagecount) : CompoundAgent(spritefile, firstimage, imagecount) {
	capacity = 0;
	bump = 0;

	// TODO: set cabin bounds?
}

void Vehicle::tick() {
	CompoundAgent::tick();
	if (paused) return;

	if (xvec.getInt() != 0 && engine.version == 1) {
		// handle magical C1 vehicle world wrapping, if necessary
		
		MetaRoom *m = world.map.metaRoomAt(x, y);
		if (!m) {
			std::cout << "BUG: moving c1 vehicle " << identify() << " is out of room system at " << x << ", " << y << ", so killing it" << std::endl;
			kill();
			return;
		}

		if (x + (xvec.getInt() / 256.0) < m->x()) {
			assert(xvec.getInt() < 0);
			x += m->width();
		} else if (x + (xvec.getInt() / 256.0) > m->x() + m->width()) {
			assert(xvec.getInt() > 0);
			x -= m->width();
		}
	}
	
	// move by xvec/yvec!
	x += xvec.getInt() / 256.0;
	y += yvec.getInt() / 256.0;
}

/* vim: set noet: */
