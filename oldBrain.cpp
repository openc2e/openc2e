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

oldLobe::oldLobe(oldBrain *b, oldBrainLobeGene *g) {
	assert(b);
	parent = b;
	assert(g);
	ourGene = g;

	unsigned int width = g->width, height = g->height;
	// TODO: good?
	if (width < 1) width = 1;
	if (height < 1) height = 1;

	neurons.reserve(width * height);
	
	oldNeuron n;
	for (unsigned int i = 0; i < width * height; i++) {
		neurons.push_back(n);
	}

	// TODO
}

void oldLobe::init() {
	wipe();

	// TODO
}

void oldLobe::wipe() {
	// TODO
}

void oldLobe::tick() {
	// TODO
}

oldBrain::oldBrain(oldCreature *p) {
	assert(p);
	parent = p;

	shared_ptr<genomeFile> genome = p->getGenome();

	unsigned int n = 0;
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if ((*i)->header.flags.femaleonly && !p->isFemale()) continue;
		if ((*i)->header.flags.maleonly && p->isFemale()) continue;
		// TODO: lifestage
		if (typeid(**i) == typeid(oldBrainLobeGene)) {
			oldBrainLobeGene *g = (oldBrainLobeGene *)*i;
			oldLobe *l = new oldLobe(this, g);
			// TODO: good?
			lobes[n] = l;
			n++;
		}
	}
}

void oldBrain::init() {
	for (std::map<unsigned int, oldLobe *>::iterator i = lobes.begin(); i != lobes.end(); i++) {
		(*i).second->init();
	}
}

void oldBrain::tick() {
	for (std::map<unsigned int, oldLobe *>::iterator i = lobes.begin(); i != lobes.end(); i++) {
		(*i).second->tick();
	}
}

oldLobe *oldBrain::getLobeByTissue(unsigned int id) {
	if (lobes.find(id) == lobes.end())
		return 0;

	return lobes[id];
}

/* vim: set noet: */
