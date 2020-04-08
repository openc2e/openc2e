/*
 *  CreatureAI.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat May 15 2010.
 *  Copyright (c) 2004-2010 Alyssa Milburn. All rights reserved.
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

#include "oldCreature.h"
#include "c2eCreature.h"
#include "CreatureAgent.h"
#include "World.h"
#include "c2eBrain.h"
#include "oldBrain.h"

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

	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		std::shared_ptr<Agent> a = *i;
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
