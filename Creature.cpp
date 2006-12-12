/*
 *  Creature.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
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
#include "CreatureAgent.h"
#include "World.h"
#include <cmath> // powf

Creature::Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant) {
	parent = 0;
	assert(g);
	genome = g;
	
	female = is_female;
	variant = _variant;
	stage = baby;

	alive = true; // ?
	asleep = false; // ?
	dreaming = false; // ?
	tickage = false;
	zombie = false;

	ticks = 0;
}

Creature::~Creature() {
}

void Creature::processGenes() {
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if ((*i)->header.flags.femaleonly && !female) continue;
		if ((*i)->header.flags.maleonly && female) continue;
		addGene(*i);
	}
}

void Creature::setAgent(CreatureAgent *a) {
	parent = a;
	assert(parent);

	// TODO:
	// so, doing this here seems sort of horrible, but we can't do it during initial setup
	// because the only point is to set the genus of the parent agent..
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if ((*i)->header.flags.femaleonly && !female) continue;
		if ((*i)->header.flags.maleonly && female) continue;
			if (typeid(*(*i)) == typeid(creatureGenus)) {
			// initialize genus
			creatureGenus *g = (creatureGenus *)(*i);
			parent->genus = g->genus + 1;
		}
	}
}

void Creature::addGene(gene *g) {
}

void Creature::ageCreature() {
	if (stage < senile) // TODO
		stage = (lifestage)((int)stage + 1);

	assert(parent);
	parent->creatureAged();
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
	world.history.getMoniker(world.history.findMoniker(genome)).addEvent(7, "", ""); // died event
	// TODO: disable brain/biochemistry updates, trigger die script?
	// skeletalcreature eyes, also? see setAsleep comment
	alive = false;
}

void Creature::tick() {
	ticks++;

	if (!alive) return;

	if (tickage) age++;
}

c1Creature::c1Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant) : Creature(g, is_female, _variant) {
	assert(g->getVersion() == 1);

	for (unsigned int i = 0; i < 256; i++) chemicals[i] = 0;
	for (unsigned int i = 0; i < 8; i++) floatingloci[i] = 0;
	for (unsigned int i = 0; i < 7; i++) lifestageloci[i] = 0;
	muscleenergy = 0;
	fertile = pregnant = receptive = 0;
	dead = 0;
	for (unsigned int i = 0; i < 6; i++) senses[i] = 0;
	for (unsigned int i = 0; i < 8; i++) involaction[i] = 0;
	for (unsigned int i = 0; i < 8; i++) gaitloci[i] = 0;
	for (unsigned int i = 0; i < 16; i++) drives[i] = 0;
	
	biochemticks = 0;
	halflives = 0;

	processGenes();
}

c2eCreature::c2eCreature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant) : Creature(g, is_female, _variant) {
	assert(g->getVersion() == 3);

	for (unsigned int i = 0; i < 256; i++) chemicals[i] = 0.0f;

	// initialise loci
	for (unsigned int i = 0; i < 7; i++) lifestageloci[i] = 0.0f;
	muscleenergy = 0.0f;
	for (unsigned int i = 0; i < 32; i++) floatingloci[i] = 0.0f;
	fertile = pregnant = ovulate = receptive = chanceofmutation = degreeofmutation = 0.0f;
	dead = 0.0f;
	for (unsigned int i = 0; i < 8; i++) involaction[i] = 0.0f;
	for (unsigned int i = 0; i < 16; i++) gaitloci[i] = 0.0f;
	for (unsigned int i = 0; i < 14; i++) senses[i] = 0.0f;
	for (unsigned int i = 0; i < 20; i++) drives[i] = 0.0f;

	halflives = 0;

	processGenes();
}

void c1Creature::tick() {
	// TODO: should we tick some things even if dead?
	if (!alive) return;

	senses[0] = 255; // always-on
	senses[1] = (asleep ? 255 : 0); // asleep

	tickBiochemistry();

	// lifestage checks
	for (unsigned int i = 0; i < 7; i++) {
		if ((lifestageloci[i] != 0) && (stage == (lifestage)i))
			ageCreature();
	}

	if (dead != 0) die();

	Creature::tick();
}

void c2eCreature::tick() {
	// TODO: should we tick some things even if dead?
	if (!alive) return;

	senses[0] = 1.0f; // always-on
	senses[9] = 1.0f; // air quality (TODO)
	
	tickBiochemistry();

	// lifestage checks
	for (unsigned int i = 0; i < 7; i++) {
		if ((lifestageloci[i] != 0.0f) && (stage == (lifestage)i))
			ageCreature();
	}

	if (dead != 0.0f) die();
	
	Creature::tick();
}

void c1Creature::addGene(gene *g) {
	Creature::addGene(g);

	if (typeid(*g) == typeid(bioInitialConcentration)) {
		// initialise chemical levels
		bioInitialConcentration *b = (bioInitialConcentration *)(g);
		chemicals[b->chemical] = b->quantity;
	} else if (typeid(*g) == typeid(bioHalfLives)) {
		bioHalfLives *d = dynamic_cast<bioHalfLives *>(g);
		assert(d);
		halflives = d;
	} else if (typeid(*g) == typeid(bioReaction)) {
		reactions.push_back(shared_ptr<c1Reaction>(new c1Reaction()));
		reactions.back()->init((bioReaction *)(g));
	} else if (typeid(*g) == typeid(bioEmitter)) {
		emitters.push_back(c1Emitter());
		emitters.back().init((bioEmitter *)(g), this);
	} else if (typeid(*g) == typeid(bioReceptor)) {
		receptors.push_back(c1Receptor());
		receptors.back().init((bioReceptor *)(g), this);
	}
}

void c2eCreature::addGene(gene *g) {
	Creature::addGene(g);

	if (typeid(*g) == typeid(bioInitialConcentration)) {
		// initialise chemical levels
		bioInitialConcentration *b = (bioInitialConcentration *)(g);
		chemicals[b->chemical] = b->quantity / 255.0f; // TODO: correctness unchecked
	} else if (typeid(*g) == typeid(organGene)) {
		// create organ
		organGene *o = dynamic_cast<organGene *>(g);
		assert(o);
		if (!o->isBrain()) { // TODO: handle brain organ
			organs.push_back(shared_ptr<c2eOrgan>(new c2eOrgan(this, o)));
		}
	} else if (typeid(*g) == typeid(bioHalfLives)) {
		bioHalfLives *d = dynamic_cast<bioHalfLives *>(g);
		assert(d);
		halflives = d;
	}
}

void c1Creature::addChemical(unsigned char id, unsigned char val) {
	if (id == 0) return;

	// clipping..
	if ((int)chemicals[id] + val > 255) chemicals[id] = 255;
	else chemicals[id] += val;
}

void c1Creature::subChemical(unsigned char id, unsigned char val) {
	if (id == 0) return;

	// clipping..
	if ((int)chemicals[id] - val < 0) chemicals[id] = 0;
	else chemicals[id] -= val;
}

void c2eCreature::adjustChemical(unsigned char id, float value) {
	if (id == 0) return;
	
	chemicals[id] += value;

	if (chemicals[id] < 0.0f) chemicals[id] = 0.0f;
	else if (chemicals[id] > 1.0f) chemicals[id] = 1.0f;
}

void c2eCreature::adjustDrive(unsigned int id, float value) {
	assert(id < 20);
	drives[id] += value;

	if (drives[id] < 0.0f) drives[id] = 0.0f;
	else if (drives[id] > 1.0f) drives[id] = 1.0f;
}

// lookup table, snaffled from real creatures
// TODO: work out if these are meaningful values :)
unsigned int c1rates[32] = {
	0, 0x32A5, 0x71DD, 0xAABB, 0xD110, 0xE758, 0xF35C,
	0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999,
	0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999,
	0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999,
	0xFFFF
};

inline unsigned int c1Creature::calculateMultiplier(unsigned char rate) {
	return c1rates[rate];
}

inline unsigned int c1Creature::calculateTickMask(unsigned char rate) {
	if (rate < 7) return 0;
	else return (1 << ((unsigned int)rate - 7)) - 1;
}

void c1Creature::tickBiochemistry() {
	// TODO: untested

	if ((ticks % 5) != 0) return;
	biochemticks++;

	// process emitters
	for (std::vector<c1Emitter>::iterator i = emitters.begin(); i != emitters.end(); i++) {
		processEmitter(*i);
	}

	// process receptors
	for (std::vector<c1Receptor>::iterator i = receptors.begin(); i != receptors.end(); i++) {
		processReceptor(*i);
	}

	// process reactions
	for (std::vector<shared_ptr<c1Reaction> >::iterator i = reactions.begin(); i != reactions.end(); i++) {
		processReaction(**i);
	}

	// process half-lives
	if (!halflives) return; // TODO: correct?
	for (unsigned int i = 0; i < 256; i++) {
		// TODO: this code hasn't been tested thoroughly, but seems to agree with basic testing

		// work out which rate we're dealing with
		unsigned char rate = halflives->halflives[i] / 8;
	
		// if the tickmask doesn't want us to change things this tick, don't!
		if ((biochemticks & calculateTickMask(rate)) != 0) continue;

		// do the actual adjustment
		chemicals[i] = (chemicals[i] * calculateMultiplier(rate)) / 65536;
	}
}

void c2eCreature::tickBiochemistry() {
	// only process biochem every 4 ticks
	// TODO: correct? should probably apply to brain too, at least
	if ((ticks % 4) != 0) return;
	
	// tick organs
	for (std::vector<shared_ptr<c2eOrgan> >::iterator x = organs.begin(); x != organs.end(); x++) {
		(*x)->tick();
	}

	// process half-lives for chemicals
	if (!halflives) return; // TODO: correct?
	for (unsigned int x = 0; x < 256; x++) {
		if (halflives->halflives[x] == 0) {
			// 0 is a special case for half-lives
			chemicals[x] = 0.0f;
		} else {
			// reaction rate = 1.0 - 0.5**(1.0 / 2.2**(rate * 32.0 / 255.0))
			float rate = 1.0 - powf(0.5, 1.0 / powf(2.2, (halflives->halflives[x] * 32.0) / 255.0));

			chemicals[x] -= chemicals[x] * rate;
		}
	}
}

unsigned char *c1Creature::getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l) {
	switch (o) {
		case 0: // brain
			return 0; // TODO

		case 1: // creature
			switch (t) {
				case 0: // somantic
					if (receptor) {
						if (l > 7) break;
						return &lifestageloci[l];
					} else if (l == 0) return &muscleenergy;
					break;

				case 1: // circulatory
					if (l > 8) break;
					return &floatingloci[l];
				
				case 2: // reproductive
					if (receptor) {
						if (l == 0) return &fertile;
						else if (l == 1) return &receptive;
					} else {
						if (l == 0) return &fertile;
						else if (l == 1) return &pregnant;
					}
					break;

				case 3: // immune
					if (l == 0) return &dead;
					break;

				case 4: // sensorimotor
					if (receptor) {
						if (l < 8) return &involaction[l];
						else if (l < 16) return &gaitloci[l - 8];
					} else {
						if (l < 6) return &senses[l];
					}

				case 5: // drive levels
					if (l < 16) return &drives[l];
			}
	}

	std::cout << "c1Creature::getLocusPointer failed to interpret locus (" << (int)o << ", "
		<< (int)t << ", " << (int)l << ") of " << (receptor ? "receptor" : "emitter")
		<< std::endl;

	return 0;
}

float *c2eCreature::getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l) {
	switch (o) {
		case 0: // brain
			{
			// t = lobe tissue id
			unsigned int neuronid = o/3, stateno = o%3;
			return 0; // TODO
			}

		case 1: // creature
			switch (t) {
				case 0: // somatic
					if (receptor) {
						if (l > 6) break;
						return &lifestageloci[l];
					} else if (l == 0) return &muscleenergy;
					break;

				case 1: // circulatory
					if (l > 31) break;
					return &floatingloci[l];

				case 2: // reproductive
					if (!receptor) {
						if (l == 0) return &fertile;
						else if (l == 1) return &pregnant;
						l = l - 2; // TODO: this throws off error msg at end of function
					}
					switch (l) {
						case 0: return &ovulate;
						case 1: return &receptive;
						case 2: return &chanceofmutation;
						case 3: return &degreeofmutation;
					}
					break;
					
				case 3: // immune
					if (l == 0) return &dead;
					break;
				
				case 4: // sensorimotor
					if (!receptor) {
						if (l < 14) return &senses[l];
						l -= 14;
					}
					if (l < 8) return &involaction[l];
					l -= 8;
					if (l < 16) return &gaitloci[l];
					break;

				case 5: // drives
					if (l < 20) return &drives[l];
					break;
			}

	}

	std::cout << "c2eCreature::getLocusPointer failed to interpret locus (" << (int)o << ", "
		<< (int)t << ", " << (int)l << ") of " << (receptor ? "receptor" : "emitter")
		<< std::endl;
	return 0;
}
 
/*****************************************************************************/

