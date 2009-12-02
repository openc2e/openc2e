/*
 *  oldBrain.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon Aug 13 2007.
 *  Copyright (c) 2007 Alyssa Milburn. All rights reserved.
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

#include "oldBrain.h"
#include "Creature.h"

/*
 * svrule examples:
 *
 * * creatures 1:
 * output:TRUE:input
 *
 * * creatures 2:
 * random:0:chem 5:PLUS:state
 * state:PLUS:type 0:MINUS:type 1
 * input:TRUE:output:TRUE:suscept:move twrds:255:64
 * suscept:TRUE:chem 0:TRUE:STW
 *
 * * canny:
 * state:PLUS:state:PLUS:state
 * type 0:TRUE:type 0:PLUS:type 1
 *
 */

unsigned char oldLobe::evaluateSVRuleConstant(oldNeuron *cell, oldDendrite *dend, uint8 id, unsigned char rndconst) {
	switch (id) {
		/*
		 * these numbers are the C2 svrules
		 *
		 * TODO: remap the C1 svrule numbers at load so they match these
		 */
		case 1: // 0
			return 0;

		case 2: // 1
			return 1;

		case 3: // 64
			return 64;

		case 4: // 255
			return 255;

		case 5: // chem0
			return chems[0];

		case 6: // chem1
			return chems[1];

		case 7: // chem2
			return chems[2];

		case 8: // chem3
			return chems[3];

		case 9: // state
			return cell->state;

		case 10: // output
			return cell->output;

		case 11: // thres
			return threshold;

		case 12: // type0
			return dendrite_sum(0, false);

		case 13: // type1
			return dendrite_sum(1, false);

		case 14: // anded0
			return dendrite_sum(0, true);

		case 15: // anded1
			// unused?
			return dendrite_sum(1, true);

		case 16: // input
			// This comes from IMPT for the decision lobe.
			if (!dend) return 0;
			return dend->src->output;

		case 17: // conduct
			// unused?
			if (!dend) return 0;
			return 0; // TODO: what's this?

		case 18: // suscept
			if (!dend) return 0;
			return dend->suscept;

		case 19: // STW
			if (!dend) return 0;
			return dend->stw;

		case 20: // LTW
			// unused?
			if (!dend) return 0;
			return dend->ltw;

		case 21: // strength
			// unused?
			if (!dend) return 0;
			return dend->strength;

		case 22: // 32
			// unused?
			return 32;

		case 23: // 128
			// unused?
			return 128;

		case 24: // rnd const
			// unused?
			return rndconst;

		case 25: // chem4
			return chems[4];

		case 26: // chem5
			return chems[5];

		case 27: // leak in
			// unused: back/forward prop
			return cell->leakin;

		case 28: // leak out
			// unused: back/forward prop
			return cell->leakout;

		case 29: // curr src leak in
			// unused: back/forward prop
			if (!dend) return 0;
			return dend->src->leakin;

		default:
			return 0;
	}
}

unsigned char oldLobe::processSVRule(oldNeuron *cell, oldDendrite *dend, uint8 *svrule, unsigned int len, unsigned char rndconst) {
	unsigned char state = 0;

	// original engine seems to simply happily walk off the end of the svrule array for constants!
	// so our behaviour for the 'if (i == len)' lines is NOT the same

	for (unsigned int i = 0; i < len; i++) {
		switch (svrule[i]) {
			case 0: // <end>
				return state;

			default:
				state = evaluateSVRuleConstant(cell, dend, svrule[i], rndconst);
				break;

			case 30: // TRUE
				if (!state) return 0;
				break;

			case 31: // PLUS
				i++;
				if (i == len) return state;
				state = state + evaluateSVRuleConstant(cell, dend, svrule[i], rndconst);
				break;

			case 32: // MINUS
				i++;
				if (i == len) return state;
				state = state - evaluateSVRuleConstant(cell, dend, svrule[i], rndconst);
				break;

			case 33: // TIMES
				// unused?
				i++;
				if (i == len) return state;
				state = (state * evaluateSVRuleConstant(cell, dend, svrule[i], rndconst)) / 256;
				break;

			case 34: // INCR
				// unused?
				state++;
				break;

			case 35: // DECR
				// unused?
				state--;
				break;

			case 36: // FALSE
				if (state) return 0;
				break;

			case 37: // multiply
				// unused?
				i++;
				if (i == len) return state;
				state = state * evaluateSVRuleConstant(cell, dend, svrule[i], rndconst);
				break;

			case 38: // average
				// unused?
				i++;
				if (i == len) return state;
				state = (state + evaluateSVRuleConstant(cell, dend, svrule[i], rndconst)) / 2;
				break;

			case 39: { // move twrds
				i++;
				if (i == len) return state;
				unsigned char towards = evaluateSVRuleConstant(cell, dend, svrule[i], rndconst);
				i++;
				if (i == len) return state;
				unsigned char multiplier = evaluateSVRuleConstant(cell, dend, svrule[i], rndconst);
				state = ((towards - state) * multiplier) / 256;
				} break;

			case 40: { // random
				i++;
				if (i == len) return state;
				unsigned char min = evaluateSVRuleConstant(cell, dend, svrule[i], rndconst);
				i++;
				if (i == len) return state;
				unsigned char max = evaluateSVRuleConstant(cell, dend, svrule[i], rndconst);
				state = (rand() % (max - min + 1)) + min;
				} break;
		}
	}

	return state;
}

