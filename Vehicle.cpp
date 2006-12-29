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

void Vehicle::tick() {
	CompoundAgent::tick();
	if (paused) return;

	// move by xvec/yvec!
	x += xvec.getInt() / 256.0;
	y += yvec.getInt() / 256.0;
}

/* vim: set noet: */