c2eOrgan::c2eOrgan(c2eCreature *p, organGene *g) {
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
	
	shared_ptr<c2eReaction> r; // we need to store the previous reaction for possible receptor use
	for (vector<gene *>::iterator i = ourGene->genes.begin(); i != ourGene->genes.end(); i++) {
		if ((*i)->header.flags.femaleonly && !p->isFemale()) continue;
		if ((*i)->header.flags.maleonly && p->isFemale()) continue;
		if (typeid(*(*i)) == typeid(bioReaction)) {
			reactions.push_back(shared_ptr<c2eReaction>(new c2eReaction()));
			r = reactions.back();
			reactions.back()->init((bioReaction *)(*i));
		} else if (typeid(*(*i)) == typeid(bioEmitter)) {
			emitters.push_back(c2eEmitter());
			emitters.back().init((bioEmitter *)(*i), this);
		} else if (typeid(*(*i)) == typeid(bioReceptor)) {
			receptors.push_back(c2eReceptor());
			receptors.back().init((bioReceptor *)(*i), this, r);
		}
	}
}

void c2eOrgan::tick() {
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
			for (vector<c2eEmitter>::iterator i = emitters.begin(); i != emitters.end(); i++)
				processEmitter(*i);
			
			// *** tick reactions
			for (vector<shared_ptr<c2eReaction> >::iterator i = reactions.begin(); i != reactions.end(); i++)
				processReaction(**i);
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
	for (vector<shared_ptr<c2eReaction> >::iterator i = reactions.begin(); i != reactions.end(); i++) (*i)->receptors = 0;
	clockratereceptors = 0; repairratereceptors = 0; injuryreceptors = 0;
		
	for (vector<c2eReceptor>::iterator i = receptors.begin(); i != receptors.end(); i++)
		processReceptor(*i, ticked);
	
	for (vector<shared_ptr<c2eReaction> >::iterator i = reactions.begin(); i != reactions.end(); i++) if ((*i)->receptors > 0) (*i)->rate /= (*i)->receptors;
	if (clockratereceptors > 0) clockrate /= clockratereceptors;
	if (repairratereceptors > 0) repairrate /= repairratereceptors;
	if (injuryreceptors > 0) injurytoapply /= injuryreceptors;
	
	// *** decay life force
	shorttermlifeforce -= shorttermlifeforce * (1.0f / 1000000.0f);
	longtermlifeforce -= longtermlifeforce * (1.0f / 1000000.0f);
}

