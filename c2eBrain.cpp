/*
 *  c2eBrain.cpp
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

#include "c2eBrain.h"
#include "Creature.h"
#include <math.h>
#include <boost/format.hpp>

float dummyValues[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

/*
 * c2ebraincomponentorder::operator()
 *
 * A functor to sort brain components by their update time.
 *
 */
bool c2ebraincomponentorder::operator()(const class c2eBrainComponent *b1, const class c2eBrainComponent *b2) const {
	return b1->updatetime < b2->updatetime;
}

/*
 * c2eTract::c2eTract
 *
 * Constructor for a c2eTract. Pass it the relevant gene.
 *
 */
c2eTract::c2eTract(c2eBrain *b, c2eBrainTractGene *g) : c2eBrainComponent(b) {
	assert(g);
	ourGene = g;
	updatetime = g->updatetime;

	initrule.init(g->initialiserule);
	updaterule.init(g->updaterule);

	std::string srclobename = std::string((char *)g->srclobe, 4);
	std::string destlobename = std::string((char *)g->destlobe, 4);

	if (b->lobes.find(srclobename) == b->lobes.end() || b->lobes.find(destlobename) == b->lobes.end()) {
		std::cout << "brain debug: failed to create dendrites for " << dump() << " (missing lobe)" << std::endl;
		return;
	}
	c2eLobe *srclobe = b->lobes[srclobename];
	c2eLobe *destlobe = b->lobes[destlobename];

	std::vector<c2eNeuron *> src_neurons, dest_neurons;

	for (unsigned int i = g->srclobe_lowerbound; i <= g->srclobe_upperbound; i++) {
		if (i >= srclobe->getNoNeurons()) break;
		src_neurons.push_back(srclobe->getNeuron(i));
	}

	for (unsigned int i = g->destlobe_lowerbound; i <= g->destlobe_upperbound; i++) {
		if (i >= destlobe->getNoNeurons()) break;
		dest_neurons.push_back(destlobe->getNeuron(i));
	}

	if (src_neurons.size() == 0 || dest_neurons.size() == 0) {
		std::cout << "brain debug: failed to create dendrites for " << dump() << " (no neurons)" << std::endl;
		return;
	}
	
	// create/distribute dendrites as needed
	if (g->migrates) {
		// You can't have *both* sides of the tract unconstrained, we'd have no idea how many dendrites to make!
		if (g->src_noconnections == 0 && g->dest_noconnections == 0) {
			std::cout << "brain debug: failed to create dendrites for " << dump() << " (both connections unconstrained)" << std::endl;
		} else if (g->src_noconnections != 0 && g->dest_noconnections != 0) {
			// TODO: correct behaviour? seems to be, given CL's brain-in-a-vat behaviour
			std::cout << "brain debug: failed to create dendrites for " << dump() << " (no unconstrained connections)" << std::endl;
			return;
		}

		// assume we're doing src->dest
		unsigned int neuronsize = src_neurons.size();
		unsigned int noconnections = g->src_noconnections;
		// change things if we're doing dest->src :)
		if (g->src_noconnections == 0) {
			neuronsize = dest_neurons.size();
			noconnections = g->dest_noconnections;
		}

		// distribute neurons
		// TODO: work out if this algorithm works vaguely correctly
		// TODO: low-order bit badness in the randomness?
		for (unsigned int i = 0; i < neuronsize; i++) {
			unsigned int noconns = noconnections;
			if (g->norandomconnections)
				noconns = 1 + (rand() % noconnections);

			for (unsigned int j = 0; j < noconns; j++) {
				c2eDendrite d;
				if (g->src_noconnections == 0) {
					d.source = src_neurons[rand() % src_neurons.size()];
					d.dest = dest_neurons[i];
				} else {
					d.source = src_neurons[i];
					d.dest = dest_neurons[rand() % dest_neurons.size()];
				}
				dendrites.push_back(d);
			}
		}
	} else {
		// if the genome tells us to make no connections, give up
		if (g->src_noconnections == 0 || g->dest_noconnections == 0) {
			std::cout << "brain debug: failed to create dendrites for " << dump() << " (no connections)" << std::endl;
			return;
		}
	
		// distribute neurons
		// this seems identical to CL's brain-in-a-vat for the default brain and for some test cases fuzzie made up
		// TODO: test the algorithm a bit more
		// TODO: take notice of norandomconnections? (doesn't look like it)
		unsigned int srcneuron = 0, srcconns = 0;
		unsigned int destneuron = 0, destconns = 0;
		while (true) {
			c2eNeuron *src = src_neurons[srcneuron];
			c2eNeuron *dest = dest_neurons[destneuron];

			// if there's already a dendrite like the one we're about to create, we're done
			if (getDendriteFromTo(src, dest)) return;
			
			c2eDendrite d;
			d.source = src;
			d.dest = dest;
			dendrites.push_back(d);

			srcconns++;
			if (srcconns >= g->src_noconnections) {
				srcconns = 0;
				destneuron++;
				if (destneuron >= dest_neurons.size())
					destneuron = 0;
			}
			destconns++;
			if (destconns >= g->dest_noconnections) {
				destconns = 0;
				srcneuron++;
				if (srcneuron >= src_neurons.size())
					srcneuron = 0;
			}
		}
	}
}

