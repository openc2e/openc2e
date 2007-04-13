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
#include <math.h>

float dummyValues[8];

c2eTract::c2eTract(c2eBrainTractGene *g) {
	assert(g);
	ourGene = g;

	// TODO: find source/dest lobes, calculate neurons
	// TODO: create/distribute dendrites as needed
	// TODO: run init rule
}

/*
 * c2eTract::getDendriteFromTo
 *
 * Returns the dendrite from this tract between the two neurons, or null if there isn't one.
 *
 */
c2eDendrite *c2eTract::getDendriteFromTo(c2eNeuron *from, c2eNeuron *to) {
	// TODO
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
		// TODO: last dummyValues should be biochemistry
		rule.runRule(i->source->variables[0], i->source->variables, i->dest->variables, dummyValues, i->variables, dummyValues);
	}

	// TODO: reward/punishment? anything else? scary brains!
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
		// TODO: last dummyValues should be biochemistry
		if (rule.runRule(neurons[i].input, dummyValues, neurons[i].variables, neurons[spare].variables, dummyValues, dummyValues))
			spare = i;
		neurons[i].input = 0.0f;
	}
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

		switch (rule.operanddata) {
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

/*
 * c2eSVRule::runRule
 *
 * Executes the SVRule using the provided variables.
 *
 * Returns whether the 'register as spare' opcode was executed or not.
 *
 */
bool c2eSVRule::runRule(float acc, float srcneuron[8], float neuron[8], float spareneuron[8], float dendrite[8], float chemicals[256]) {
	float accumulator = acc;
	float operandvalue;
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
				// TODO: does this set operandpointer?
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
				break;

			case 7: // chemical
				// Ratboy sez: "chemicals appear to be read-only; cannot write data to them"
				operandvalue = chemicals[rule.operanddata];
				break;

			case 8: // destination chemical
				// TODO: unused?
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
		}

		switch (rule.opcode) {
			case 0: // stop
				goto done;

			case 1: // blank
				*operandpointer = 0.0f;
				// TODO: ratboy sez: "Blanking the accumulator seems to have no effect (accumulator value does not change), nor can chemical tokens be blanked."
				// TODO: however, stim lobe in standard genome blanks the accumulator..
				break;

			case 2: // store in
				*operandpointer = accumulator;
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
				// TODO
				//tendrate = operandvalue;
				break;

			case 25: // tend to
				// TODO
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
				accumulator = operandvalue;
				if (accumulator < 0.0f) accumulator = 0.0f;
				else if (accumulator > 1.0f) accumulator = 1.0f;
				break;

			case 33: // bound in range [-1, 1]
				// TODO: make sure this is correct
				accumulator = operandvalue;
				if (accumulator < -1.0f) accumulator = -1.0f;
				else if (accumulator > 1.0f) accumulator = 1.0f;
				break;

			case 34: // add and store in
				*operandpointer = accumulator + operandvalue;
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

/* vim: set noet: */