void c2eOrgan::applyInjury(float value) {
	shorttermlifeforce -= value;
	if (shorttermlifeforce < 0.0f)
		shorttermlifeforce = 0.0f;
	// adjust Injury chemical (TODO: de-hardcode)
	parent->adjustChemical(127, value / lifeforce);
}

void c1Creature::processReaction(c1Reaction &d) {
	// TODO: untested

	bioReaction &g = *d.data;

	// TODO: this might not all be correct

	// work out which rate we're dealing with
	unsigned char rate = g.rate / 8;
	
	// if the tickmask doesn't want us to change things this tick, don't!
	if ((biochemticks & calculateTickMask(rate)) != 0) return;

	unsigned char ratio = 255, ratio2 = 255;
	if (g.reactant[0] != 0) {
		assert(g.quantity[0] != 0); // TODO
		ratio = getChemical(g.reactant[0]) / g.quantity[0];
	}
	if (g.reactant[1] != 0) {
		assert(g.quantity[1] != 0); // TODO
		ratio2 = getChemical(g.reactant[1]) / g.quantity[1];
	}

	// pick lowest ratio, if zero then return
	if (ratio2 < ratio) ratio = ratio2;
	if (ratio == 0) return;
	
	// calculate the actual adjustment
	ratio = (ratio * calculateMultiplier(rate)) / 65536;

	// change chemical levels
	subChemical(g.reactant[0], ratio * g.quantity[0]);
	subChemical(g.reactant[1], ratio * g.quantity[1]);
	addChemical(g.reactant[2], ratio * g.quantity[2]);
	addChemical(g.reactant[3], ratio * g.quantity[3]);
}

