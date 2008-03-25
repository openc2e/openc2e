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
#include "Catalogue.h"
#include <cmath> // powf
#include "c2eBrain.h"
#include "oldBrain.h"

Creature::Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a) {
	assert(g);
	genome = g;

	female = is_female;
	genus = 0; // TODO: really, we shouldn't do this, and should instead later assert that a genus was set
	variant = _variant;
	stage = baby;

	assert(a);
	parent = a;
	
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
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (shouldProcessGene(*i)) addGene(*i);
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
		parent->genus = genus + 1;
	} else if (typeid(*g) == typeid(creaturePigmentGene)) {
		creaturePigmentGene &p = *((creaturePigmentGene *)g);
		// TODO: we don't sanity-check
		tintinfo[p.color] = p.amount;
	} else if (typeid(*g) == typeid(creaturePigmentGene)) {
		creaturePigmentBleedGene &p = *((creaturePigmentBleedGene *)g);
		tintinfo[3] = p.rotation;
		tintinfo[4] = p.swap;
	}
}

void Creature::ageCreature() {
	if (stage >= senile) return; // TODO
	
	stage = (lifestage)((int)stage + 1);
	processGenes();

	assert(parent);
	parent->creatureAged();
#ifndef _CREATURE_STANDALONE	
	world.history.getMoniker(world.history.findMoniker(genome)).addEvent(4, "", ""); // aged event
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
	world.history.getMoniker(world.history.findMoniker(genome)).wasBorn();
	world.history.getMoniker(world.history.findMoniker(genome)).addEvent(3, "", ""); // born event, parents..
#endif

	tickage = true;
}

