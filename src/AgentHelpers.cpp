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

#include "Agent.h"
#include "Map.h"
#include "MetaRoom.h"
#include "Room.h"
#include "World.h"

#include <cassert>
#include <memory>

bool agentIsVisible(Agent* seeing, Agent* a, float ownerx, float ownery, MetaRoom* ownermeta, std::shared_ptr<Room> ownerroom) {
	assert(ownermeta && ownerroom);

	if (seeing == a)
		return false;

	// verify we're in the same metaroom as owner, and in a room
	float thisx = a->x + (a->getWidth() / 2.0f);
	float thisy = a->y + (a->getHeight() / 2.0f);
	MetaRoom* m = world.map->metaRoomAt(thisx, thisy);
	if (m != ownermeta)
		return false;
	std::shared_ptr<Room> r = world.map->roomAt(thisx, thisy);
	if (!r)
		return false;

	// compare squared distance with range
	double deltax = thisx - ownerx;
	deltax *= deltax;
	double deltay = thisy - ownery;
	deltay *= deltay;
	if ((deltax + deltay) > (seeing->range * seeing->range))
		return false;

	// do the actual visibiltiy check using a line between centers
	Point src(ownerx, ownery), dest(thisx, thisy);
	Line dummywall;
	unsigned int dummydir;
	std::shared_ptr<Room> newroom = ownerroom;
	world.map->collideLineWithRoomSystem(src, dest, newroom, src, dummywall, dummydir, seeing->perm);
	if (src != dest)
		return false;

	return true;
}

bool agentIsVisible(Agent* seeing, Agent* dest) {
	float ownerx = (seeing->x + (seeing->getWidth() / 2.0f));
	float ownery = (seeing->y + (seeing->getHeight() / 2.0f));
	MetaRoom* ownermeta = world.map->metaRoomAt(ownerx, ownery);
	std::shared_ptr<Room> ownerroom = world.map->roomAt(ownerx, ownery);
	if (!ownermeta)
		return false;
	if (!ownerroom)
		return false;

	return agentIsVisible(seeing, dest, ownerx, ownery, ownermeta, ownerroom);
}

std::vector<std::shared_ptr<Agent> > getVisibleList(Agent* seeing, unsigned char family, unsigned char genus, unsigned short species) {
	std::vector<std::shared_ptr<Agent> > agents;

	float ownerx = (seeing->x + (seeing->getWidth() / 2.0f));
	float ownery = (seeing->y + (seeing->getHeight() / 2.0f));
	MetaRoom* ownermeta = world.map->metaRoomAt(ownerx, ownery);
	std::shared_ptr<Room> ownerroom = world.map->roomAt(ownerx, ownery);
	if (!ownermeta)
		return agents;
	if (!ownerroom)
		return agents;

	for (auto& agent : world.agents) {
		std::shared_ptr<Agent> a = agent;
		if (!a)
			continue;

		// TODO: if owner is a creature, skip stuff with invisible attribute

		// verify species/genus/family
		if (species && species != a->species)
			continue;
		if (genus && genus != a->genus)
			continue;
		if (family && family != a->family)
			continue;

		if (agentIsVisible(seeing, a.get(), ownerx, ownery, ownermeta, ownerroom))
			agents.push_back(a);
	}

	return agents;
}

bool agentsTouching(Agent* first, Agent* second) {
	assert(first && second);

	// TODO: c2e docs say it only checks if bounding lines overlap, implement it like that?

	// this check should probably be integrated into line overlap check?
	if (first == second)
		return false;

	if (first->x < second->x) {
		if ((first->x + first->getWidth()) < second->x)
			return false;
	} else {
		if ((second->x + second->getWidth()) < first->x)
			return false;
	}

	if (first->y < second->y) {
		if ((first->y + first->getHeight()) < second->y)
			return false;
	} else {
		if ((second->y + second->getHeight()) < first->y)
			return false;
	}

	return true;
}

std::shared_ptr<Room> roomContainingAgent(AgentRef agent) {
	MetaRoom* m = world.map->metaRoomAt(agent->x, agent->y);
	if (!m)
		return std::shared_ptr<Room>();
	return m->roomAt(agent->x + (agent->getWidth() / 2.0f), agent->y + (agent->getHeight() / 2.0f));
}
