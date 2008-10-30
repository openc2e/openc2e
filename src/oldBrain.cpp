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

unsigned char processSVRule(oldNeuron *cell, oldDendrite *dend, uint8 *svrule, unsigned int len) {
	unsigned char state = 0;

	for (unsigned int i = 0; i < len; i++) {
		switch (svrule[i]) {
			/*
			 * these numbers are the C2 svrules
			 *
			 * TODO: remap the C1 svrule numbers at load so they match these
			 */
			case 0: // <end>
				return state;

			case 1: // 0
				state = 0;
				break;

			case 2: // 1
				state = 1;
				break;

			case 3: // 64
				state = 64;
				break;

			case 4: // 255
				state = 255;
				break;

			case 5: // chem0
				break;

			case 6: // chem1
				break;

			case 7: // chem2
				break;

			case 8: // chem3
				break;

			case 9: // state
				state = cell->state;
				break;

			case 10: // output
				break;

			case 11: // thres
				break;

			case 12: // type0
				break;

			case 13: // type1
				break;

			case 14: // anded0
				break;

			case 15: // anded1
				break;

			case 16: // input
				break;

			case 17: // conduct
				break;

			case 18: // suscept
				break;

			case 19: // STW
				break;

			case 20: // LTW
				break;

			case 21: // strength
				break;

			case 22: // 32
				break;

			case 23: // 128
				break;

			case 24: // rnd const
				break;

			case 25: // chem4
				break;

			case 26: // chem5
				break;

			case 27: // leak in
				break;

			case 28: // leak out
				break;

			case 29: // curr src leak in
				break;

			case 30: // TRUE
				if (!state) return 0;
				break;

			case 31: // PLUS
				break;

			case 32: // MINUS
				break;

			case 33: // TIMES
				break;

			case 34: // INCR
				state++;
				break;

			case 35: // DECR
				state--;
				break;

			case 36: // FALSE
				break;

			case 37: // multiply
				break;

			case 38: // average
				break;

			case 39: // move twrds
				break;

			case 40: // random
				break;

			case 41: // <error>
				break;
		}
	}

	return state;
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

	// TODO: is this it?
}

void oldLobe::wipe() {
	for (unsigned int i = 0; i < neurons.size(); i++) {
		neurons[i].state = neurons[i].output = ourGene->reststate; // TODO: good?
	}
}

void oldLobe::tick() {
	// TODO: do something with inputgain? presumably applied to 'input', so something to do with decision lobe..

	for (unsigned int i = 0; i < neurons.size(); i++) {
		unsigned char out = neurons[i].state; // TODO: svrule (ourGene->staterule)..

		// apply leakage rate in order to settle at rest state
		if ((parent->getTicks() & parent->getParent()->calculateTickMask(leakagerate)) == 0) {
			// TODO: untested
			// TODO: what happens if out < ourGene->reststate? test!
			out = ourGene->reststate + ((out - ourGene->reststate) * parent->getParent()->calculateMultiplier(leakagerate)) / 65536;
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
	// TODO: winner takes all (ourGene->flags)
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
