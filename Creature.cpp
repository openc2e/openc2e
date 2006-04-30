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

	biochemticks = 0;
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
	if (id == 0) return;
	
	chemicals[id] += value;

	if (chemicals[id] < 0.0f) chemicals[id] = 0.0f;
	else if (chemicals[id] > 1.0f) chemicals[id] = 1.0f;
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
	// only process biochem every 4 ticks
	// TODO: correct? should probably apply to brain too, at least
	biochemticks++;
	if (biochemticks == 4)
		biochemticks = 0;
	else
		return;
	
	// tick organs
	for (std::vector<Organ *>::iterator x = organs.begin(); x != organs.end(); x++) {
		(*x)->tick();
	}

	// process half-lives for chemicals
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (typeid(*(*i)) == typeid(bioHalfLives)) {
			bioHalfLives *d = dynamic_cast<bioHalfLives *>(*i);
			assert(d);

			for (unsigned int x = 0; x < 256; x++) {
				if (d->halflives[x] == 0) {
					// 0 is a special case for half-lives
					chemicals[x] = 0.0f;
				} else {
					// reaction rate = 1.0 - 0.5**(1.0 / 2.2**(rate * 32.0 / 255.0))
					float rate = 1.0 - powf(0.5, 1.0 / powf(2.2, (d->halflives[x] * 32.0) / 255.0));

					chemicals[x] -= chemicals[x] * rate;
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
	longtermlifeforce = shorttermlifeforce = lifeforce;

	repairrate = 0.0f;
	clockrate = ourGene->clockrate / 255.0f;
	injurytoapply = 0.0f;
	damagerate = ourGene->damagerate / 255.0f;
	biotick = ourGene->biotickstart / 255.0f;
	atpdamagecoefficient = ourGene->atpdamagecoefficient * (lifeforce / (255.0f * 255.0f));

	// TODO: is genes.size() always the size we want?
	energycost = (1.0f / 128.0f) + ourGene->genes.size() * (0.1f / 255.0f);
	
	Reaction *r = 0; // we need to store the previous reaction for possible receptor use
	for (vector<gene *>::iterator i = ourGene->genes.begin(); i != ourGene->genes.end(); i++) {
		if (typeid(*(*i)) == typeid(bioReaction)) {
			reactions.push_back(Reaction());
			r = &reactions.back();
			reactions.back().init((bioReaction *)(*i));
		} else if (typeid(*(*i)) == typeid(bioEmitter)) {
			emitters.push_back(Emitter());
			emitters.back().init((bioEmitter *)(*i));
		} else if (typeid(*(*i)) == typeid(bioEmitter)) {
			receptors.push_back(Receptor());
			receptors.back().init((bioReceptor *)(*i), r);
		}
	}
}

void Organ::tick() {
	if (longtermlifeforce <= 0.5f) return; // We're dead!

	biotick += clockrate;

	bool ticked = false;
	
	// if it's our turn to tick..
	if (biotick >= 1.0f) {
		ticked = true;
		// .. push the biotick back down
		biotick -= 1.0f;

		// *** energy consumption
		// chem 35 = ATP, chem 36 = ADP (TODO: fix hardcoding)
		float atplevel = parent->getChemical(35);
		bool hadenergy = false;
		if (atplevel >= energycost) {
			hadenergy = true;
			parent->adjustChemical(35, -energycost);
			parent->adjustChemical(36, energycost);
			
			// *** tick emitters
			for (vector<Emitter>::iterator i = emitters.begin(); i != emitters.end(); i++)
				processEmitter(*i);
			
			// *** tick reactions
			for (vector<Reaction>::iterator i = reactions.begin(); i != reactions.end(); i++)
				processReaction(*i);
		} else {
			// *** out of energy damage	
			applyInjury(atpdamagecoefficient);
		}

		// *** long-term damage
		float diff = longtermlifeforce - shorttermlifeforce;
		longtermlifeforce = longtermlifeforce - (diff * damagerate); // damagerate always <= 1.0

		// *** repair injuries
		float repair = diff * repairrate; // repairrate always <= 1.00
		shorttermlifeforce += repair;
		// adjust Injury chemical (TODO: de-hardcode)
		parent->adjustChemical(127, -repair / lifeforce);

		if (injurytoapply)
			applyInjury(injurytoapply);
	}
	
	// *** tick receptors
	for (vector<Receptor>::iterator i = receptors.begin(); i != receptors.end(); i++)
		processReceptor(*i, ticked);
	
	// *** decay life force
	shorttermlifeforce -= shorttermlifeforce * (1.0f / 1000000.0f);
	longtermlifeforce -= longtermlifeforce * (1.0f / 1000000.0f);
}

void Organ::applyInjury(float value) {
	shorttermlifeforce -= value;
	if (shorttermlifeforce < 0.0f)
		shorttermlifeforce = 0.0f;
	// adjust Injury chemical (TODO: de-hardcode)
	parent->adjustChemical(127, value / lifeforce);
}

void Organ::processReaction(Reaction &d) {
	bioReaction &g = *d.data;
	
	// TODO: this might not all be correct

	float ratio = 1.0f, ratio2 = 1.0f;
	if (g.reactant[0] != 0) {
		assert(g.quantity[0] != 0); // TODO
		ratio = parent->getChemical(g.reactant[0]) / (float)g.quantity[0];
	}
	if (g.reactant[1] != 0) {
		assert(g.quantity[1] != 0); // TODO
		ratio2 = parent->getChemical(g.reactant[1]) / (float)g.quantity[1];
	}
	if (ratio2 < ratio)
		ratio = ratio2;

	if (ratio == 0.0f) return;

	float rate = 1.0 - powf(0.5, 1.0 / powf(2.2, (1.0 - d.rate) * 32.0));

	ratio = ratio * rate;

	parent->adjustChemical(g.reactant[0], -(ratio * (float)g.quantity[0]));
	parent->adjustChemical(g.reactant[1], -(ratio * (float)g.quantity[1]));
	parent->adjustChemical(g.reactant[2], ratio * (float)g.quantity[2]);
	parent->adjustChemical(g.reactant[3], ratio * (float)g.quantity[3]);
}

void Organ::processEmitter(Emitter &d) {
	// TODO
}

void Organ::processReceptor(Receptor &d, bool checkchem) {
	if (checkchem) {
		// TODO

		d.processed = true;
		d.lastvalue = 0.0f; // TODO: chem result
	}

	if (!d.processed) return;
	
	// TODO
}

void Reaction::init(bioReaction *g) {
	data = g;

	// rate is stored in genome as 0 fastest, 255 slowest
	// reversed in game, i think .. TODO: check this
	rate = 1.0 - (g->rate / 255.0);
}

void Receptor::init(bioReceptor *g, Reaction *r) {
	data = g;
	processed = false;
	lastReaction = r;
}

void Emitter::init(bioEmitter *g) {
	data = g;
}

/* vim: set noet: */
