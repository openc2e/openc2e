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
#include "World.h"
#include <cmath> // powf

Creature::Creature(shared_ptr<genomeFile> g, unsigned char _family, bool is_female, unsigned char _variant)
 : Agent(_family, 0, 0, 0) {
	caos_assert(g);
	genome = g;
	slots[0] = g;
	female = is_female;
	variant = _variant;
	species = (female ? 2 : 1);
	// TODO: set zorder randomly :) should be somewhere between 1000-2700, at a /guess/
	zorder = 1500;

	for (unsigned int i = 0; i < 256; i++)
		halflife_timers[i] = 0.0f;

	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (typeid(*(*i)) == typeid(bioInitialConcentration)) {
			// initialise chemical levels
			bioInitialConcentration *b = (bioInitialConcentration *)(*i);
			chemicals[b->chemical] = b->quantity / 255.0f; // TODO: correctness unchecked
		} else if (typeid(*(*i)) == typeid(creatureGenus)) {
			// initialize genus
			creatureGenus *g = (creatureGenus *)(*i);
			genus = g->genus + 1;
		} else if (typeid(*(*i)) == typeid(organGene)) {
			// create organ
			organGene *o = dynamic_cast<organGene *>(*i);
			assert(o);
			if (!o->isBrain()) { // TODO: handle brain organ
				Organ *x = new Organ(this, o);
				organs.push_back(x);
			}
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
	world.history.getMoniker(world.history.findMoniker(genome)).wasBorn();
	world.history.getMoniker(world.history.findMoniker(genome)).addEvent(3, "", ""); // born event, parents..

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
	if ((paused) || (!alive)) return;

	if (tickage) age++;

	tickBiochemistry();
}

void Creature::tickBiochemistry() {
	// tick organs
	for (std::vector<Organ *>::iterator x = organs.begin(); x != organs.end(); x++) {
		(*x)->tick();
	}

	// process half-lives for chemicals
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (typeid(*(*i)) == typeid(bioHalfLives)) {
			bioHalfLives *d = dynamic_cast<bioHalfLives *>(*i);
			assert(d);

			// TODO: this seems like an absolutely absurd way to do this, so it's probably
			// wrong! research and fix it!
			for (unsigned int x = 0; x < 256; x++) {
				halflife_timers[x] += 1; // increase timer by one tick
				
				// Here, we use a magical constant, which depends on the C3 genome kit
				// being right about timescales (we assume tick == 50ms, like c2e).
				// TODO: lookup table better than this?
				float halflife = powf(1.104005, d->halflives[x]);

				// if we've exceeded the timer, reset and halve chemical level
				if (halflife_timers[x] >= halflife) {
					halflife_timers[x] = 0;
					chemicals[x] = chemicals[x] / 2.0f;
				}
			}
		}
	}
}

/*****************************************************************************/

Organ::Organ(Creature *p, organGene *g) {
	parent = p; assert(parent);
	ourGene = g; assert(ourGene);
	lifeforce = ourGene->lifeforce * (1000000.0f / 255.0f);
	shorttermlifeforce = lifeforce;

	repairrate = 0.0f; // TODO: ???
	clockrate = 1.0f; // TODO: ???
	injurytoapply = 0.0f;

	// TODO: is genes.size() always the size we want?
	// TODO: is it really 0.000391258 or did I just fail at rounding?
	energycost = ourGene->genes.size() * 0.000391258;
}

void Organ::tick() {
	if (lifeforce == 0.0f) return; // We're dead!
	
	tickInjury();
}

void Organ::tickInjury() {
	// Repair organ if possible
	shorttermlifeforce += repairrate;

	// Apply injury to short-term life force, if necessary
	if (injurytoapply != 0.0f) {
		shorttermlifeforce -= injurytoapply;

		if (shorttermlifeforce < 0.0f) shorttermlifeforce = 0.0f;
	}

	// Converge lifeforce upon short-term lifeforce, ie real damage
	if (shorttermlifeforce < lifeforce)
		lifeforce -= (ourGene->damagerate * (1.0f / 255.0f)); // TODO: probably nonsense

	// Make sure we didn't go too far.
	if (shorttermlifeforce > lifeforce)
		shorttermlifeforce = lifeforce;
}

/* vim: set noet: */