void c2eOrgan::processReaction(c2eReaction &d) {
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

	// pick lowest ratio, if zero then return
	if (ratio2 < ratio) ratio = ratio2;
	if (ratio == 0.0f) return;

	// calculate the actual adjustment
	float rate = 1.0 - powf(0.5, 1.0 / powf(2.2, (1.0 - d.rate) * 32.0));
	ratio = ratio * rate;

	// change chemical levels
	parent->adjustChemical(g.reactant[0], -(ratio * (float)g.quantity[0]));
	parent->adjustChemical(g.reactant[1], -(ratio * (float)g.quantity[1]));
	parent->adjustChemical(g.reactant[2], ratio * (float)g.quantity[2]);
	parent->adjustChemical(g.reactant[3], ratio * (float)g.quantity[3]);
}

void c1Creature::processEmitter(c1Emitter &d) {
	// TODO: untested

	bioEmitter &g = *d.data;

	if ((biochemticks % g.rate) != 0) return;

	if (!d.locus) return;
	unsigned char f = *d.locus;
	if (g.clear) *d.locus = 0;
	if (g.invert) f = 255 - f;

	if (g.digital) {
		if (f < g.threshold) return;
		addChemical(g.chemical, g.gain);
	} else {
		int r = (((int)f - g.threshold) * g.gain) / 255;
		
		// clip the result of the calculation to unsigned char, and reassign it
		if (r < 0) r = 0; else if (r > 255) r = 255;
		f = r;

		addChemical(g.chemical, f);
	}
}

