/*
 *  c2eBrain.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed Apr 11 2007.
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

#ifndef __C2EBRAIN_H
#define __C2EBRAIN_H

#include "genome.h"
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;

class Creature;

class c2eBrainComponent {
};

struct c2erule {
	uint8 opcode;
	uint8 operandtype;
	uint8 operanddata;
	float operandvalue;
};

class c2eSVRule {
protected:
	std::vector<c2erule> rules;

public:
	void init(uint8 ruledata[48]);
	bool runRule(float acc, float srcneuron[8], float neuron[8], float spareneuron[8], float dendrite[8], float chemicals[256]);
};

struct c2eNeuron {
	float variables[8];
	float input;
};

struct c2eDendrite {
	float variables[8];
	c2eNeuron *source, *dest;
};

class c2eLobe : public c2eBrainComponent {
protected:
	c2eBrainLobeGene *ourGene;
	c2eSVRule initrule, updaterule;
	std::vector<c2eNeuron> neurons;
	unsigned int spare;

public:
	c2eLobe(c2eBrainLobeGene *g);
	void tick();
	unsigned int getSpareNeuron() { return spare; }
};

class c2eTract : public c2eBrainComponent {
protected:
	c2eBrainTractGene *ourGene;
	c2eSVRule initrule, updaterule;
	std::vector<c2eDendrite> dendrites;

	c2eDendrite *getDendriteFromTo(c2eNeuron *, c2eNeuron *);
	void doMigration();

public:
	c2eTract(c2eBrainTractGene *g);
	void tick();	
};

class c2eBrain {
protected:
	class c2eCreature *parent;

	void addGene(gene *);

public:
	c2eBrain(c2eCreature *p);
	float *getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l);
	void tick();
};

#endif

/* vim: set noet: */
