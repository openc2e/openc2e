/*
 *  oldBrain.h
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

#ifndef __OLDBRAIN_H
#define __OLDBRAIN_H

#include "genome.h"
#include <boost/shared_ptr.hpp>
#include <set>
#include <map>

using boost::shared_ptr;

struct oldDendrite {
	unsigned int *src, *dest;
	unsigned int strength;
};

class oldLobe {
protected:
	class oldBrain *parent;
	oldBrainLobeGene *ourGene;
	std::vector<unsigned int> neurons;
	std::vector<oldDendrite> dendrites[2];
	oldLobe *sourceLobe[2];

public:
	oldLobe(class oldBrain *b, oldBrainLobeGene *g);
	void tick();
	void init();
	void wipe();
	oldBrainLobeGene *getGene() { return ourGene; }
	unsigned int getNoNeurons() { return neurons.size(); }
	unsigned int getNoDendrites(unsigned int t) { return dendrites[t].size(); }
	unsigned int *getNeuron(unsigned int i) { return &neurons[i]; }
	oldDendrite *getDendrite(unsigned int t, unsigned int i) { return &dendrites[t][i]; }
};

class oldBrain {
protected:
	// TODO: should be oldCreature?
	class oldCreature *parent;

public:
	std::map<unsigned int, oldLobe *> lobes;

	oldBrain(oldCreature *p);
	void tick();
	void init();
	oldLobe *getLobeByTissue(unsigned int id);
	oldCreature *getParent() { return parent; }
};

#endif

/* vim: set noet: */
