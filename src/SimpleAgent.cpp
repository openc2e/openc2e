/*
 *  SimpleAgent.cpp
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

#include "SimpleAgent.h"
#include "DullPart.h"

SimpleAgent::SimpleAgent(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
			 std::string spritefile, unsigned int firstimage, unsigned int imagecount) :
	Agent(family, genus, species, plane) {
	dullpart = new DullPart(this, 0, spritefile, firstimage, 0, 0, 0);
}

SimpleAgent::~SimpleAgent() {
	delete dullpart;
}

void SimpleAgent::setZOrder(unsigned int plane) {
	Agent::setZOrder(plane);
	dullpart->zapZOrder();
	dullpart->addZOrder();
}

CompoundPart *SimpleAgent::part(unsigned int id) {
	return dullpart;
}

void SimpleAgent::tick() {
	if (!paused) dullpart->tick();
	Agent::tick();
}

/* vim: set noet: */
