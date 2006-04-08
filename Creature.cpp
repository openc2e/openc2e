/*
 *  Creature.cpp
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

#include "Creature.h"

Creature::Creature(shared_ptr<genomeFile> g, unsigned char _family, bool is_female, unsigned char _variant)
 : Agent(_family, 0, 0, 0) {
	caos_assert(g);
	genome = g;
	female = is_female;
	variant = _variant;
	species = (female ? 2 : 1);
	// TODO: set zorder randomly :) should be somewhere between 1000-2700, at a /guess/
	zorder = 1500;

	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (typeid(*(*i)) == typeid(bioInitialConcentration)) {
			bioInitialConcentration *b = (bioInitialConcentration *)(*i);
			chemicals[b->chemical] = b->quantity / 255.0f; // TODO: correctness unchecked
		} else if (typeid(*(*i)) == typeid(creatureGenus)) {
			creatureGenus *g = (creatureGenus *)(*i);
			genus = g->genus + 1;
		}
	}
	alive = true; // ?
	asleep = false; // ?
	dreaming = false; // ?
	tickage = false;
	zombie = false;
}

Creature::~Creature() {
	for (std::vector<Organ *>::iterator i = organs.begin(); i != organs.end(); i++) {
		delete *i;
	}
}

void Creature::ageCreature() {
	if (stage < senile) // TODO
		stage = (lifestage)((int)stage + 1);
}

void Creature::adjustChemical(unsigned char id, float value) {
	chemicals[id] += value;
	// TODO: clamp?
}

void Creature::setAsleep(bool a) {
	// TODO: skeletalcreature might need to close eyes? or should that just be done during the skeletal update?
	if (!a && dreaming)
		setDreaming(false);
	asleep = a;
}

void Creature::setDreaming(bool d) {
	if (d && !asleep)
		setAsleep(true);
	dreaming = d;
}

void Creature::born() {
	// TODO: life event?
	tickage = true;
}

void Creature::die() {
	// TODO: life event?
	// TODO: disable brain/biochemistry updates, trigger die script?
	// skeletalcreature eyes, also? see setAsleep comment
	alive = false;
}

void Creature::tick() {
	Agent::tick();
	if (paused) return;

	if (tickage) age++;
}

/*****************************************************************************/

Organ::Organ(Creature *p, organGene *g) {
	parent = p; assert(parent);
	ourGene = g; assert(ourGene);
	lifeforce = ourGene->lifeforce;
	shorttermlifeforce = lifeforce;

	repairrate = 0.0f; // TODO: ???
	clockrate = 1.0f; // TODO: ???
	injurytoapply = 0.0f;
}

float Organ::getEnergyCost() {
	// TODO: is genes.size() always the size we want?
	// TODO: is it really 0.000391258 or did I just fail at rounding?
	return ourGene->genes.size() * 0.000391258;
}

void Organ::tick() {
	if (lifeforce == 0.0f) return; // We're dead!
	
	tickInjury();
}

void Organ::tickInjury() {
	// Apply injury to short-term life force, if necessary
	if (injurytoapply != 0.0f) {
		shorttermlifeforce -= injurytoapply;

		if (shorttermlifeforce < 0.0f) shorttermlifeforce = 0.0f;
	}

	// Repair organ if possible
	shorttermlifeforce += repairrate;

	// Converge lifeforce upon short-term lifeforce, ie real damage
	if (shorttermlifeforce < lifeforce)
		lifeforce -= ourGene->damagerate; // TODO: probably nonsense

	// Make sure we didn't go too far.
	if (shorttermlifeforce > lifeforce)
		shorttermlifeforce = lifeforce;
}

/* vim: set noet: */
