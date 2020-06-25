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
#include "oldCreature.h"
#include "c2eCreature.h"
#include "CreatureAgent.h"
#include "World.h"
#include "Catalogue.h"
#include "c2eBrain.h"
#include "historyManager.h"
#include "oldBrain.h"
#include <cassert>
#include <memory>

Creature::Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a) {
	assert(g);
	genome = g;

	female = is_female;
	genus = 0; // TODO: really, we shouldn't do this, and should instead later assert that a genus was set
	variant = _variant;
	stage = baby;

	parent = a;
	assert(parent);
	parentagent = dynamic_cast<Agent *>(parent);
	assert(parentagent);
	
	alive = true; // ?
	asleep = false; // ?
	dreaming = false; // ?
	tickage = false;
	zombie = false;

	age = ticks = 0;

	attn = decn = -1;

	for (unsigned int i = 0; i < 5; i++)
		tintinfo[i] = 128;
}

Creature::~Creature() {
}

void Creature::finishInit() {
	processGenes();
}

bool Creature::shouldProcessGene(gene *g) {
	geneFlags &flags = g->header.flags;

	// non-expressed genes are to be ignored
	if (flags.notexpressed) return false;

	// gender-specific genes are only to be processed if they are of this 
	if (flags.femaleonly && !female) return false;
	if (flags.maleonly && female) return false;

	// obviously we only switch on at the stage in question
	if (g->header.switchontime != stage) return false;

	// TODO: header.variant?
	
	return true;
}

void Creature::processGenes() {
	for (auto i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (shouldProcessGene(i->get())) addGene(i->get());
	}
}

void oldCreature::processGenes() {
	brain->processGenes();
	Creature::processGenes();
}

void c2Creature::processGenes() {
	oldCreature::processGenes();

	for (std::vector<shared_ptr<c2Organ> >::iterator x = organs.begin(); x != organs.end(); x++) {
		(*x)->processGenes();
	}
}

void c2eCreature::processGenes() {
	// brain must be processed first (to create loci etc)
	// organs should be processed last, because new ones will be created by normal processGenes()

	brain->processGenes();
	Creature::processGenes();
	for (std::vector<shared_ptr<c2eOrgan> >::iterator x = organs.begin(); x != organs.end(); x++) {
		(*x)->processGenes();
	}
}

void Creature::addGene(gene *g) {
	if (typeid(*g) == typeid(creatureInstinctGene)) {
		unprocessedinstincts.push_back((creatureInstinctGene *)g);
	} else if (typeid(*g) == typeid(creatureGenusGene)) {
		// TODO: mmh, genus changes after setup shouldn't be valid
		genus = ((creatureGenusGene *)g)->genus;
		parentagent->genus = genus + 1;
	} else if (typeid(*g) == typeid(creaturePigmentGene)) {
		creaturePigmentGene &p = *((creaturePigmentGene *)g);
		// TODO: we don't sanity-check
		tintinfo[p.color] = p.amount;
	} else if (typeid(*g) == typeid(creaturePigmentBleedGene)) {
		creaturePigmentBleedGene &p = *((creaturePigmentBleedGene *)g);
		tintinfo[3] = p.rotation;
		tintinfo[4] = p.swap;
	}
}

void Creature::ageCreature() {
	if (stage >= senile) {
		die();
		return;
	} //previously we just returned

	stage = (lifestage)((int)stage + 1);
	processGenes();

	assert(parent);
	parent->creatureAged();
#ifndef _CREATURE_STANDALONE
	world.history->getMoniker(world.history->findMoniker(genome)).addEvent(4, "", ""); // aged event
#endif
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
	parent->creatureBorn();

	// TODO: life event?
#ifndef _CREATURE_STANDALONE
	world.history->getMoniker(world.history->findMoniker(genome)).wasBorn();
	world.history->getMoniker(world.history->findMoniker(genome)).addEvent(3, "", ""); // born event, parents..
#endif

	tickage = true;
}

void Creature::die() {
	parent->creatureDied();

	// TODO: life event?
#ifndef _CREATURE_STANDALONE
	world.history->getMoniker(world.history->findMoniker(genome)).hasDied();
	world.history->getMoniker(world.history->findMoniker(genome)).addEvent(7, "", ""); // died event
#endif
	// TODO: disable brain/biochemistry updates
	
	// force die script
	parentagent->stopScript();
	parentagent->queueScript(72); // Death script in c1, c2 and c2e
	
	// skeletalcreature eyes, also? see setAsleep comment
	alive = false;
}