void c2eOrgan::processEmitter(c2eEmitter &d) {
	bioEmitter &g = *d.data;
	
	if (d.sampletick != g.rate) {
		assert(d.sampletick < g.rate);
		d.sampletick++;
		return;
	}
	
	d.sampletick = 0;

	if (!d.locus) return;
	float f = *d.locus;
	if (g.clear) *d.locus = 0.0f;
	if (g.invert) f = 1.0f - f;

	if (g.digital) {
		if (f < d.threshold) return;
		parent->adjustChemical(g.chemical, d.gain);
	} else {
		f = (f - d.threshold) * d.gain;
		if (f > 0.0f) // TODO: correct check?
			parent->adjustChemical(g.chemical, f);
	}
}

void c1Creature::processReceptor(c1Receptor &d) {
	// TODO: untested

	bioReceptor &g = *d.data;

	// TODO: same issues as c2eOrgan::processReceptor below, probably

	if (!d.locus) return;

	unsigned char f = chemicals[g.chemical];
	int r;
	if (g.digital)
		r = f > g.threshold ? g.gain : 0;
	else
		// TODO: int promotion correct to makke this work out?
		r = (((int)f - g.threshold) * g.gain) / 255;
	
	if (g.inverted) r = g.nominal - r;
	else r += g.nominal;
	
	// clip the result of the calculation to unsigned char, and reassign it
	if (r < 0) r = 0; else if (r > 255) r = 255;
	f = r;

	if (f == 0 && g.organ == 1 && g.tissue == 3 && g.locus == 0) // evil check for "Die if non-zero!" locus
		return;
	else
		*d.locus = f;
}

