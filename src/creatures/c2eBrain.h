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
#include <cassert>
#include <set>
#include <map>

using std::shared_ptr;

class Creature;

struct c2ebraincomponentorder {
	bool operator()(const class c2eBrainComponent *b1, const class c2eBrainComponent *b2) const;
};

class c2eBrainComponent {
protected:
	friend struct c2ebraincomponentorder;

	uint8_t updatetime;
	class c2eBrain *parent;
	bool inited;

public:
	virtual void init() = 0;
	virtual void tick() = 0;
	uint8_t getUpdateTime() { return updatetime; }
	bool wasInited() { return inited; }

	c2eBrainComponent(class c2eBrain *b) : parent(b) { assert(b); inited = false; }
	virtual ~c2eBrainComponent() { }
};

struct c2erule {
	uint8_t opcode;
	uint8_t operandtype;
	uint8_t operanddata;
	float operandvalue;
};

class c2eSVRule {
protected:
	std::vector<c2erule> rules;

public:
	void init(uint8_t ruledata[48]);
	bool runRule(float acc, float srcneuron[8], float neuron[8], float spareneuron[8], float dendrite[8], class c2eCreature *creature);
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
	c2eLobe(class c2eBrain *b, c2eBrainLobeGene *g);
	void tick();
	void init();
	void wipe();
	c2eBrainLobeGene *getGene() { return ourGene; }
	unsigned int getNoNeurons() { return neurons.size(); }
	c2eNeuron *getNeuron(unsigned int i) { return &neurons[i]; }
	unsigned int getSpareNeuron() { return spare; }
	void setNeuronInput(unsigned int i, float input);
	std::string getId();
};

class c2eTract : public c2eBrainComponent {
protected:
	c2eBrainTractGene *ourGene;
	c2eSVRule initrule, updaterule;
	std::vector<c2eDendrite> dendrites;
	std::vector<c2eNeuron *> src_neurons, dest_neurons;

	void setupTract();
	c2eDendrite *getDendriteFromTo(c2eNeuron *, c2eNeuron *);
	void doMigration();

public:
	c2eTract(class c2eBrain *b, c2eBrainTractGene *g);
	void tick();	
	void init();
	void wipe();
	c2eBrainTractGene *getGene() { return ourGene; }
	unsigned int getNoDendrites() { return dendrites.size(); }
	c2eDendrite *getDendrite(unsigned int i) { return &dendrites[i]; }
	
	std::string dump();
};

class c2eBrain {
protected:
	class c2eCreature *parent;

	std::multiset<c2eBrainComponent *, c2ebraincomponentorder> components;

public:
	std::map<std::string, c2eLobe *> lobes;
	std::vector<c2eTract *> tracts;

	c2eBrain(c2eCreature *p);
	void processGenes();
	void tick();
	void init();
	c2eLobe *getLobeById(std::string id);
	c2eLobe *getLobeByTissue(unsigned int id);
	c2eCreature *getParent() { return parent; }
};

#endif

/* vim: set noet: */