void Creature::die() {
	parent->creatureDied();

	// TODO: life event?
#ifndef _CREATURE_STANDALONE
	world.history.getMoniker(world.history.findMoniker(genome)).hasDied();
	world.history.getMoniker(world.history.findMoniker(genome)).addEvent(7, "", ""); // died event
#endif
	// TODO: disable brain/biochemistry updates
	// force die script
	// TODO: TODO: TODO: this is c2e-specific
	parent->stopScript();
	parent->queueScript(72);
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

void oldCreature::tickBrain() {
	// TODO

	brain->tick();
	
	// TODO
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

void c2eCreature::tickBrain() {
	if (asleep) {
		attn = -1;
		decn = -1;
		attention.clear(); // TODO: doesn't belong here
		if (!dreaming) return; // TODO
	}

	// TODO: correct timing?
	if ((ticks % 4) != 0)
		return;

	if (dreaming) {
		// TODO: this returns a bool (whether it did an instinct or not), shouldn't we do non-instinct dreaming or something if it's false?
		// .. if not, make it a void ;p
		processInstinct();
		return;
	}

	c2eLobe *drivlobe = brain->getLobeById("driv");
	if (drivlobe) {
		for (unsigned int i = 0; i < 20 && i < drivlobe->getNoNeurons(); i++) {
			drivlobe->setNeuronInput(i, drives[i]);
		}
	}

	/*c2eLobe *verblobe = brain->getLobeById("verb");
	if (verblobe) {
		for (unsigned int i = 0; i < verblobe->getNoNeurons(); i++) {
			verblobe->setNeuronInput(i, 0.0f); // TODO
		}
	}
	
	c2eLobe *nounlobe = brain->getLobeById("noun");
	if (nounlobe) {
		for (unsigned int i = 0; i < nounlobe->getNoNeurons(); i++) {
			nounlobe->setNeuronInput(i, 0.0f); // TODO
		}	
	}*/

#ifndef _CREATURE_STANDALONE	
	// TODO: situ, detl
	
	chooseAgents();
	
	c2eLobe *visnlobe = brain->getLobeById("visn");
	if (visnlobe) {
		for (unsigned int i = 0; i < visnlobe->getNoNeurons() && i < chosenagents.size(); i++) {
			AgentRef a = chosenagents[i];
			if (!a) continue;

			// TODO: use eye position? see Creature::agentInSight
			float ourxpos = parent->x + (parent->getWidth() / 2.0f);
			float theirxpos = a->x + (a->getWidth() / 2.0f);
			float distance = theirxpos - ourxpos;

			// TODO: squash result into appropriate range?
			visnlobe->setNeuronInput(i, distance / parent->range.getFloat());
		}
	}

	c2eLobe *smellobe = brain->getLobeById("smel");
	if (smellobe) {
		// TODO
	}
#endif

	brain->tick();
	
#ifndef _CREATURE_STANDALONE	
	AgentRef oldattn = attention;
	int olddecn = decn;

	c2eLobe *attnlobe = brain->getLobeById("attn");
	if (attnlobe) {
		attn = attnlobe->getSpareNeuron();
	}

	c2eLobe *decnlobe = brain->getLobeById("decn");
	if (decnlobe) {
		// TODO: check bounds of mappinginfo
		decn = mappinginfo[decnlobe->getSpareNeuron()];
	}

	// TODO: doesn't belong here
	if (attn >= 0 && attn < (int)chosenagents.size())
		attention = chosenagents[attn];

	if (zombie) return; // TODO: docs say zombies "don't process decision scripts", correct?

	// fire scripts as needed
	// TODO: doesn't belong here
	// TODO: deal with decisions which don't have agents attached
	// TODO: deal with moving between ATTNs which don't have a choseagent right now (eg, nothing in sight)
	if (parent->vmStopped() || oldattn != attention || olddecn != decn) {
		if (attention && dynamic_cast<CreatureAgent *>(attention.get())) {
			parent->queueScript(decn + 32); // 'on creatures'
		} else {
			parent->queueScript(decn + 16); // 'on agents'
		}
	}

	// involuntary actions
	for (unsigned int i = 0; i < 8; i++) {
		if (involactionlatency[i] > 0) {
			involactionlatency[i]--;
			continue;
		}

		if (involaction[i] > 0.0f) {
			parent->queueScript(i + 64);
		}
	}
#endif
}

bool c2eCreature::processInstinct() {
	if (unprocessedinstincts.empty()) return false;

	creatureInstinctGene *g = unprocessedinstincts.front();
	unprocessedinstincts.pop_front();

	// *** work out which verb neuron to fire by reverse-mapping from the mapping table
	int actualverb = reverseMapVerbToNeuron(g->action);
	// we have no idea which verb neuron to use, so no instinct processing
	if (actualverb == -1) return false;

	// *** debug output

	/*std::cout << "*** processing instinct for verb #" << actualverb << std::endl;
	std::cout << "reinforce using drive #" << (int)g->drive << " at level " << ((int)g->level - 128) / 128.0f << std::endl;
	for (unsigned int i = 0; i < 3; i++) {
		if (g->lobes[i] != 255) {
			std::cout << "input: lobe tissue #" << (int)(g->lobes[i] - 1) << ", neuron #" << (int)g->neurons[i] << std::endl;
		}
	}*/

	/*
	 * instinct processing! a production by fuzzie in conjunction with coffee
	 *
	 * this is mostly guesswork because instincts seem to take place in a single tick in the engine,
	 * making them pretty difficult to observe
	 *
	 * we reset the brain by setting pre-REM chemical to full and ticking it once, then we set REM to full
	 * and perform two ticks: one with just the inputs set, and once with a response in the 'resp' lobe
	 */

	// *** sanity checks/setup

	c2eLobe *resplobe = brain->getLobeById("resp");
	c2eLobe *verblobe = brain->getLobeById("verb");
	// no response/verb lobe? no instincts for you, then..
	if (!resplobe || !verblobe) return false;

	// if action/drive are beyond the size of the relevant lobe, can't process instinct
	if ((unsigned int)actualverb >= verblobe->getNoNeurons()) return false;
	if (g->drive >= resplobe->getNoNeurons()) return false;

	c2eLobe *inputlobe[3] = { 0, 0, 0 };

	for (unsigned int i = 0; i < 3; i++) {
		// TODO: what about unused?
		uint8 lobetissueid = g->lobes[i];
		if (lobetissueid == 255) continue;
		/* fuzzie would like to take this opportunity to quote from the pygenes source:
		 * Apparently, someone decided that because the rows are 1 above the lobe IDs, they should write the ROW NUMBER into the file, instead. Someone, somewhere, needs SHOOTING. */
		lobetissueid -= 1;
		inputlobe[i] = brain->getLobeByTissue(lobetissueid);
		// TODO: should we really barf out if this happens?
		if (!inputlobe[i]) return false;
		if (g->neurons[i] >= inputlobe[i]->getNoNeurons()) return false;
	}

	// *** reset brain
	
	// TODO: is this a sensible place to wipe the lobes?
	for (std::map<std::string, c2eLobe *>::iterator i = brain->lobes.begin(); i != brain->lobes.end(); i++)
		i->second->wipe();

	// TODO: non-hardcode 212/213? they seem to be in "Brain Parameters" catalogue tag
	// TODO: won't learning be sort of ruined by the repeated application of pre-REM?
	chemicals[212] = 1.0f; // pre-REM to full
	chemicals[213] = 0.0f; // REM to null
	brain->tick();
	chemicals[212] = 0.0f; // pre-REM to null
	chemicals[213] = 1.0f; // REM to full

	// *** set inputs and tick

	for (unsigned int i = 0; i < 3; i++) {
		// TODO: eeeevil hack - it looks like this is required, but is there no better way?
		if (g->lobes[i] == 3) {
			/*
			 * the visn lobe subtracts input from 1.0 to get distance of object, so 1.0 is no good
			 * we use 0.1, like c2e seems to feed it (the joys of hacked genetics and brain-in-a-vat!)
			 */
			// TODO: shouldn't we check lobe size?
			c2eLobe *visnlobe = brain->getLobeById("visn");
			if (visnlobe)
				visnlobe->setNeuronInput(g->neurons[i], 0.1f);
		}

		if (inputlobe[i])
			inputlobe[i]->setNeuronInput(g->neurons[i], 1.0f);
	}
	verblobe->setNeuronInput(actualverb, 1.0f);
	brain->tick();

	// *** set response and tick

	// TODO: shouldn't we make sure that decn/attn achieved the desired result?
	// TODO: should we set the input neurons again here? (it seems to work without - fuzzie)

	// TODO: TODO: TODO: check division of g->level!!
	// g->drive seems to be a direct mapping
	resplobe->setNeuronInput(g->drive, ((int)g->level - 128) / 128.0f);
	brain->tick();

	// *** finish off and return

	// TODO: shouldn't REM be present throughout sleep?
	chemicals[213] = 0.0f; // REM to null

	// wipe the lobes again, to stop any issues with neurons being set which shouldn't be at the end of an instinct run
	// TODO: is wiping the lobes here truly what we should do?
	for (std::map<std::string, c2eLobe *>::iterator i = brain->lobes.begin(); i != brain->lobes.end(); i++)
		i->second->wipe();

	//std::cout << "*** instinct done" << std::endl;
	//std::cout << std::endl;

	return true;
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
	Creature::addGene(g);

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

void oldCreature::addChemical(unsigned char id, unsigned char val) {
	if (id == 0) return;

	// clipping..
	if ((int)chemicals[id] + val > 255) chemicals[id] = 255;
	else chemicals[id] += val;
}

void oldCreature::subChemical(unsigned char id, unsigned char val) {
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

inline unsigned int oldCreature::calculateMultiplier(unsigned char rate) {
	return c1rates[rate];
}

inline unsigned int oldCreature::calculateTickMask(unsigned char rate) {
	if (rate < 7) return 0;
	else return (1 << ((unsigned int)rate - 7)) - 1;
}

void c1Creature::tickBiochemistry() {
	// TODO: untested

	if ((ticks % 5) != 0) return;

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

	oldCreature::tickBiochemistry();
}

void c2Creature::tickBiochemistry() {
	// TODO

	oldCreature::tickBiochemistry();
}

void oldCreature::tickBiochemistry() {
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
	
	biochemticks++;
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
					break; // TODO: should this break be here? added it, but can't check, no internet

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
			c2eLobe *lobe = brain->getLobeByTissue(t);
			if (!lobe) break;

			unsigned int neuronid = o/3, stateno = o%3;
			if (neuronid >= lobe->getNoNeurons()) break;
			return &lobe->getNeuron(neuronid)->variables[stateno];
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
					{ int val = l;

					if (!receptor) { // emitter
						if (l == 0) return &fertile;
						else if (l == 1) return &pregnant;
						val = l - 2;
					}

					switch (val) {
						case 0: return &ovulate;
						case 1: return &receptive;
						case 2: return &chanceofmutation;
						case 3: return &degreeofmutation;
					} }
					break;
					
				case 3: // immune
					if (l == 0) return &dead;
					break;
				
				case 4: // sensorimotor
					{ int val = l;

					if (!receptor) { // emitter
						if (val < 14) return &senses[val];
						val -= 14;
					}
					if (val < 8) return &involaction[val];
					val -= 8;
					if (val < 16) return &gaitloci[val];
					}
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

c2Organ::c2Organ(c2Creature *p, organGene *g) {
	parent = p; assert(parent);
	ourGene = g; assert(ourGene);

	// TODO
}

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
}

void c2Organ::processGenes() {
	for (vector<gene *>::iterator i = ourGene->genes.begin(); i != ourGene->genes.end(); i++) {
		if (!parent->shouldProcessGene(*i)) continue;

		// TODO
	}
}

void c2eOrgan::processGenes() {
	shared_ptr<c2eReaction> r; // we need to store the previous reaction for possible receptor use
	// TODO: should this cope with receptors created at other lifestages? i doubt it.. - fuzzie

	for (vector<gene *>::iterator i = ourGene->genes.begin(); i != ourGene->genes.end(); i++) {
		if (!parent->shouldProcessGene(*i)) continue;

		if (typeid(*(*i)) == typeid(bioReactionGene)) {
			reactions.push_back(shared_ptr<c2eReaction>(new c2eReaction()));
			r = reactions.back();
			reactions.back()->init((bioReactionGene *)(*i));
		} else if (typeid(*(*i)) == typeid(bioEmitterGene)) {
			emitters.push_back(c2eEmitter());
			emitters.back().init((bioEmitterGene *)(*i), this);
		} else if (typeid(*(*i)) == typeid(bioReceptorGene)) {
			receptors.push_back(c2eReceptor());
			receptors.back().init((bioReceptorGene *)(*i), this, r);
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

	bioReactionGene &g = *d.data;

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
	
	// calculate the actual adjustment (can't go out of bounds)
	ratio = ratio - ((ratio * calculateMultiplier(rate)) / 65536);

	// change chemical levels
	subChemical(g.reactant[0], ratio * g.quantity[0]);
	subChemical(g.reactant[1], ratio * g.quantity[1]);
	addChemical(g.reactant[2], ratio * g.quantity[2]);
	addChemical(g.reactant[3], ratio * g.quantity[3]);
}

void c2eOrgan::processReaction(c2eReaction &d) {
	bioReactionGene &g = *d.data;
	
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

	bioEmitterGene &g = *d.data;

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
	bioEmitterGene &g = *d.data;
	
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

	bioReceptorGene &g = *d.data;

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
	bioReceptorGene &g = *d.data;
	
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
				}
			break;
		case 3: // reaction
			if (t == 0 && l == 0) { // reaction rate
				shared_ptr<c2eReaction> r = reactions.back();
				if (!r) {
					std::cout << "c2eOrgan::getLocusPointer failed to find a reaction" << std::endl;
					return 0;
				} else {
					if (receptors) *receptors = &r->receptors;
					return &r->rate;
				}
			}
	}

	return parent->getLocusPointer(receptor, o, t, l);
}

void c1Reaction::init(bioReactionGene *g) {
	data = g;
}

void c2eReaction::init(bioReactionGene *g) {
	data = g;

	// rate is stored in genome as 0 fastest, 255 slowest
	// reversed in game, i think .. TODO: check this
	rate = 1.0 - (g->rate / 255.0);
}

void c1Receptor::init(bioReceptorGene *g, c1Creature *parent) {
	data = g;
	locus = parent->getLocusPointer(true, g->organ, g->tissue, g->locus);
}
	
void c2eReceptor::init(bioReceptorGene *g, c2eOrgan *parent, shared_ptr<c2eReaction> r) {
	data = g;
	processed = false;
	nominal = g->nominal / 255.0f;
	threshold = g->threshold / 255.0f;
	gain = g->gain / 255.0f;
	locus = parent->getLocusPointer(true, g->organ, g->tissue, g->locus, &receptors);
}

void c1Emitter::init(bioEmitterGene *g, c1Creature *parent) {
	data = g;
	locus = parent->getLocusPointer(false, g->organ, g->tissue, g->locus);
}

void c2eEmitter::init(bioEmitterGene *g, c2eOrgan *parent) {
	data = g;
	sampletick = 0;
	threshold = g->threshold / 255.0f;
	gain = g->gain / 255.0f;
	locus = parent->getLocusPointer(false, g->organ, g->tissue, g->locus, 0);
}

#include "AgentHelpers.h"

bool Creature::agentInSight(AgentRef a) {
#ifndef _CREATURE_STANDALONE	
	if (a->invisible()) return false;

	// TODO: specify x/y location for eyes
	// TODO: check open cabin?
	return agentIsVisible(parent, a);
#else
	return false;
#endif
}

void Creature::chooseAgents() {
#ifndef _CREATURE_STANDALONE	
	// zot any chosen agents which went out of range, went invisible or changed category
	for (unsigned int i = 0; i < chosenagents.size(); i++) {
		AgentRef a = chosenagents[i];
		if (a) {
			if (a->category != (int)i || !agentInSight(a))
				chosenagents[i].clear();
		}
	}

	std::vector<std::vector<AgentRef> > possibles(chosenagents.size());

	for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		boost::shared_ptr<Agent> a = *i;
		if (!a) continue;

		// if agent category is -1 or outside of our #categories, continue
		if (a->category < 0) continue;
		if (a->category >= (int)chosenagents.size()) continue;

		// if we already chose an agent from this category, continue
		if (chosenagents[a->category]) continue;

		if (!agentInSight(a)) continue;

		possibles[a->category].push_back(a);
	}

	for (unsigned int i = 0; i < chosenagents.size(); i++) {
		if (!chosenagents[i])
			chosenagents[i] = selectRepresentativeAgent(i, possibles[i]);
	}
#endif
}

AgentRef c2eCreature::selectRepresentativeAgent(int type, std::vector<AgentRef> possibles) {
	// TODO: proper selection method

	if (possibles.size() > 0)
		return possibles[rand() % possibles.size()];
	else
		return AgentRef();
}

int c2eCreature::reverseMapVerbToNeuron(unsigned int verb) {
	// TODO: reverse-mapping like this seems utterly horrible, is it correct?
	int actualverb = -1;
	for (unsigned int i = 0; i < mappinginfo.size(); i++) {
		if (mappinginfo[i] == verb)
			actualverb = (int)i;
	}
	return actualverb;
}

void c2eCreature::handleStimulus(c2eStim &stim) {
	// TODO: handle out-of-range verb_amount/noun_amount

	if (stim.verb_id >= 0) {
		c2eLobe *verblobe = brain->getLobeById("verb");
		if (verblobe) {
			if ((unsigned int)stim.verb_id < verblobe->getNoNeurons())
				verblobe->setNeuronInput(stim.verb_id, stim.verb_amount);
		}
	}

	if (stim.noun_id >= 0) {
		c2eLobe *nounlobe = brain->getLobeById("noun");
		if (nounlobe) {
			if ((unsigned int)stim.noun_id < nounlobe->getNoNeurons())
				nounlobe->setNeuronInput(stim.noun_id, stim.noun_amount);
		}
	}

	for (unsigned int i = 0; i < 4; i++) {
		if (stim.drive_id[i] >= 0) {
			unsigned char chemno = stim.drive_id[i] + 148;
			adjustChemical(chemno, stim.drive_amount[i]);
			if (!stim.drive_silent[i]) {
				c2eLobe *resplobe = brain->getLobeById("resp");
				if (resplobe) {
					if ((unsigned int)stim.drive_id[i] < resplobe->getNoNeurons())
						resplobe->setNeuronInput(stim.drive_id[i], stim.drive_amount[i]);
				}
			}
		}
	}
}

// TODO: this needs to be passed noun details, it seems, judging by documentation
void c2eCreature::handleStimulus(unsigned int id, float strength) {
	// note that g->addoffset does not seem to exist in c2e
	
	c2eStim stim;
	creatureStimulusGene *g = 0;
	
	// TODO: generate the damn c2eStims in addGene, thus zapping a whole bunch of bugs
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (typeid(*(*i)) == typeid(creatureStimulusGene)) {
			creatureStimulusGene *x = (creatureStimulusGene *)(*i);
			if (x->stim == id) {
				g = x;
				break;
			}
		}
	}
	if (!g) return;

	// if we're asleep and the stimulus isn't to be processed when asleep, return
	if (!g->whenasleep && isAsleep()) return;

	// TODO: g->modulate
	
	// TODO: is multipler usage below okay?
	float multiplier = (strength == 0.0f ? 1.0f : strength);

	/*
	 * TODO: what the heck does g->intensity do? it seems to almost entirely be 0
	 * in the standard genomes (apart from a 255?) and it doesn't seem to change
	 * anything - fuzzie
	 */
	// TODO: grmph, g->sensoryneuron and g->significance make no sense to me right now
	// either, so commenting the verb_id setting out until someone works it out - fuzzie
	/*if (stim.verb_id != 0) // TODO: this is a guess to stop stuff from resting seemingly forever
		stim.verb_id = reverseMapVerbToNeuron(g->sensoryneuron); */
	// TODO: huh? gene kit has it in 255-ish steps
	stim.verb_amount = g->significance * (1.0f / 124.0f); /* multiply by 0.5? */
	for (unsigned int i = 0; i < 4; i++) {
		// TODO: ack, amount should be bound to range
		// TODO: fuzzie is suspicious about multiply/dividing on g->amounts here
		if (g->drives[i] != 255)
			stim.setupDriveStim(i, g->drives[i], ((g->amounts[i] * (1.0f / 124.0f)) - 1.0f) * multiplier, g->silent[i]);
		
		if (strength == 0.0f)
			stim.drive_silent[i] = true;
	}

	handleStimulus(stim);
}

/* vim: set noet: */
