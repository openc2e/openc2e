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
	struct oldNeuron *src;
	unsigned char strength, stw, ltw, suscept;
};

struct oldNeuron {
	unsigned char state, output, leakin, leakout;
	std::vector<oldDendrite> dendrites[2];
};

class oldLobe {
protected:
	class oldBrain *parent;
	oldBrainLobeGene *ourGene;
	std::vector<oldNeuron> neurons;
	oldLobe *sourceLobe[2];
	bool inited;

	unsigned int width, height;

	unsigned char threshold, leakagerate, inputgain;
	unsigned char chems[6];

	unsigned char rndconst_staterule;

	unsigned char evaluateSVRuleConstant(oldNeuron *cell, oldDendrite *dend, uint8 id, unsigned char rndconst);
	unsigned char processSVRule(oldNeuron *cell, oldDendrite *dend, uint8 *svrule, unsigned int len, unsigned char rndconst);

	unsigned char dendrite_sum(unsigned int type, bool only_if_all_firing);

public:
	oldLobe(class oldBrain *b, oldBrainLobeGene *g);
	bool wasInited() { return inited; }
	void tick();
	void init();
	void wipe();

	oldBrainLobeGene *getGene() { return ourGene; }

	unsigned int getNoNeurons() { return neurons.size(); }
	oldNeuron *getNeuron(unsigned int i) { return &neurons[i]; }

	unsigned char *getChemPointer(unsigned int chemid) { return &chems[chemid]; }
	unsigned char *getThresholdPointer() { return &threshold; }
	unsigned char *getLeakageRatePointer() { return &leakagerate; }
	unsigned char *getInputGainPointer() { return &inputgain; }
};

class oldBrain {
protected:
	class oldCreature *parent;
	unsigned int ticks;

public:
	std::map<unsigned int, oldLobe *> lobes;

	oldBrain(oldCreature *p);
	void tick();
	void processGenes();
	void init();

	oldLobe *getLobeByTissue(unsigned int id);
	oldCreature *getParent() { return parent; }
	unsigned int getTicks() { return ticks; }
};

#endif

/* vim: set noet: */
