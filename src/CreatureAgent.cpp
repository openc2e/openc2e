/*
 *  CreatureAgent.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Dec 09 2006.
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

#include "CreatureAgent.h"
#include "Creature.h"

CreatureAgent::CreatureAgent() {
	creature = 0;

	walking = false;
	approaching = false;
	direction = 0;
}

CreatureAgent::~CreatureAgent() {
	if (creature)
		delete creature;
}

void CreatureAgent::setCreature(Creature *c) {
	assert(c);
	creature = c;

	getAgent()->slots[0] = creature->getGenome();

	unsigned short species = (creature->isFemale() ? 2 : 1);
	// category needs to be set, so call setClassifier now
	getAgent()->setClassifier(getAgent()->family, getAgent()->genus, species);
}

void CreatureAgent::tick() {
	creature->tick();
}

void CreatureAgent::startWalking() {
	walking = true;
	approaching = false;
}

void CreatureAgent::stopWalking() {
	walking = false;
}

void CreatureAgent::approach(AgentRef it) {
	assert(it);

	walking = false;
	approaching = true;
	approachtarget = it;
}

/* vim: set noet: */
