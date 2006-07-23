/*
 *  AgentHelpers.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Jul 23 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

bool agentIsVisible(Agent *seeing, Agent *a, float ownerx, float ownery, MetaRoom *ownermeta, Room *ownerroom) {
	assert(ownermeta && ownerroom);

	// verify we're in the same metaroom as owner, and in a room
	float thisx = a->x + (a->getWidth() / 2.0f);
	float thisy = a->y + (a->getHeight() / 2.0f);
	MetaRoom *m = world.map.metaRoomAt(thisx, thisy);
	if (m != ownermeta) return false;
	Room *r = world.map.roomAt(thisx, thisy);
	if (!r) return false;
		
	// compare squared distance with range
	double deltax = thisx - ownerx; deltax *= deltax;
	double deltay = thisy - ownery; deltay *= deltay;
	if ((deltax + deltay) > (seeing->range * seeing->range)) return false;

	// do the actual visibiltiy check using a line between centers
	Point src(ownerx, ownery), dest(thisx, thisy);
	Line dummywall; unsigned int dummydir;
	Room *newroom = ownerroom;
	world.map.collideLineWithRoomSystem(src, dest, newroom, src, dummywall, dummydir, seeing->perm);
	if (src != dest) return false;

	return true;
}

bool agentIsVisible(Agent *seeing, Agent *dest) {
	float ownerx = (seeing->x + (seeing->getWidth() / 2.0f));
	float ownery = (seeing->y + (seeing->getHeight() / 2.0f));
	MetaRoom *ownermeta = world.map.metaRoomAt(ownerx, ownery);
	Room *ownerroom = world.map.roomAt(ownerx, ownery);
	if (!ownermeta) return false; if (!ownerroom) return false;

	return agentIsVisible(seeing, dest, ownerx, ownery, ownermeta, ownerroom);
}

std::vector<boost::shared_ptr<Agent> > getVisibleList(Agent *seeing, unsigned char family, unsigned char genus, unsigned short species) {
	std::vector<boost::shared_ptr<Agent> > agents;

	float ownerx = (seeing->x + (seeing->getWidth() / 2.0f));
	float ownery = (seeing->y + (seeing->getHeight() / 2.0f));
	MetaRoom *ownermeta = world.map.metaRoomAt(ownerx, ownery);
	Room *ownerroom = world.map.roomAt(ownerx, ownery);
	if (!ownermeta) return agents; if (!ownerroom) return agents;
	
	for (std::list<boost::shared_ptr<Agent> >::iterator i
			= world.agents.begin(); i != world.agents.end(); i++) {
		boost::shared_ptr<Agent> a = (*i);
		if (!a) continue;
		
		// TODO: if owner is a creature, skip stuff with invisible attribute
		
		// verify species/genus/family
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		if (agentIsVisible(seeing, a.get(), ownerx, ownery, ownermeta, ownerroom))	
			agents.push_back(a);
	}

	return agents;
}


