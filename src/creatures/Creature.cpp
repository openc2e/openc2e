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
#include "oldBrain.h"

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

void oldCreature::tickBrain() {
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
		// TODO
	}

	// TODO
	
	chooseAgents();

	// TODO

	brain->tick();

	// TODO

	AgentRef oldattn = attention;
	int olddecn = decn;

	// TODO: doesn't belong here
	if (attn >= 0 && attn < (int)chosenagents.size())
		attention = chosenagents[attn];

	if (zombie) return; // TODO: docs say zombies "don't process decision scripts", correct?

	// involuntary actions
	for (unsigned int i = 0; i < 8; i++) {
		if (involactionlatency[i] > 0) {
			involactionlatency[i]--;
			continue;
		}

		// TODO
		if (parentagent->vmStopped() && involaction[i] > 0) {
			parentagent->queueScript(i + 64);
		}
	}

	// TODO
	if (parentagent->vmStopped() || oldattn != attention || olddecn != decn) {
		// TODO
		parentagent->queueScript(decn + 16); // Extra scripts
	}
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
			float ourxpos = parentagent->x + (parentagent->getWidth() / 2.0f);
			float theirxpos = a->x + (a->getWidth() / 2.0f);
			float distance = theirxpos - ourxpos;

			// TODO: squash result into appropriate range?
			visnlobe->setNeuronInput(i, distance / parentagent->range.getFloat());
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
	if (parentagent->vmStopped() || oldattn != attention || olddecn != decn) {
		if (attention && dynamic_cast<CreatureAgent *>(attention.get())) {
			parentagent->queueScript(decn + 32); // 'on creatures'
		} else {
			parentagent->queueScript(decn + 16); // 'on agents'
		}
	}

	// involuntary actions
	for (unsigned int i = 0; i < 8; i++) {
		if (involactionlatency[i] > 0) {
			involactionlatency[i]--;
			continue;
		}

		if (involaction[i] > 0.0f) {
			parentagent->queueScript(i + 64);
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

#include "AgentHelpers.h"

bool Creature::agentInSight(AgentRef a) {
#ifndef _CREATURE_STANDALONE	
	if (a->invisible()) return false;

	// TODO: specify x/y location for eyes
	// TODO: check open cabin?
	return agentIsVisible(parentagent, a);
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

AgentRef oldCreature::selectRepresentativeAgent(int type, std::vector<AgentRef> possibles) {
	// TODO: proper selection method

	if (possibles.size() > 0)
		return possibles[rand() % possibles.size()];
	else
		return AgentRef();
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

void oldCreature::handleStimulus(unsigned int id) {
	// TODO
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