/*
 * c2eTract::dump
 *
 * Returns a textual string describing the tract for use in debug messages.
 *
 */
std::string c2eTract::dump() {
	c2eBrainTractGene *g = ourGene;

	std::string srclobename = std::string((char *)g->srclobe, 4);
	std::string destlobename = std::string((char *)g->destlobe, 4);

	std::string data = boost::str(boost::format("tract %s->%s, src neurons %d-%d #cons %d, dest neurons %d-%d #cons %d") % srclobename % destlobename
		% (int)g->srclobe_lowerbound % (int)g->srclobe_upperbound % (int)g->src_noconnections
		% (int)g->destlobe_lowerbound % (int)g->destlobe_upperbound % (int)g->dest_noconnections
		);

	if (g->migrates) data += ", migratory";

	return data;
}

/*
 * c2eTract::getDendriteFromTo
 *
 * Returns the dendrite from this tract between the two neurons, or null if there isn't one.
 *
 */
c2eDendrite *c2eTract::getDendriteFromTo(c2eNeuron *from, c2eNeuron *to) {
	for (std::vector<c2eDendrite>::iterator i = dendrites.begin(); i != dendrites.end(); i++) {
		if (i->source == from && i->dest == to) return &(*i);
	}
	
	return 0;
}

/*
 * c2eTract::tick
 *
 * Do a single update of the tract.
 *
 */
void c2eTract::tick() {
	// attempt to migrate dendrites, if enabled
	if (ourGene->migrates)
		doMigration();

	// work out which svrule to use for updating
	c2eSVRule &rule = updaterule;
	if (ourGene->initrulealways)
		rule = initrule;

	// run that svrule against every dendrite
	for (std::vector<c2eDendrite>::iterator i = dendrites.begin(); i != dendrites.end(); i++) {
		rule.runRule(i->source->variables[0], i->source->variables, i->dest->variables, dummyValues, i->variables, parent->getParent());
	}

	// TODO: reward/punishment? anything else? scary brains!
}

void c2eTract::init() {
	for (std::vector<c2eDendrite>::iterator i = dendrites.begin(); i != dendrites.end(); i++) {
		for (unsigned int j = 0; j < 8; j++)
			i->variables[j] = 0.0f;
		// TODO: will i->source/i->dest data always be initialised here? i doubt it.
		// TODO: good way to run rule?
		initrule.runRule(i->source->variables[0], i->source->variables, i->dest->variables, dummyValues, i->variables, parent->getParent());
	}
}

void c2eTract::doMigration() {
	// TODO
}

/*
 * c2eLobe::c2eLobe
 *
 * Constructor for a c2eLobe. Pass it the relevant gene.
 *
 */
c2eLobe::c2eLobe(c2eBrain *b, c2eBrainLobeGene *g) : c2eBrainComponent(b) {
	assert(g);
	ourGene = g;
	updatetime = g->updatetime;

	spare = 0;

	unsigned int width = g->width, height = g->height;
	if (width < 1) width = 1;
	if (height < 1) height = 1;

	neurons.reserve(width * height);

	c2eNeuron n;
	for (unsigned int i = 0; i < width * height; i++) {
		neurons.push_back(n);
	}

	initrule.init(g->initialiserule);
	updaterule.init(g->updaterule);
}

/*
 *
 * c2eLobe::tick
 *
 * Do a single update of the lobe.
 *
 */
void c2eLobe::tick() {
	// work out which svrule to use for updating
	c2eSVRule &rule = updaterule;
	if (ourGene->initrulealways)
		rule = initrule;

	// run that svrule against every neuron
	for (unsigned int i = 0; i < neurons.size(); i++) {
		if (rule.runRule(neurons[i].input, dummyValues, neurons[i].variables, neurons[spare].variables, dummyValues, parent->getParent()))
			spare = i;
		neurons[i].input = 0.0f;
	}
}

