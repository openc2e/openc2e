/*
 *  Vehicle.h
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

#ifndef __VEHICLE_H
#define __VEHICLE_H

#include "CompoundAgent.h"
#include "openc2e.h"

class Vehicle : public CompoundAgent {
protected:
	int cabinleft, cabintop, cabinright, cabinbottom;
	unsigned int capacity;

public:
	Vehicle(unsigned int family, unsigned int genus, unsigned int species, unsigned int plane,
		std::string spritefile, unsigned int firstimage, unsigned int imagecount);

	void setCabinRect(int l, int t, int r, int b) { cabinleft = l; cabintop = t; cabinright = r; cabinbottom = b; }
	void setCapacity(unsigned int c) { capacity = c; }
};

#endif
/* vim: set noet: */
