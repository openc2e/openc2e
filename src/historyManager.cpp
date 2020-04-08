/*
 *  historyManager.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed 26 Apr 2006.
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

#include "creatures/genome.h"
#include "historyManager.h"
#include "World.h"
#include "Catalogue.h"
#include "creatures/CreatureAgent.h"
#include "creatures/Creature.h"
#include "Engine.h" // version

#include <boost/format.hpp>

historyevent::historyevent(unsigned int eno, CreatureAgent *c) {
	timestamp = time(NULL);
	eventno = eno;
	worldtick = world.tickcount;
	// TODO: worldname = world.name;
	// TODO: worldmoniker = world.moniker;
	// TODO: networkid = world.username;
	
	if (c) {
		tage = c->getCreature()->getAge();
		stage = c->getCreature()->getStage();
	} else {
		tage = -1;
		stage = baby; // TODO: correct?
	}
}

void monikerData::init(std::string m, shared_ptr<genomeFile> f) {
	moniker = m;
	status = unreferenced;
	warpveteran = false;
	variant = -1;
	gender = -1;
	no_crossover_points = 0;
	no_point_mutations = 0;
	
	assert(f);
	genome = f;

	for (vector<gene *>::iterator i = f->genes.begin(); i != f->genes.end(); i++) {
		if (typeid(*(*i)) == typeid(creatureGenusGene)) {
			// initialize genus
			creatureGenusGene *g = (creatureGenusGene *)(*i);
			genus = g->genus + 1;
			break;
		}
	}
}

historyevent &monikerData::addEvent(unsigned int event, std::string moniker1, std::string moniker2) {
	CreatureAgent *c = 0;
	if (owner) c = dynamic_cast<CreatureAgent *>(owner.get());	
	
	events.push_back(historyevent(event, c));
	events.back().monikers[0] = moniker1;
	events.back().monikers[1] = moniker2;

	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;

		(*i)->queueScript(127, 0, moniker, (int)(events.size() - 1)); // new life event
	}
	
	return events.back();
}

void monikerData::moveToAgent(AgentRef a) {
	assert(status == referenced || status == unreferenced);

	owner = a;
	if (!owner) {
		status = unreferenced;
	} else {
		status = referenced;
	}
}

void monikerData::moveToCreature(AgentRef a) {
	moveToAgent(a);
	
	CreatureAgent *c = dynamic_cast<CreatureAgent *>(owner.get());
	assert(c);
	status = creature;
}

void monikerData::wasBorn() {
	assert(status == creature);
	CreatureAgent *c = dynamic_cast<CreatureAgent *>(owner.get());
	assert(c);
	
	status = borncreature;
	gender = (c->getCreature()->isFemale() ? 2 : 1);
	variant = c->getCreature()->getVariant();
}

void monikerData::hasDied() {
	CreatureAgent *c = dynamic_cast<CreatureAgent *>(owner.get());
	assert(c);
	
	// TODO

	status = dead;
}

monikerstatus monikerData::getStatus() {
	switch (status) {
		case referenced:
		case exported:
		case unreferenced:
			return status;
		
		case creature:
		case borncreature:
			if (owner)
				return status;
			else
				return deadandkilled; // we missed it?! TODO: what's correct behaviour here?
			
		case dead:
			if (owner)
				return dead;
			else
				return deadandkilled;

		default:
			assert(false); // explode!
	}

}

std::string historyManager::newMoniker(shared_ptr<genomeFile> genome) {
	unsigned int genus = 0;
	
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (typeid(*(*i)) == typeid(creatureGenusGene)) {
			// initialize genus
			creatureGenusGene *g = (creatureGenusGene *)(*i);
			genus = g->genus + 1;
			break;
		}
	}
	
	std::string basename = "xxxx";

	if (engine.version > 2) {
		const std::vector<std::string> *extensions = 0;
		std::string tagname = boost::str(boost::format("Moniker Friendly Names %i") % genus);
	
		if (catalogue.hasTag(tagname)) {
			extensions = &catalogue.getTag(tagname);
		} else if (catalogue.hasTag("Moniker Friendly Names")) {
			extensions = &catalogue.getTag("Moniker Friendly Names");
		} else {
			std::cout << "Warning: No \"Moniker Friendly Names\" in catalogue for genus " << genus <<
				", defaulting to 'xxxx' for a moniker friendly name." << std::endl;
		}

		if (extensions) {
			unsigned int i = (int) (extensions->size() * (rand() / (RAND_MAX + 1.0)));
			basename = (*extensions)[i];
		}
	}
	
	std::string newmoniker = world.generateMoniker(basename);
	unsigned int i = 0;
	while (hasMoniker(newmoniker)) { // last-moment sanity check..
		std::string newmoniker = world.generateMoniker(basename);
		i++;
		if (i > 100) // emergency "don't go into an infinite loop handling"
			throw creaturesException("Couldn't generate a moniker we don't already have!");
	}

	if (engine.version > 2)
		newmoniker = "001-" + newmoniker; // TODO: bad hack we should use the generation number here!

	monikers[newmoniker].init(newmoniker, genome);
	return newmoniker;
}

bool historyManager::hasMoniker(std::string s) {
	std::map<std::string, monikerData>::iterator i = monikers.find(s);
	if (i == monikers.end()) return false;
	else return true;
}

monikerData &historyManager::getMoniker(std::string s) {
	std::map<std::string, monikerData>::iterator i = monikers.find(s);
	if (i == monikers.end()) throw creaturesException("getMoniker was called with a non-existant moniker");
	return i->second;
}

std::string historyManager::findMoniker(shared_ptr<genomeFile> g) {
	for (std::map<std::string, monikerData>::iterator i = monikers.begin(); i != monikers.end(); i++) {
		if (i->second.genome.lock() == g) return i->first;
	}

	return "";
}

std::string historyManager::findMoniker(AgentRef a) {
	for (std::map<std::string, monikerData>::iterator i = monikers.begin(); i != monikers.end(); i++) {
		if (i->second.owner == a) return i->first;
	}

	return "";
}

void historyManager::delMoniker(std::string s) {
	std::map<std::string, monikerData>::iterator i = monikers.find(s);
	if (i == monikers.end()) throw creaturesException("getMoniker was called with a non-existant moniker");
	monikers.erase(i);
}

/* vim: set noet: */