void Creature::tick() {
	ticks++;

	if (!alive) return;

	if (tickage) age++;
}

/*
 * oldCreature contains the shared elements of C1 of C2 (creatures are mostly identical in both games)
 */
oldCreature::oldCreature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a) : Creature(g, is_female, _variant, a) {
	biochemticks = 0;
	halflives = 0;
	
	for (unsigned int i = 0; i < 8; i++) floatingloci[i] = 0;
	for (unsigned int i = 0; i < 7; i++) lifestageloci[i] = 0;
	for (unsigned int i = 0; i < 8; i++) involaction[i] = 0;
	for (unsigned int i = 0; i < 256; i++) chemicals[i] = 0;
	
	for (unsigned int i = 0; i < 8; i++) involactionlatency[i] = 0;
	
	muscleenergy = 0;
	fertile = pregnant = receptive = 0;
	dead = 0;

	brain = 0; // just in case
}

c1Creature::c1Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a) : oldCreature(g, is_female, _variant, a) {
	assert(g->getVersion() == 1);

	for (unsigned int i = 0; i < 6; i++) senses[i] = 0;
	for (unsigned int i = 0; i < 8; i++) gaitloci[i] = 0;
	for (unsigned int i = 0; i < 16; i++) drives[i] = 0;
	
	// TODO: chosenagents size

	brain = new oldBrain(this);
	finishInit();
	brain->init();
}

c2Creature::c2Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a) : oldCreature(g, is_female, _variant, a) {
	assert(g->getVersion() == 2);

	for (unsigned int i = 0; i < 14; i++) senses[i] = 0;
	for (unsigned int i = 0; i < 16; i++) gaitloci[i] = 0;
	for (unsigned int i = 0; i < 17; i++) drives[i] = 0;

	mutationchance = 0; mutationdegree = 0;

	// TODO: chosenagents size

	brain = new oldBrain(this);
	finishInit();
	brain->init();	
}

c2eCreature::c2eCreature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a) : Creature(g, is_female, _variant, a) {
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

	for (unsigned int i = 0; i < 8; i++) involactionlatency[i] = 0;

	halflives = 0;

	if (!catalogue.hasTag("Action Script To Neuron Mappings"))
		throw creaturesException("c2eCreature was unable to read the 'Action Script To Neuron Mappings' catalogue tag");
	const std::vector<std::string> &mappinginfotag = catalogue.getTag("Action Script To Neuron Mappings");
	for (std::vector<std::string>::const_iterator i = mappinginfotag.begin(); i != mappinginfotag.end(); i++)
		mappinginfo.push_back(atoi(i->c_str()));

	// TODO: should we really hard-code this?
	chosenagents.resize(40);

	brain = new c2eBrain(this);
	finishInit();
	brain->init();	
}

unsigned int c1Creature::getGait() {
	unsigned int gait = 0;

	for (unsigned int i = 1; i < 8; i++)
		if (gaitloci[i] > gaitloci[gait])
			gait = i;

	return gait;
}

unsigned int c2Creature::getGait() {
	unsigned int gait = 0;

	for (unsigned int i = 1; i < 16; i++)
		if (gaitloci[i] > gaitloci[gait])
			gait = i;

	return gait;
}

unsigned int c2eCreature::getGait() {
	unsigned int gait = 0;

	for (unsigned int i = 1; i < 16; i++)
		if (gaitloci[i] > gaitloci[gait])
			gait = i;

	return gait;
}

void c1Creature::tick() {
	// TODO: should we tick some things even if dead?
	if (!alive) return;

	senses[0] = 255; // always-on
	senses[1] = (asleep ? 255 : 0); // asleep
	senses[2] = 0; // air coldness (TODO)
	senses[3] = 0; // air hotness (TODO)
	senses[4] = 0; // light level (TODO)
	senses[5] = 0; // crowdedness (TODO)	

	tickBrain();
	tickBiochemistry();

	// lifestage checks
	for (unsigned int i = 0; i < 7; i++) {
		if ((lifestageloci[i] != 0) && (stage == (lifestage)i))
			ageCreature();
	}

	if (dead != 0) die();

	Creature::tick();
}

