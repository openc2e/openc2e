/*
 *  World.cpp
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

#include "World.h"
#include "caosVM.h" // for setupCommandPointers()
#include "SimpleAgent.h"
#include <limits.h> // for MAXINT

World world;

World::World() {
}

// annoyingly, if we put this in the constructor, imageGallery isn't available yet
void World::init() {
	theHand = new SimpleAgent(2, 1, 1, UINT_MAX, 0, 0);
	((SimpleAgent *)theHand)->setImage("hand");
	addAgent(theHand);
}

caosVM *World::getVM(Agent *a) {
	if (vmpool.empty()) {
		return new caosVM(a);
	} else {
		caosVM *x = vmpool.back();
		vmpool.pop_back();
		x->setOwner(a);
		return x;
	}
}

void World::freeVM(caosVM *v) {
	v->setOwner(0);
	vmpool.push_back(v);
}

void World::addAgent(Agent *a) {
	agents.insert(a);
}

void World::tick() {
	for (std::multiset<Agent *, agentzorder>::iterator i = agents.begin(); i != agents.end(); i++) {
		(**i).tick();
	}
	// todo: tick rooms
}

Agent *World::agentAt(unsigned int x, unsigned int y, bool needs_activateable) {
	Agent *temp = 0;

	// we're looking for the *last* agent in the set which is at this location (ie, topmost)
	// TODO: this needs to check if agents are USEFUL (ie, not background scenery etc)
	for (std::multiset<Agent *, agentzorder>::iterator i = agents.begin(); i != agents.end(); i++) {
		if ((*i)->x <= x) if ((*i)->y <= y) if (((*i) -> x + (*i)->getWidth()) >= x) if (((*i) -> y + (*i)->getHeight()) >= y)
			if ((*i) != theHand)
				if ((!needs_activateable) || (*i)->activateable)
					temp = *i;
	}
	
	return temp;
}