void c2eOrgan::processReceptor(c2eReceptor &d, bool checkchem) {
	bioReceptor &g = *d.data;
	
	/*
	 * TODO: This code has issues..
	 *
	 * eg, if you have two receptors pointing at a non-local locus,
	 * we just stomp over it in order, so the last receptor always get it
	 * while c2e seems to alternate between them (possibly organ clockrate stuff)
	 *
	 */
	
	if (checkchem) {
		d.processed = true;
		d.lastvalue = parent->getChemical(g.chemical);
	}

	if (!d.processed) return;
	if (!d.locus) return;

	float f;
	if (g.digital)
		f = d.lastvalue > d.threshold ? d.gain : 0.0f;
	else
		f = (d.lastvalue - d.threshold) * d.gain;
	if (g.inverted) f *= -1.0f;
	f += d.nominal;

	if (f < 0.0f) f = 0.0f; else if (f > 1.0f) f = 1.0f; // TODO: correct?

	if (f == 0.0f && g.organ == 1 && g.tissue == 3 && g.locus == 0) // evil check for "Die if non-zero!" locus
		return;
	else if (d.receptors) {
		if (*d.receptors == 0) *d.locus = 0.0f;
		(*d.receptors)++;
		*d.locus += f;
	} else
		*d.locus = f;
}

float *c2eOrgan::getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l, unsigned int**receptors) {
	if (receptors) *receptors = 0;

	switch (o) {
		case 2: // organ
			if (t == 0)
				switch (l) {
					case 0: // clock rate
						if (receptors) *receptors = &clockratereceptors;
						return &clockrate;
					case 1: // repair rate
						if (receptors) *receptors = &repairratereceptors;
						return &repairrate;
					case 2: // injury to apply
						if (receptors) *receptors = &injuryreceptors;
						return &injurytoapply;
						return 0;
				}
			break;
		case 3: // reaction
			if (t == 0 && l == 0) { // reaction rate
				shared_ptr<c2eReaction> r = reactions.back();
				if (!r) {
					std::cout << "Organ::getLocusPointer failed to find a reaction" << std::endl;
					return 0;
				} else {
					if (receptors) *receptors = &r->receptors;
					return &r->rate;
				}
			}
	}

	return parent->getLocusPointer(receptor, o, t, l);
}

void c1Reaction::init(bioReaction *g) {
	data = g;
}

void c2eReaction::init(bioReaction *g) {
	data = g;

	// rate is stored in genome as 0 fastest, 255 slowest
	// reversed in game, i think .. TODO: check this
	rate = 1.0 - (g->rate / 255.0);
}

void c1Receptor::init(bioReceptor *g, c1Creature *parent) {
	data = g;
	locus = parent->getLocusPointer(true, g->organ, g->tissue, g->locus);
}
	
void c2eReceptor::init(bioReceptor *g, c2eOrgan *parent, shared_ptr<c2eReaction> r) {
	data = g;
	processed = false;
	nominal = g->nominal / 255.0f;
	threshold = g->threshold / 255.0f;
	gain = g->gain / 255.0f;
	locus = parent->getLocusPointer(true, g->organ, g->tissue, g->locus, &receptors);
}

void c1Emitter::init(bioEmitter *g, c1Creature *parent) {
	data = g;
	locus = parent->getLocusPointer(false, g->organ, g->tissue, g->locus);
}

void c2eEmitter::init(bioEmitter *g, c2eOrgan *parent) {
	data = g;
	sampletick = 0;
	threshold = g->threshold / 255.0f;
	gain = g->gain / 255.0f;
	locus = parent->getLocusPointer(false, g->organ, g->tissue, g->locus, 0);
}

/* vim: set noet: */