void c2eLobe::init() {
	for (std::vector<c2eNeuron>::iterator i = neurons.begin(); i != neurons.end(); i++) {
		for (unsigned int j = 0; j < 8; j++)
			i->variables[j] = 0.0f;
		// TODO: good way to run rule?
		initrule.runRule(0.0f, dummyValues, i->variables, dummyValues, dummyValues, parent->getParent());
		i->input = 0.0f; // TODO: good to do that here?
	}
}

void c2eLobe::setNeuronInput(unsigned int i, float input) {
	assert(i < neurons.size());
	neurons[i].input = input; // TODO: always stomp over any existing input?
}

std::string c2eLobe::getId() {
	return std::string((char *)ourGene->id, 4);
}

/*
 * c2eSVRule::init
 *
 * Initialises an SVRule from the provided data, precalculating any constant data.
 *
 */
void c2eSVRule::init(uint8 ruledata[48]) {
	rules.reserve(16);

	for (unsigned int i = 0; i < 16; i++) {
		c2erule rule;

		rule.opcode = ruledata[i * 3];
		rule.operandtype = ruledata[(i * 3) + 1];
		rule.operanddata = ruledata[(i * 3) + 2];

		switch (rule.operandtype) {
			// for neuron/dendrite values, sanitise value (there are only 8 options)

			case 1: // input neuron
			case 2: // dendrite
			case 3: // neuron
			case 4: // spare neuron
				// TODO: what should we do here?
				if (rule.operanddata > 7) rule.operanddata = 7;
				break;

			// for constant values, precalculate data
			
			case 9: // zero
				rule.operandvalue = 0.0f;
				break;

			case 10: // one
				rule.operandvalue = 1.0f;
				break;

			case 11: // value
				rule.operandvalue = (float)rule.operanddata * (1.0f / 248);
				break;

			case 12: // negative value
				rule.operandvalue = (float)rule.operanddata * (-1.0f / 248);
				break;

			case 13: // value * 10
				rule.operandvalue = (float)rule.operanddata * (10.0f / 248);
				break;

			case 14: // value / 10
				rule.operandvalue = (float)rule.operanddata * (0.1f / 248);
				break;

			case 15: // value integer
				rule.operandvalue = (float)rule.operanddata;
				break;
		}

		rules.push_back(rule);
	}
}

// convenience function for c2eSVRule::runRule
inline float bindFloatValue(float val, float min = -1.0f, float max = 1.0f) {
	if (val > max) return max;
	else if (val < min) return min;
	else return val;
}

/*
 * c2eSVRule::runRule
 *
 * Executes the SVRule using the provided variables.
 *
 * Returns whether the 'register as spare' opcode was executed or not.
 *
 */
