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
#include "Catalogue.h"
#include "Engine.h"
#include <cassert>

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

	getAgent()->genome_slots[0] = creature->getGenome();

	unsigned short species = (creature->isFemale() ? 2 : 1);
	// category needs to be set, so call setClassifier now
	getAgent()->setClassifier(getAgent()->family, getAgent()->genus, species);

	// TODO: this should be in a seperate function
	if (engine.version < 3) {
		switch (getAgent()->genus) {
			case 1:
				if (creature->isFemale())
					getAgent()->setVoice("female");
				else
					getAgent()->setVoice("male");
				break;
			case 2:
				getAgent()->setVoice("grendel");
				break;
			case 3:
				getAgent()->setVoice("ettin");
				break;
		}
	} else {
		std::string tagname;
		switch (getAgent()->genus) {
			case 1: tagname = "Norn"; break;
			case 2: tagname = "Grendel"; break;
			case 3: tagname = "Ettin"; break;
			case 4: tagname = "Geat"; break;
			default: tagname = "Unknown"; break;
		}
		if (creature->isFemale())
			tagname += " Female";
		else
			tagname += " Male";
		// TODO: actually pay attention to age
		tagname += " Embryo";
		if (catalogue.hasTag(tagname))
			getAgent()->setVoice(tagname);
	}
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