unsigned char oldLobe::dendrite_sum(unsigned int type, bool only_if_firing) {
	// TODO: cache this result, since it will be used every time the svrule runs
	// (and remember you can calculate both only_if_firing and not only_if_firing in one go!)
	unsigned int sum = 0; // TODO: sum((src output * strength) / 255)
	return (sum * inputgain) / 255;
}

oldLobe::oldLobe(oldBrain *b, oldBrainLobeGene *g) {
	assert(b);
	parent = b;
	assert(g);
	ourGene = g;

	inited = false;

	threshold = g->nominalthreshold;
	leakagerate = g->leakagerate;
	inputgain = g->inputgain;

	width = g->width;
	height = g->height;
	// TODO: good?
	if (width < 1) width = 1;
	if (height < 1) height = 1;

	neurons.reserve(width * height);

	oldNeuron n;
	for (unsigned int i = 0; i < width * height; i++) {
		neurons.push_back(n);
	}

	// TODO

	for (unsigned int i = 0; i < 6; i++) {
		chems[i] = 0;
	}
}

void oldLobe::init() {
	inited = true;
	wipe();

	rndconst_staterule = 0; // TODO
}

void oldLobe::wipe() {
	for (unsigned int i = 0; i < neurons.size(); i++) {
		neurons[i].state = neurons[i].output = ourGene->reststate; // TODO: good?
	}
}

void oldLobe::tick() {
	for (unsigned int i = 0; i < neurons.size(); i++) {
		// TODO: c1 rules are not 12
		unsigned char out = processSVRule(&neurons[i], NULL, ourGene->staterule, 12, rndconst_staterule);

		// apply leakage rate in order to settle at rest state
		if ((parent->getTicks() & parent->getParent()->calculateTickMask(leakagerate / 8)) == 0) {
			if (out > ourGene->reststate)
				out = ourGene->reststate + ((out - ourGene->reststate) * parent->getParent()->calculateMultiplier(leakagerate / 8)) / 65536;
			else
				out = ourGene->reststate;
		}

		neurons[i].state = out;

		if (out < threshold)
			out = 0;
		else
			out -= threshold;

		neurons[i].output = out;
	}

	// TODO: dendrites (ourGene->dendrite1, ourGene->dendrite2)

	// TODO: data copied to perception lobe (ourGene->perceptflag - not just true/false!)

	if (ourGene->flags & 1) {
		// winner takes all
		unsigned char bestvalue = 0;
		unsigned char *bestoutput = NULL;
		for (unsigned int i = 0; i < neurons.size(); i++) {
			if (neurons[i].output > bestvalue) {
				bestvalue = neurons[i].output;
				bestoutput = &neurons[i].output;
			}
			neurons[i].output = 0;
		}
		if (bestoutput)
			*bestoutput = bestvalue;
	}

	// TODO: migration
}

oldBrain::oldBrain(oldCreature *p) {
	assert(p);
	parent = p;

	ticks = 0;
}

void oldBrain::processGenes() {
	shared_ptr<genomeFile> genome = parent->getGenome();
	
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (!parent->shouldProcessGene(*i)) continue;
		
		if (typeid(**i) == typeid(oldBrainLobeGene)) {
			oldBrainLobeGene *g = (oldBrainLobeGene *)*i;
			oldLobe *l = new oldLobe(this, g);
			lobes[lobes.size()] = l; // TODO: muh
		}
	}
}

void oldBrain::init() {
	for (std::map<unsigned int, oldLobe *>::iterator i = lobes.begin(); i != lobes.end(); i++) {
		if (!(*i).second->wasInited()) (*i).second->init();
	}
}

void oldBrain::tick() {
	for (std::map<unsigned int, oldLobe *>::iterator i = lobes.begin(); i != lobes.end(); i++) {
		(*i).second->tick();
	}

	ticks++;
}

oldLobe *oldBrain::getLobeByTissue(unsigned int id) {
	if (lobes.find(id) == lobes.end())
		return 0;

	return lobes[id];
}

/* vim: set noet: */