bool c2eSVRule::runRule(float acc, float srcneuron[8], float neuron[8], float spareneuron[8], float dendrite[8], c2eCreature *creature) {
	float accumulator = acc;
	float operandvalue = 0.0f; // valid rules should never use this
	float tendrate = 0.0f;
	float *operandpointer;
	float dummy;
	bool is_spare = false;
	bool skip_next = false;

	for (unsigned int i = 0; i < rules.size(); i++) {
		c2erule &rule = rules[i];

		if (skip_next) { // if the last if opcode was *false*..
			// .. then don't execute the next line
			skip_next = false;
			continue;
		}

		// We don't always *need* the operand and/or pointer, but for now we'll always calculate it anyway.
		operandpointer = &dummy; // point into nowhere, by default.. TODO: good choice?
		switch (rule.operandtype) {
			case 0: // accumulator
				operandvalue = accumulator;
				// accumulator does *not* set operandpointer
				// (eg, 'blank accumulator' and 'add to and store in accumulator' do not change it)
				break;

			case 1: // input neuron
				operandpointer = &srcneuron[rule.operanddata];
				operandvalue = *operandpointer;
				break;

			case 2: // dendrite
				operandpointer = &dendrite[rule.operanddata];
				operandvalue = *operandpointer;
				break;

			case 3: // neuron
				operandpointer = &neuron[rule.operanddata];
				operandvalue = *operandpointer;
				break;

			case 4: // spare neuron
				operandpointer = &spareneuron[rule.operanddata];
				operandvalue = *operandpointer;
				break;

			case 5: // random
				// TODO: find a quicker RNG?
				// TODO: untested
				operandvalue = rand() / (float)RAND_MAX;
				break;

			case 6: // source chemical
				// TODO: unused?
				std::cout << "brain debug: something tried using source chemical!" << std::endl;
				break;

			case 7: // chemical
				// Ratboy sez: "chemicals appear to be read-only; cannot write data to them"
				operandvalue = creature->getChemical(rule.operanddata);
				break;

			case 8: // destination chemical
				// TODO: unused?
				std::cout << "brain debug: something tried using dest chemical!" << std::endl;
				break;

			case 9: // zero
			case 10: // one
			case 11: // value
			case 12: // negative value
			case 13: // value * 10
			case 14: // value / 10
			case 15: // value integer
				// precalculated constants
				operandvalue = rule.operandvalue;
				break;

			default:
				// TODO: what to do?
				std::cout << "brain debug: something tried using unknown operand type " << (int)rule.operandtype << std::endl;
				break;
		}

		switch (rule.opcode) {
			case 0: // stop
				goto done;

			case 1: // blank
				*operandpointer = 0.0f;
				break;

			case 2: // store in
				*operandpointer = bindFloatValue(accumulator);
				break;

			case 3: // load from
				accumulator = operandvalue;
				break;

			case 4: // if =
				if (!(accumulator == operandvalue))
					skip_next = true;
				break;

			case 5: // if <>
				if (!(accumulator != operandvalue))
					skip_next = true;
				break;

			case 6: // if >
				if (!(accumulator > operandvalue))
					skip_next = true;
				break;

			case 7: // if <
				if (!(accumulator < operandvalue))
					skip_next = true;
				break;

			case 8: // if >=
				if (!(accumulator >= operandvalue))
					skip_next = true;
				break;

			case 9: // if <=
				if (!(accumulator <= operandvalue))
					skip_next = true;
				break;

			case 10: // if zero
				if (!(accumulator == 0.0f))
					skip_next = true;
				break;

			case 11: // if non-zero
				if (!(accumulator != 0.0f))
					skip_next = true;
				break;

			case 12: // if positive
				if (!(accumulator > 0.0f)) // TODO: correct?
					skip_next = true;
				break;

			case 13: // if negative
				if (!(accumulator < 0.0f)) // TODO: correct?
					skip_next = true;
				break;

			case 14: // if non-positive
				if (!(accumulator <= 0.0f)) // TODO: correct?
					skip_next = true;
				break;

			case 15: // if non-negative
				if (!(accumulator >= 0.0f)) // TODO: correct?
					skip_next = true;
				break;

			case 16: // add
				accumulator += operandvalue;
				break;

			case 17: // subtract
				accumulator -= operandvalue;
				break;

			case 18: // subtract from
				accumulator = operandvalue - accumulator;
				break;

			case 19: // multiply by
				accumulator *= operandvalue;
				break;

			case 20: // divide by
				// TODO: make sure this is correct
				accumulator /= operandvalue;
				break;

			case 21: // divide into
				// TODO: make sure this is correct
				accumulator = operandvalue / accumulator;
				break;

			case 22: // minimum with
				accumulator = std::min(accumulator, operandvalue);
				break;

			case 23: // maximum with
				accumulator = std::max(accumulator, operandvalue);
				break;

			case 24: // set tend rate
				tendrate = operandvalue;
				break;

			case 25: // tend to
				// TODO: make sure this is correct
				accumulator += tendrate * (operandvalue - accumulator);
				break;

			case 26: // load negation of
				accumulator = -operandvalue;
				break;
			
			case 27: // load abs of
				accumulator = fabsf(operandvalue);
				break;

			case 28: // distance to
				// TODO: make sure this is correct
				accumulator = fabsf(accumulator - operandvalue);
				break;

			case 29: // flip around
				// TODO: make sure this is correct
				accumulator = operandvalue - accumulator;
				break;

			case 30: // no operation
				break;

			case 31: // register as spare
				is_spare = true;
				break;

			case 32: // bound in range [0, 1]
				// TODO: make sure this is correct
				accumulator = bindFloatValue(operandvalue, 0.0f);
				break;

			case 33: // bound in range [-1, 1]
				// TODO: make sure this is correct
				accumulator = bindFloatValue(operandvalue);
				break;

			case 34: // add and store in
				*operandpointer = bindFloatValue(accumulator + operandvalue);
				break;

			case 35: // tend to and store in
				// TODO
				break;

			case 36: // nominal threshold
				// TODO: make sure this is correct
				if (accumulator < operandvalue)
					accumulator = 0.0f;
				break;

			case 37: // leakage rate
				// TODO
				break;
			
			case 38: // rest state
				// TODO
				break;

			case 39: // input gain hi-lo
				// TODO
				break;

			case 40: // persistence
				// TODO
				break;

			case 41: // signal noise
				// TODO
				break;

			case 42: // winner takes all
				// TODO
				break;

			case 43: // short-term relax rate
				// TODO
				break;

			case 44: // long-term relax rate
				// TODO
				break;

			case 45: // store abs in
				// TODO
				break;

			case 46: // stop if zero
				if (accumulator == 0.0f) goto done;
				break;

			case 47: // stop if non-zero
				if (accumulator != 0.0f) goto done;
				break;

			case 48: // if zero goto
				// TODO
				break;

			case 49: // if non-zero goto
				// TODO
				break;

			case 50: // divide by, add to neuron input
				// TODO
				break;

			case 51: // multiply by, add to neuron input
				// TODO
				break;

			case 52: // goto line
				// TODO
				break;

			case 53: // stop if <
				if (accumulator < operandvalue) goto done;
				break;

			case 54: // stop if >
				if (accumulator > operandvalue) goto done;
				break;

			case 55: // stop if <=
				if (accumulator <= operandvalue) goto done;
				break;

			case 56: // stop if >=
				if (accumulator >= operandvalue) goto done;
				break;

			case 57: // reward threshold
				// TODO
				break;

			case 58: // reward rate
				// TODO
				break;

			case 59: // use reward with
				// TODO
				break;

			case 60: // punish threshold
				// TODO
				break;

			case 61: // punish rate
				// TODO
				break;

			case 62: // use punish with
				// TODO
				break;

			case 63: // preserve neuron SV
				// TODO
				break;

			case 64: // restore neuron SV
				// TODO
				break;

			case 65: // preserve spare neuron
				// TODO
				break;

			case 66: // restore spare neuron
				// TODO
				break;

			case 67: // if negative goto
				// TODO
				break;

			case 68: // if positive goto
				// TODO
				break;

			default:
				// do nothing!
				break;
		}
	}

done:
	return is_spare;
}

