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

struct oldSVRule {
	unsigned int length;
	uint8 rndconst;
	uint8 rules[12];
	void init(uint8 version, uint8 *src);
};

struct oldDendrite {
	struct oldNeuron *src;
	unsigned char strength, stw, ltw, suscept;
};

struct oldNeuron {
	unsigned char state, output, leakin, leakout;
	std::vector<oldDendrite> dendrites[2];
	unsigned char percept_src;
	oldNeuron() { state = 0; output = 0; leakin = 0; leakout = 0; percept_src = 0; }
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

	oldSVRule staterule;
	oldSVRule strgainrule[2], strlossrule[2];
	oldSVRule susceptrule[2];
	oldSVRule relaxrule[2];
	oldSVRule backproprule[2], forproprule[2];

	unsigned char evaluateSVRuleConstant(oldNeuron *cell, oldDendrite *dend, uint8 id, oldSVRule &rule);
	unsigned char processSVRule(oldNeuron *cell, oldDendrite *dend, oldSVRule &rule);

	unsigned char dendrite_sum(unsigned int type, bool only_if_all_firing);

	void connectDendrite(unsigned int type, oldDendrite &dend, oldNeuron *dest);

	void tickDendrites(unsigned int id, unsigned int type);

public:
	oldLobe(class oldBrain *b, oldBrainLobeGene *g);
	bool wasInited() { return inited; }
	void ensure_minimum_size(unsigned int size);
	void tick();
	void init();
	void connect();
	void wipe();

	oldBrainLobeGene *getGene() { return ourGene; }

	unsigned int getNoNeurons() { return neurons.size(); }
	oldNeuron *getNeuron(unsigned int i) { return &neurons[i]; }
	unsigned int getDendriteCount();
	unsigned int getWidth() { return width; }
	unsigned int getHeight() { return height; }

	unsigned char *getChemPointer(unsigned int chemid) { return &chems[chemid]; }
	unsigned char *getThresholdPointer() { return &threshold; }
	unsigned char *getLeakageRatePointer() { return &leakagerate; }
	unsigned char *getInputGainPointer() { return &inputgain; }
};

class oldBrain {
protected:
	class oldCreature *parent;
	unsigned int ticks;
	std::vector<unsigned int> lobe_process_order;

public:
	std::vector<oldLobe *> lobes;

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
