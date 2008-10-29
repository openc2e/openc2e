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

Vehicle::Vehicle(unsigned int family, unsigned int genus, unsigned int species, unsigned int plane,
		std::string spritefile, unsigned int firstimage, unsigned int imagecount) :
		CompoundAgent(family, genus, species, plane, spritefile, firstimage, imagecount) {
	capacity = 0;
	bump = 0;

	cabinleft = 0; cabintop = 0; cabinright = getWidth(); cabinbottom = getHeight();
	cabinplane = 1; // TODO: is this sane? seems to be the default in c2e
}

Vehicle::Vehicle(std::string spritefile, unsigned int firstimage, unsigned int imagecount) : CompoundAgent(spritefile, firstimage, imagecount) {
	capacity = 0;
	bump = 0;

	// TODO: set cabin bounds? we don't know width/height at this point..
	cabinplane = 1;
}

void Vehicle::tick() {
	CompoundAgent::tick();
	if (paused) return;

	// move by xvec/yvec!
	moveTo(x + xvec.getInt() / 256.0, y + yvec.getInt() / 256.0);
}

void Vehicle::carry(AgentRef passenger) {
	// TODO: 'return' is not a good idea here, because the callung function already does stuff

	if (passenger->carriedby) return; // TODO: change to assert?
	if (passenger->invehicle) return; // TODO: change to assert?

	int cabinwidth = cabinright - cabinleft;
	int cabinheight = cabinbottom - cabintop;

	if (engine.version > 2) {
		// reject if passenger is too big
		if ((int)passenger->getWidth() > cabinwidth) return;
		if ((int)passenger->getHeight() > cabinheight) return;
	}

	// push into our cabin
	// TODO: should we use moveTo here?
	if (passenger->x + passenger->getWidth() > (x + cabinright)) passenger->x = x + cabinright - passenger->getWidth();
	if (passenger->x < (x + cabinleft)) passenger->x = x + cabinleft;
	if (engine.version > 1) {
		// TODO: not sure if this is good for too-high agents, if it's possible for them to exist (see comment above)
		if (passenger->y + passenger->getHeight() > (y + cabinbottom)) passenger->y = y + cabinbottom - passenger->getHeight();
		if (passenger->y < (y + cabintop)) passenger->y = y + cabintop;
	} else {
		passenger->y = y + cabinbottom - passenger->getHeight();
	}

	passengers.push_back(passenger);
	passenger->invehicle = this;
	
	if (engine.version >= 3)
		passenger->queueScript(121, this); // Vehicle Pickup, TODO: is this valid call?
}

void Vehicle::drop(AgentRef passenger) {
	std::vector<AgentRef>::iterator i = std::find(passengers.begin(), passengers.end(), passenger);
	assert(i != passengers.end());
	assert(passenger->invehicle == AgentRef(this));
	passengers.erase(i);

	passenger->beDropped();
	assert(passenger->invehicle != AgentRef(this));

	if (engine.version >= 3)
		passenger->queueScript(122, this); // Vehicle Drop, TODO: is this valid call?
}

void Vehicle::adjustCarried(float xoffset, float yoffset) {
	Agent::adjustCarried(xoffset, yoffset);

	for (std::vector<AgentRef>::iterator i = passengers.begin(); i != passengers.end(); i++) {
		if (!(*i)) continue; // TODO: muh
		(*i)->moveTo((*i)->x + xoffset, (*i)->y + yoffset);
	}
}

void Vehicle::kill() {
	// TODO: sane?
	while (passengers.size() > 0) {
		if (passengers[0]) dropCarried(passengers[0]);
		else passengers.erase(passengers.begin());
	}
	
	Agent::kill();
}

/* vim: set noet: */