/*
 * c2eBrain::c2eBrain
 *
 * Constructor for a c2eBrain. Pass it the creature it belongs to, and it will construct itself.
 *
 */
c2eBrain::c2eBrain(c2eCreature *p) {
	assert(p);
	parent = p;

	shared_ptr<genomeFile> genome = p->getGenome();
	
	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if ((*i)->header.flags.femaleonly && !p->isFemale()) continue;
		if ((*i)->header.flags.maleonly && p->isFemale()) continue;
		// TODO: lifestage
		if (typeid(**i) == typeid(c2eBrainLobeGene)) {
			c2eLobe *l = new c2eLobe(this, (c2eBrainLobeGene *)*i);
			components.insert(l);
			lobes[l->getId()] = l;
		}
	}

	for (vector<gene *>::iterator i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if ((*i)->header.flags.femaleonly && !p->isFemale()) continue;
		if ((*i)->header.flags.maleonly && p->isFemale()) continue;
		// TODO: lifestage
		if (typeid(**i) == typeid(c2eBrainTractGene)) {
			c2eTract *t = new c2eTract(this, (c2eBrainTractGene *)*i);
			components.insert(t);
			tracts.push_back(t);
		}
	}
}

void c2eBrain::init() {
	for (std::multiset<c2eBrainComponent *, c2ebraincomponentorder>::iterator i = components.begin(); i != components.end(); i++) {
		(*i)->init();
	}
}

void c2eBrain::tick() {
	for (std::multiset<c2eBrainComponent *, c2ebraincomponentorder>::iterator i = components.begin(); i != components.end(); i++) {
		// TODO: good check for this?
		if ((*i)->getUpdateTime() != 0)
			(*i)->tick();
	}
}

c2eLobe *c2eBrain::getLobeByTissue(unsigned int id) {
	for (std::map<std::string, c2eLobe *>::iterator i = lobes.begin(); i != lobes.end(); i++) {
		if (i->second->getGene()->tissue == id)
			return i->second;
	}

	return 0;
}

c2eLobe *c2eBrain::getLobeById(std::string id) {
	std::map<std::string, c2eLobe *>::iterator i = lobes.find(id);

	if (i != lobes.end())
		return i->second;
	else
		return 0;
}

/* vim: set noet: */