void c2Creature::tick() {
	// TODO: should we tick some things even if dead?
	if (!alive) return;

	senses[0] = 255; // always-on
	senses[1] = (asleep ? 255 : 0); // asleep
	senses[2] = 0; // air coldness (TODO)
	senses[3] = 0; // air hotness (TODO)
	senses[4] = 0; // light level (TODO)
	senses[5] = 0; // crowdedness (TODO)
	senses[6] = 0; // radiation (TODO)
	senses[7] = 0; // time of day (TODO)
	senses[8] = 0; // season (TODO)
	senses[9] = 255; // air quality (TODO)
	senses[10] = 0; // slope up (TODO)
	senses[11] = 0; // slope down (TODO)
	senses[12] = 0; // wind towards (TODO)
	senses[13] = 0; // wind behind (TODO)

	tickBrain();
	// TODO: update brain organ every 0.4ms (ie: when brain is processed)!
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

	// TODO: update muscleenergy

	senses[0] = 1.0f; // always-on
	senses[1] = (asleep ? 1.0f : 0.0f); // asleep
	// space for old C2 senses: hotness, coldness, light level
	senses[5] = 0.0f; // crowedness (TODO)
	// space for old C2 senses: radiation, time of day, season
	senses[9] = 1.0f; // air quality (TODO)
	senses[10] = 0.0f; // steepness of upcoming slope (up) (TODO)
	senses[11] = 0.0f; // steepness of upcoming slope (down) (TODO)
	// space for old C2 senses: oncoming wind, wind from behind

	tickBrain();
	tickBiochemistry();

	// lifestage checks
	for (unsigned int i = 0; i < 7; i++) {
		if ((lifestageloci[i] != 0.0f) && (stage == (lifestage)i))
			ageCreature();
	}

	if (dead != 0.0f) die();
	
	Creature::tick();
}

void oldCreature::addGene(gene *g) {
	Creature::addGene(g);
	if (typeid(*g) == typeid(bioInitialConcentrationGene)) {
		// initialise chemical levels
		bioInitialConcentrationGene *b = (bioInitialConcentrationGene *)(g);
		chemicals[b->chemical] = b->quantity;
	} else if (typeid(*g) == typeid(bioHalfLivesGene)) {
		bioHalfLivesGene *d = dynamic_cast<bioHalfLivesGene *>(g);
		assert(d);
		halflives = d;
	}	
}

void c1Creature::addGene(gene *g) {
	oldCreature::addGene(g);

	if (typeid(*g) == typeid(bioReactionGene)) {
		reactions.push_back(shared_ptr<c1Reaction>(new c1Reaction()));
		reactions.back()->init((bioReactionGene *)(g));
	} else if (typeid(*g) == typeid(bioEmitterGene)) {
		emitters.push_back(c1Emitter());
		emitters.back().init((bioEmitterGene *)(g), this);
	} else if (typeid(*g) == typeid(bioReceptorGene)) {
		receptors.push_back(c1Receptor());
		receptors.back().init((bioReceptorGene *)(g), this);
	}
}

void c2Creature::addGene(gene *g) {
	oldCreature::addGene(g);

	if (typeid(*g) == typeid(organGene)) {
		// create organ
		organGene *o = dynamic_cast<organGene *>(g);
		assert(o);
		if (!o->isBrain()) { // TODO: handle brain organ
			organs.push_back(shared_ptr<c2Organ>(new c2Organ(this, o)));
		}
	}
}

void c2eCreature::addGene(gene *g) {
	Creature::addGene(g);

	if (typeid(*g) == typeid(bioInitialConcentrationGene)) {
		// initialise chemical levels
		bioInitialConcentrationGene *b = (bioInitialConcentrationGene *)(g);
		chemicals[b->chemical] = b->quantity / 255.0f; // TODO: correctness unchecked
	} else if (typeid(*g) == typeid(organGene)) {
		// create organ
		organGene *o = dynamic_cast<organGene *>(g);
		assert(o);
		if (!o->isBrain()) { // TODO: handle brain organ
			organs.push_back(shared_ptr<c2eOrgan>(new c2eOrgan(this, o)));
		}
	} else if (typeid(*g) == typeid(bioHalfLivesGene)) {
		bioHalfLivesGene *d = dynamic_cast<bioHalfLivesGene *>(g);
		assert(d);
		halflives = d;
	}
}

void c2eCreature::adjustDrive(unsigned int id, float value) {
	assert(id < 20);
	drives[id] += value;

	if (drives[id] < 0.0f) drives[id] = 0.0f;
	else if (drives[id] > 1.0f) drives[id] = 1.0f;
}

/* vim: set noet: */
