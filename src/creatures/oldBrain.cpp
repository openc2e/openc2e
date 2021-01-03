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
#include "oldCreature.h"
#include <cassert>
#include <memory>
#include <fmt/core.h>

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

void oldSVRule::init(uint8_t version, uint8_t *src) {
	length = (version == 0) ? 8 : 12;
	for (unsigned int i = 0; i < length; i++) {
		rules[i] = src[i];
		if (version == 0 && rules[i] > 21) // if rule is above the strength id
			rules[i] += 8; // then skip the 8 new C2 svrules (to map to C2 svrules)
	}
	rndconst = 0; // TODO: correct?
}

unsigned char oldLobe::evaluateSVRuleConstant(oldNeuron *cell, oldDendrite *dend, uint8_t id, oldSVRule &rule) {
	switch (id) {
		/*
		 * these numbers are the C2 svrules (see rewrite in oldSVRule constructor)
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
			return dendrite_sum(*cell, 0, false);

		case 13: // type1
			return dendrite_sum(*cell, 1, false);

		case 14: // anded0
			return dendrite_sum(*cell, 0, true);

		case 15: // anded1
			// unused?
			return dendrite_sum(*cell, 1, true);

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
			return rule.rndconst;

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

unsigned char oldLobe::processSVRule(oldNeuron *cell, oldDendrite *dend, oldSVRule &rule) {
	unsigned char state = 0;

	// original engine seems to simply happily walk off the end of the svrule array for constants!
	// so our behaviour for the 'if (i == len)' lines is NOT the same

	for (unsigned int i = 0; i < rule.length; i++) {
		switch (rule.rules[i]) {
			case 0: // <end>
				return state;

			default:
				state = evaluateSVRuleConstant(cell, dend, rule.rules[i], rule);
				break;

			case 30: // TRUE
				if (!state) return 0;
				break;

			case 31: // PLUS
				i++;
				if (i == rule.length) return state;
				state = state + evaluateSVRuleConstant(cell, dend, rule.rules[i], rule);
				break;

			case 32: // MINUS
				i++;
				if (i == rule.length) return state;
				state = state - evaluateSVRuleConstant(cell, dend, rule.rules[i], rule);
				break;

			case 33: // TIMES
				// unused?
				i++;
				if (i == rule.length) return state;
				state = (state * evaluateSVRuleConstant(cell, dend, rule.rules[i], rule)) / 256;
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
				if (i == rule.length) return state;
				state = state * evaluateSVRuleConstant(cell, dend, rule.rules[i], rule);
				break;

			case 38: // average
				// unused?
				i++;
				if (i == rule.length) return state;
				state = (state + evaluateSVRuleConstant(cell, dend, rule.rules[i], rule)) / 2;
				break;

			case 39: { // move twrds
				i++;
				if (i == rule.length) return state;
				unsigned char towards = evaluateSVRuleConstant(cell, dend, rule.rules[i], rule);
				i++;
				if (i == rule.length) return state;
				unsigned char multiplier = evaluateSVRuleConstant(cell, dend, rule.rules[i], rule);
				state = ((towards - state) * multiplier) / 256;
				} break;

			case 40: { // random
				i++;
				if (i == rule.length) return state;
				unsigned char min = evaluateSVRuleConstant(cell, dend, rule.rules[i], rule);
				i++;
				if (i == rule.length) return state;
				unsigned char max = evaluateSVRuleConstant(cell, dend, rule.rules[i], rule);
				state = (rand() % (max - min + 1)) + min;
				} break;
		}
	}

	return state;
}

unsigned char oldLobe::dendrite_sum(oldNeuron &neu, unsigned int type, bool only_if_firing) {
	// TODO: cache this result, since it will be used every time the svrule runs
	// (and remember you can calculate both only_if_firing and not only_if_firing in one go!)
	bool all_firing = true;
	unsigned int sum = 0; // sum((src output * strength) / 255)
	for (unsigned int j = 0; j < neu.dendrites[type].size(); j++) {
		if (neu.dendrites[type][j].src->output == 0) all_firing = false;
		else sum += (neu.dendrites[type][j].src->output * neu.dendrites[type][j].strength) / 255;
	}
	if (!neu.dendrites[type].size()) sum = 255;

	if (only_if_firing && !all_firing) return 0;
	return (sum * inputgain) / 255;
}

// TODO: precalculate this if we end up using it much?
unsigned int oldLobe::getDendriteCount() {
	unsigned int count = 0;
	for (unsigned int i = 0; i < neurons.size(); i++) {
		count += neurons[i].dendrites[0].size();
		count += neurons[i].dendrites[1].size();
	}
	return count;
}

oldLobe::oldLobe(oldBrain *b, oldBrainLobeGene *g) {
	assert(b);
	parent = b;
	assert(g);
	ourGene = g;

	inited = false;

	staterule.init(g->version(), (uint8_t *)g->staterule);

	for (unsigned int i = 0; i < 2; i++) {
		oldDendriteInfo *dend_info = &g->dendrite1;
		if (i == 1) dend_info = &g->dendrite2;

		strgainrule[i].init(g->version(), (uint8_t *)dend_info->strgainrule);
		strlossrule[i].init(g->version(), (uint8_t *)dend_info->strlossrule);
		susceptrule[i].init(g->version(), (uint8_t *)dend_info->susceptrule);
		relaxrule[i].init(g->version(), (uint8_t *)dend_info->relaxrule);

		if (g->version() == 1) { // back/forward propogation is C2 only
			backproprule[i].init(g->version(), (uint8_t *)dend_info->backproprule);
			forproprule[i].init(g->version(), (uint8_t *)dend_info->forproprule);
		} else {
			backproprule[i].length = 0;
			forproprule[i].length = 0;
		}
	}

	threshold = g->nominalthreshold;
	leakagerate = g->leakagerate;
	inputgain = g->inputgain;

	width = g->width;
	if (width < 1) width = 1;
	height = g->height;
	if (height < 1) height = 1;

	// TODO

	for (unsigned int i = 0; i < 6; i++) {
		chems[i] = 0;
	}

	// TODO: good starting values?
	loose_neuron_upperbound[0] = 9999;
	loose_neuron_upperbound[1] = 9999;
}

void oldLobe::ensure_minimum_size(unsigned int size) {
	assert(!inited);

	if (!height) height = 3;

	while ((width * height) < size)
		width += 1;

	// sanity check
	if (width * height > 1024) { width = 256; height = 256; }
}

void oldLobe::init() {
	neurons.reserve(width * height);

	oldNeuron n;
	for (unsigned int i = 0; i < width * height; i++) {
		neurons.push_back(n);
	}

	wipe();

	// TODO: when reading from gene, we should enforce max >= min
	oldDendriteInfo *dend_info[2] = { &ourGene->dendrite1, &ourGene->dendrite2 };

	for (unsigned int i = 0; i < neurons.size(); i++) {
		for (unsigned int type = 0; type < 2; type++) {
			int our_min = dend_info[type]->min;
			int our_range = dend_info[type]->max - our_min;

			int value = 0;
			switch (dend_info[type]->spread) {
				case 0: // flat
					value = rand() % (our_range + 1);
					break;
				case 1: // normal
					// TODO: really not sure how this is different from above
					value = rand() % (our_range + 1);
					break;
				case 2: // Saw
					value = rand() % (our_range + 1);
					value *= 2;
					value = abs(value - our_range);
					break;
				case 3: // waS
					value = rand() % (our_range + 1);
					value *= 2;
					value = abs(value - our_range);
					value = our_range - value; // invert
					break;
			}
			value += our_min;
			for (unsigned int j = 0; j < (unsigned int)value; j++) {
				neurons[i].dendrites[type].push_back(oldDendrite());
			}
		}
	}

	// (don't set inited here, wait until connect() call)
}

void oldLobe::connect() {
	oldDendriteInfo *dend_info[2] = { &ourGene->dendrite1, &ourGene->dendrite2 };

	for (unsigned int type = 0; type < 2; type++) {
		oldLobe *src = parent->getLobeByTissue(dend_info[type]->srclobe);
		// TODO: fix srclobe for all dendrites to be within range (srclobe = srclobe % lobes.size())
		assert(src);

		// TODO: handle src->neurons.size() being empty? or rather, maybe should never let that happen

		uint8_t &fanout = dend_info[type]->fanout;

		unsigned int destsize = width * height, srcsize = src->width * src->height;

		unsigned int offset = 0;
		for (unsigned int i = 0; i < neurons.size(); i++) {
			oldNeuron &destneu = neurons[i];
			std::vector<oldDendrite> &dendrites = destneu.dendrites[type];
			if (dendrites.size() == 0) continue;

			unsigned int srcneu_id = offset / destsize;
			unsigned int divwidth = srcneu_id / src->width;
			unsigned int divwidth_r = srcneu_id % src->width;

			// connect src.neurons[srcneu] with destneu.dendrites[type][0]
			connectDendrite(type, dendrites[0], &src->neurons[srcneu_id]);

			for (unsigned int dend_id = 1; dend_id < dendrites.size(); dend_id++) {
				unsigned int attempts = 0;
repeat_this_dend:
				// for the other dendrites, find a source neuron to connect to using fanout
				int src_x = (rand() % ((2 * fanout) + 1)) + divwidth_r - fanout;
				int src_y = (rand() % ((2 * fanout) + 1)) + divwidth_r + divwidth;
				src_x = src_x % src->width;
				if (src_x < 0) src_x += src->width;
				src_y = src_y % src->height;
				if (src_y < 0) src_y += src->height;
				unsigned int src_neuid = (src_y * src->width) + src_x;

				// don't loop forever trying the impossible
				if (attempts >= dendrites.size() * 2) {
					// pick an appropriate src neuron methodically instead
					for (unsigned int n = 0; n < src->neurons.size(); n++) {
						unsigned int d;
						for (d = 0; d < dend_id; d++) {
							if (dendrites[d].src != &src->neurons[n] &&
								dendrites[d].src->percept_src != src->neurons[n].percept_src) {
								src_neuid = n;
								break;
							}
						}
						if (d < dend_id) break; // continue from above break
					}
					// (if we didn't find anything acceptable, fine, go on and connect to src_neuid..)
				}

				oldNeuron &srcneu = src->neurons[src_neuid];

				// percept_src is set on neurons in the perception lobe which are copied
				// from 'mutually exclusive' lobes, so we may only connect *one* dendrite
				// to each source neuron of a given percept_src
				if (attempts < dendrites.size() * 2 && srcneu.percept_src) {
					for (unsigned int d = 0; d < dend_id; d++) {
						if (dendrites[d].src != &srcneu &&
							dendrites[d].src->percept_src != srcneu.percept_src) {
							continue;
						}
						attempts++;
						goto repeat_this_dend;
					}
				}

				// connect srcneu with dendrites[type][dend_id]
				connectDendrite(type, dendrites[dend_id], &srcneu);
			}
			offset += srcsize;
		}
	}

	inited = true;
}

void oldLobe::connectDendrite(unsigned int type, oldDendrite &dend, oldNeuron *dest) {
	oldDendriteInfo *dend_info[2] = { &ourGene->dendrite1, &ourGene->dendrite2 };

	dend.src = dest;
	dend.suscept = 0;
	dend.stw = dend.ltw = rand() % (dend_info[type]->maxLTW - dend_info[type]->minLTW + 1) + dend_info[type]->minLTW;
	dend.strength = rand() % (dend_info[type]->maxstr - dend_info[type]->minstr + 1) + dend_info[type]->minstr;
}

void oldLobe::wipe() {
	for (unsigned int i = 0; i < neurons.size(); i++) {
		neurons[i].state = neurons[i].output = ourGene->reststate; // TODO: good?
	}
}

void oldLobe::tick() {
	if (ourGene->dendrite1.migrateflag == 1) loose_dendrites[0] = 255;
	else loose_dendrites[0] = 0;
	if (ourGene->dendrite2.migrateflag == 1) loose_dendrites[1] = 255;
	else loose_dendrites[1] = 0;
	active_neurons.clear();

	last_loose_neuron[0] = 0xFFFFFFFF;
	last_loose_neuron[1] = 0xFFFFFFFF;

	for (unsigned int i = 0; i < neurons.size(); i++) {
		unsigned char out = processSVRule(&neurons[i], NULL, staterule);

		// apply leakage rate in order to settle at rest state
		if ((parent->getTicks() & parent->getParent()->calculateTickMask(leakagerate / 8)) == 0) {
			if (out > ourGene->reststate)
				out = ourGene->reststate + ((out - ourGene->reststate) * parent->getParent()->calculateMultiplier(leakagerate / 8)) / 65536;
			else
				out = ourGene->reststate;
		}

		neurons[i].state = out;

		if (out < threshold) {
			out = 0;
		} else {
			active_neurons.push_back(i);
			out -= threshold;
		}

		neurons[i].output = out;

		tickDendrites(i, 0);
		tickDendrites(i, 1);

		neurons[i].leakin = 0;
	}
	lobe_activity = (unsigned char)((active_neurons.size() * 255) / neurons.size());

	// TODO: data copied to perception lobe (ourGene->perceptflag - not just true/false!)

	if (ourGene->flags & 1) {
		// winner takes all
		// TODO: some kind of magic ignoring for attn lobe?
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

	oldDendriteInfo *dend_info[2] = { &ourGene->dendrite1, &ourGene->dendrite2 };
	for (unsigned int type = 0; type < 2; type++) {
		if (dend_info[type]->migrateflag == 1) {
			oldLobe *src = parent->getLobeByTissue(dend_info[type]->srclobe);
			// TODO: see fix srclobe comment above (in oldLobe::connect)
			assert(src);

			for (unsigned int i = 0; i < neurons.size(); i++) {
				if (!neurons[i].output) continue;

				neuronTryMigration(type, neurons[i], src);
			}
		}
	}

	for (unsigned int type = 0; type < 2; type++) {
		if (dend_info[type]->migrateflag == 2) {
			if (last_loose_neuron[type] == 0xFFFFFFFF) {
				loose_neuron_upperbound[type] = 9999;
				continue;
			}
			assert(last_loose_neuron[type] < neurons.size());
			oldNeuron &neu = neurons[last_loose_neuron[type]];
			if (dend_info[1 - type]->migrateflag == 2) {
				// try (single) combined migration
				neuronTryAllLooseMigration(2, neu);
			} else {
				// try (single) migration for this type
				neuronTryAllLooseMigration(type, neu);
			}
			loose_neuron_upperbound[type] = last_loose_neuron[type];
		}
		dend_info[0] = &ourGene->dendrite2;
		dend_info[1] = &ourGene->dendrite1;

		// TODO: should we stop here if both migrateflags were 2?
	}
}

// helper function for neuronTryAllLooseMigration (below)
unsigned int oldNeuron::magicalHash(unsigned int type) {
	unsigned int type_limit = type;
	if (type == 2) { type = 0; type_limit--; }

	// this is just a silly hash of the connected source neurons, so we can check
	// if we're connected to the same sources as another neuron (type==2 for both)
	// TODO: make this less stupid
	unsigned int out = 1, outsum = 0;
	for (unsigned int t = type; t <= type_limit; t++) {
		for (unsigned int d = 0; d < dendrites[t].size(); d++) {
			// it is a haaaaash, so hopefully nothing will explode about the cast
			unsigned int v = (unsigned long)dendrites[t][d].src;
			outsum += v;
			out *= v;
		}
	}
	return (out & 0xFFFF) | (outsum << 16);
}

// helper function for neuronTryAllLooseMigration (below)
bool oldLobe::migrationTryCandidateSlice(unsigned int type, oldLobe *src, std::vector<oldDendrite> &dendrites, unsigned int i) {
	// we examine a 'slice' of candidate active neurons, one for each dendrite, starting at i
	for (unsigned int j = 0; j < dendrites.size(); j++) {
		// if the neuron is perceptible..
		if (src->neurons[src->active_neurons[i + j]].percept_src && j < dendrites.size() - 1) {
			// check all previous neurons to make sure they don't collide
			for (unsigned int k = i + j + 1; k < dendrites.size(); k++) {
				if (src->neurons[src->active_neurons[i + j]].percept_src == src->neurons[k].percept_src) {
					return false;
				}
			}
		}
	}

	// slice is an acceptable candidate, wire it up
	for (unsigned int j = 0; j < dendrites.size(); j++) {
		connectDendrite(type, dendrites[j], &src->neurons[i + j]);
	}

	return true;
}

/*
 * neuron migration which migrates *all* dendrites at once.
 *
 * dendrites only connect to a unique set of appropriate firing source neurons, so migration
 * only succeeds if there are sufficient such source neurons available (otherwise the
 * dendrites are reset to point back to where they were, ready for another migration attempt)
 *
 * pass type == 2 to migrate both types at once, otherwise it migrates the specified type only.
 */
void oldLobe::neuronTryAllLooseMigration(unsigned int type, oldNeuron &neu) {
	unsigned int original_type = type;
	unsigned int type_limit = type;
	if (type == 2) { type = 0; type_limit--; }

	oldDendriteInfo *dend_info[2] = { &ourGene->dendrite1, &ourGene->dendrite2 };

	oldLobe *src[2] = { NULL, NULL };
	for (unsigned int t = type; t <= type_limit; t++) {
		src[t] = parent->getLobeByTissue(dend_info[t]->srclobe);
		// TODO: see fix srclobe comment above (in oldLobe::connect)
		assert(src[t]);

		// not enough active source dendrites at present?
		if (src[t]->active_neurons.size() < neu.dendrites[t].size()) return;
	}

	// store old source neurons (in case we can't find a valid connection)
	std::vector<oldNeuron *> srcneus[2];
	for (unsigned int t = type; t <= type_limit; t++) {
		for (unsigned int i = 0; i < neu.dendrites[t].size(); i++) {
			srcneus[t].push_back(neu.dendrites[t][i].src);
		}
	}

	// shuffle source active neurons
	for (unsigned int t = type; t <= type_limit; t++) {
		for (unsigned int i = 0; i < src[t]->active_neurons[i]; i++) {
			unsigned int j = rand() % src[t]->active_neurons.size();
			unsigned int old = src[t]->active_neurons[j];
			src[t]->active_neurons[j] = src[t]->active_neurons[i];
			src[t]->active_neurons[i] = old;
		}
	}

	for (unsigned int i = 0; i <= src[type]->active_neurons.size() - neu.dendrites[type].size(); i++) {
		if (!migrationTryCandidateSlice(type, src[type], neu.dendrites[type], i)) continue;

		// repeat inner loop *once* for one type, or a bunch of times for two types
		unsigned int count = 0;
		if (original_type == 2) count = src[1]->active_neurons.size() - neu.dendrites[1].size();

		for (unsigned int k = 0; k <= count; k++) {
			if (original_type == 2)
				if (!migrationTryCandidateSlice(1, src[1], neu.dendrites[1], k)) continue;

			// are there any other active neurons attached to the same source neurons as us?
			unsigned int hash = neu.magicalHash(original_type);
			unsigned int t;
			for (t = type; t <= type_limit; t++) {
				unsigned int j;
				for (j = 0; j < active_neurons.size(); j++) {
					if (neurons[active_neurons[j]].dendrites[t].size() != neu.dendrites[t].size()) break;
					if (neurons[active_neurons[j]].magicalHash(original_type) == hash) break;
				}
				if (j == active_neurons.size()) break;
			}
			if (t == type_limit + 1) {
				// success!
				loose_dendrites[type]--;
				return;
			}
		}
	}

	// failed; reset all dendrites back
	for (unsigned int t = type; t <= type_limit; t++) {
		for (unsigned int i = 0; i < neu.dendrites[t].size(); i++) {
			neu.dendrites[t][i].src = srcneus[t][i];
			neu.dendrites[t][i].stw = 0;
			neu.dendrites[t][i].ltw = 0;
			neu.dendrites[t][i].strength = 0;
		}
	}
}

void oldLobe::neuronTryMigration(unsigned int type, oldNeuron &neu, oldLobe *src) {
	if (!src->active_neurons.size()) return;

	// find a dendrite with strength zero
	unsigned int d;
	for (d = 0; d < neu.dendrites[type].size(); d++) {
		if (!neu.dendrites[type][d].strength) break;
	}
	if (d == neu.dendrites[type].size()) return;
	oldDendrite &dend = neu.dendrites[type][d];

	// pick a random firing neuron in the source lobe
	unsigned int n = rand() % src->active_neurons.size();
	oldNeuron *srcneu = &src->neurons[src->active_neurons[n]];

	for (d = 0; d < neu.dendrites[type].size(); d++) {
		// give up if we're already connected to chosen source neuron
		if (neu.dendrites[type][d].src == srcneu) break;
	}
	if (d != neu.dendrites[type].size()) return;

	connectDendrite(type, dend, &neu);

	if (loose_dendrites[type]) loose_dendrites[type]--;
}

void oldLobe::tickDendrites(unsigned int id, unsigned int type) {
	oldDendriteInfo *dend_info = &ourGene->dendrite1;
	if (type == 1) dend_info = &ourGene->dendrite2;

	oldNeuron &dest = neurons[id];

	unsigned int loose_dends = 0;
	for (unsigned int i = 0; i < dest.dendrites[type].size(); i++) {
		unsigned char out;

		oldDendrite &dend = dest.dendrites[type][i];

		if (!dend.strength) loose_dends++;

		// recalculate suscept
		out = processSVRule(&dest, &dend, susceptrule[type]);
		if (out > dend.suscept) {
			dend.suscept = out;
		} else {
			// decay old suscept
			if ((parent->getTicks() & parent->getParent()->calculateTickMask(dend_info->relaxsuscept / 8)) == 0) {
				dend.suscept = (dend.suscept * parent->getParent()->calculateMultiplier(dend_info->relaxsuscept / 8)) / 65536;
			}
		}

		// recalculate reinforce (TODO: why do we call this relaxrule?) (TODO: don't run if suscept is zero?)
		out = processSVRule(&dest, &dend, relaxrule[type]);
		unsigned char x = ((int)dend.suscept * (int)out) / 255;
		if (x && x < dend.stw - dend.ltw)
			dend.stw = dend.ltw + x;

		// STW relax
		if ((parent->getTicks() & parent->getParent()->calculateTickMask(dend_info->relaxSTW / 8)) == 0) {
			out = dend.ltw + ((out - dend.ltw) * parent->getParent()->calculateMultiplier(dend_info->relaxSTW / 8)) / 65536;
		}

		// LTW gain
		if (dend_info->LTWgainrate && (parent->getTicks() % dend_info->LTWgainrate) == 0) {
			if (dend.ltw < dend.stw)
				dend.ltw++;
			else if (dend.ltw > dend.stw)
				dend.ltw--; // does this case really happen?
		}

		// strength gain
		if (dend.strength < 255 && dend_info->strgain && (parent->getTicks() % dend_info->strgain) == 0) {
			out = processSVRule(&dest, &dend, strgainrule[type]);
			if ((int)dend.strength + (int)out > 255) dend.strength = 255;
			else dend.strength += out;
		}

		// strength loss
		if (dend.strength && dend_info->strloss && (parent->getTicks() % dend_info->strloss) == 0) {
			out = processSVRule(&dest, &dend, strlossrule[type]);
			if ((int)dend.strength - (int)out < 0) dend.strength = 0;
			else dend.strength -= out;
			if (!dend.strength) {
				loose_dends++;
				// also reset STW, LTW, suscept, output/state on dest neuron
				dend.stw = 0;
				dend.ltw = 0;
				dend.suscept = 0;
				dest.output = 0;
				dest.state = 0;
			}
		}

		// back propogation (set leak in of src neuron)
		out = processSVRule(&dest, &dend, backproprule[type]);
		dend.src->leakin = out;

		// front propogation (set leak out of dest neuron)
		out = processSVRule(&dest, &dend, forproprule[type]);
		dest.leakout = out;
	}

	if (dend_info->migrateflag == 1) {
		// data useful for migrateflag == 1 (migrate if ANY loose)
		if (loose_dends < loose_dendrites[type])
			loose_dendrites[type] = (unsigned char)loose_dends;
	} else if (loose_dends) {
		// data useful for migrateflag == 2 (migrate if ALL loose)
		if (loose_dendrites[type] < 255)
			loose_dendrites[type]++;
		// this is used later by migration code, see there
		if (id < loose_neuron_upperbound[type])
			last_loose_neuron[type] = id;
	}
}

oldBrain::oldBrain(oldCreature *p) {
	assert(p);
	parent = p;

	ticks = 0;
}

// helper function for below
unsigned int minimumLobeSize(unsigned int version, unsigned int lobeid) {
	switch (lobeid) {
		case 1: return (version == 0 ? 15 : 17); // drive
		case 2: return 40; // stim source
		case 3: return 16; // verb
		case 4: return 40; // noun
		case 5: return 32; // general sensory
		case 6: return 16; // decision
		case 7: return 40; // attention
		default: return 0;
	}
}

void oldBrain::processGenes() {
	// TODO: this likely doesn't work at all well in the presence of later-turn-on lobes

	std::shared_ptr<genomeFile> genome = parent->getGenome();
	
	for (auto i = genome->genes.begin(); i != genome->genes.end(); i++) {
		if (!parent->shouldProcessGene(i->get())) continue;
		
		if (typeid(**i) == typeid(oldBrainLobeGene)) {
			oldBrainLobeGene *g = (oldBrainLobeGene *)i->get();
			oldLobe *l = new oldLobe(this, g);
			lobes.push_back(l);
		}
	}

	// TODO: this is a problem right now because oldLobe wants a copy of the genome objects
	/*while (lobes.size() < 8) {
		oldLobe *l = new oldLobe(this);
		l->ensure_minimum_size(minimumLobeSize(genome->getVersion(), lobes.size()));
		if (lobes.size() == 1) { } // TODO: force perceptible on drive lobe (lobe 1)
		lobes.push_back(l);
	}*/

	for (unsigned int i = 1; i < lobes.size(); i++) {
		if (!lobes[i]->wasInited())
			lobes[i]->ensure_minimum_size(minimumLobeSize(genome->getVersion(), i));
	}

	// ensure the perception lobe (lobe 0) is large enough for all perceptible neurons
	// (we should be able to guarantee lobes[0] exists once the above is fixed)
	unsigned int size = 0;
	for (unsigned int i = 1; i < lobes.size(); i++) {
		if (lobes[i]->getGene()->perceptflag)
			size += lobes[i]->getWidth() * lobes[i]->getHeight(); // can't use getNoNeurons before init()!
	}
	if (!lobes[0]->wasInited()) // TODO: we should really fix this even if it already got inited :-(
		lobes[0]->ensure_minimum_size(size);

	// we have to create the neurons here, so that they can be attached to by receptors/emitters
	for (unsigned int i = 0; i < lobes.size(); i++) {
		if (!lobes[i]->wasInited()) lobes[i]->init();
	}
}

void oldBrain::init() {
	for (unsigned int i = 0; i < lobes.size(); i++) {
		if (!lobes[i]->wasInited()) lobes[i]->connect();
	}

	// construct processing order list
	lobe_process_order.clear();
	for (unsigned int i = 0; i < lobes.size(); i++) {
		// first: lobes with no incoming dendrites
		if (lobes[i]->getDendriteCount() == 0)
			lobe_process_order.push_back(i);
	}
	for (unsigned int i = 0; i < lobes.size(); i++) {
		// then: lobes copied to perception lobe
		if (lobes[i]->getDendriteCount() == 0)
			continue;
		if (lobes[i]->getGene()->perceptflag)
			lobe_process_order.push_back(i);
	}
	for (unsigned int i = 0; i < lobes.size(); i++) {
		// then: every other lobe
		if (lobes[i]->getDendriteCount() == 0)
			continue;
		if (lobes[i]->getGene()->perceptflag)
			continue;
		lobe_process_order.push_back(i);
	}
	assert(lobe_process_order.size() == lobes.size());

	// precalculate mutually exclusive (percept src) data for neurons
	unsigned int offset = 0;
	for (unsigned int i = 1; i < lobes.size(); i++) {
		if (!lobes[i]->getGene()->perceptflag) continue;
		if (lobes[i]->getGene()->perceptflag == 2) { // mutually exclusive
			for (unsigned int j = 0; j < lobes[i]->getNoNeurons(); j++) {
				// (we should be able to guarantee lobes[0] exists
				// and is of sufficient size thanks to processGenes())
				lobes[0]->getNeuron(offset + j)->percept_src = i;
			}
		}
		offset += lobes[i]->getNoNeurons();
	}

	// TODO: should we force a brain tick here? locis need to be initialised, maybe..
	tick();
}

void oldBrain::tick() {
	for (unsigned int i = 0; i < lobes.size(); i++) {
		if (lobe_process_order[i] == 0) {
			// perception lobe copy
			unsigned int offset = 0;
			for (unsigned int n = 1; n < lobes.size(); n++) {
				if (!lobes[n]->getGene()->perceptflag) continue;
				for (unsigned int j = 0; j < lobes[n]->getNoNeurons(); j++) {
					unsigned char output = lobes[n]->getNeuron(j)->output;
					oldNeuron *destneu = lobes[0]->getNeuron(offset);
					if (destneu->state < output)
						destneu->state = output;
					offset++;
				}
			}
		}
		lobes[lobe_process_order[i]]->tick();
	}

	ticks++;
}

oldLobe *oldBrain::getLobeByTissue(unsigned int id) {
	if (id >= lobes.size())
		return 0;

	return lobes[id];
}

void oldBrain::processInstinct(creatureInstinctGene &instinct) {
	// work out *true* source lobe
	unsigned int srclobe[3] = { 0, 0, 0 } ;
	for (unsigned int i = 0; i < 3; i++) {
		// no lobe in the instinct?
		if (!instinct.lobes[i]) continue;

		// TODO: instincts should be sanitised somewhere?
		if (instinct.lobes[i] >= lobes.size()) continue;

		if (lobes[instinct.lobes[i]]->getGene()->perceptflag) {
			// perceptible! we can use this
			srclobe[i] = instinct.lobes[i];
		} else {
			// lobe is not perceptible; try finding a lobe which
			// feeds from it which *is* perceptible
			for (unsigned int j = 1; j < lobes.size(); j++) {
				if (!lobes[j]->getGene()->perceptflag) continue;
				// TODO: check max connections too?
				if (lobes[j]->getGene()->dendrite1.srclobe == instinct.lobes[i] ||
					lobes[j]->getGene()->dendrite2.srclobe == instinct.lobes[i]) {
					srclobe[i] = j;
					break;
				}
			}
		}

		if (!srclobe[i]) {
			fmt::print(
				"instinct: ignoring source lob {} because it's not perceptible\n",
				(int)instinct.lobes[i]
			);
		}
	}

	// find the offsets into the perceptible lobe
	std::vector<unsigned int> percept_neu_offsets;
	for (unsigned int i = 0; i < 3; i++) {
		if (!srclobe[i]) continue;

		unsigned int offset = 0;
		for (unsigned int j = 1; j < srclobe[i]; j++) {
			if (!lobes[j]->getGene()->perceptflag) continue;
			offset += lobes[j]->getNoNeurons();
		}
		offset += instinct.neurons[i];

		// original engine will happily go off the end of the lobe here!
		// (see cdouble's posts about Canny norns causing crashes with instincts using non-perceptible lobes)
		// TODO: sanity-check this at source (ie, above, when finding feed lobe) also?
		if (offset >= lobes[0]->getNoNeurons()) {
			fmt::print(
				"instinct: ignoring offset {} (was lobe {}, now lobe {}, new offset {})\n",
				(int)offset,
				(int)instinct.lobes[i],
				(int)srclobe[i],
				(int)instinct.neurons[i]
			);
			continue;
		}
		percept_neu_offsets.push_back(offset);
	}

	// were there no valid lobes? then we're done
	if (!percept_neu_offsets.size()) {
		fmt::print("instinct failed: no valid lobes\n");
		return;
	}

	// we need a concept lobe (lobe 8) for instincts to work
	if (lobes.size() < 9) {
		fmt::print("instinct failed: no concept lobe\n");
		return;
	}
	// TODO: verify the concept lobe does actually have type 0 dendrites pointing at the perceptible lobe?

	// find a candidate neuron in the concept lobe with the necessary number
	// of type 0 dendrites and minimal strength
	unsigned int candidateneuron = 0xFFFFFFFF;
	for (unsigned int maxstr = 0; maxstr < 256; maxstr++) {
		for (unsigned int i = 0; i < lobes[8]->getNoNeurons(); i++) {
			oldNeuron *neu = lobes[8]->getNeuron(i);

			if (neu->dendrites[0].size() != percept_neu_offsets.size()) continue;

			unsigned int j;
			for (j = 0; j < neu->dendrites[0].size(); j++) {
				if (neu->dendrites[0][j].strength > maxstr) break;
			}
			if (j == neu->dendrites[0].size()) {
				candidateneuron = i;
				break;
			}
		}
		if (candidateneuron != 0xFFFFFFFF) break;
	}

	if (candidateneuron == 0xFFFFFFFF) {
		fmt::print(
			"instinct failed: no candidate concept neuron with {} type 0 dendrites\n",
			(int)percept_neu_offsets.size()
		);
		return;
	}

	// connect the discovered concept neuron to the relevant perceptible neurons
	oldNeuron *conceptneu = lobes[8]->getNeuron(candidateneuron);
	for (unsigned int i = 0; i < percept_neu_offsets.size(); i++) {
		oldDendrite &dend = conceptneu->dendrites[0][i];
		dend.ltw = dend.stw = dend.strength = instinct.level;
		// in theory thanks to earlier sanity checks, all percept_neu_offsets should be in range
		dend.src = lobes[0]->getNeuron(percept_neu_offsets[i]);
	}

	// find relevant decision neuron
	// TODO: again, instincts should be sanitised somewhere
	oldNeuron *decnneu = lobes[6]->getNeuron(instinct.action);

	// work out relevant dendrite type
	// type 1 for punishment chem, type 0 otherwise (reward)
	// this is kind of horrible hardcoding (but then, so is the rest)
	unsigned int type = (instinct.drive == 53 ? 1 : 0);

	// find a minimal-strength dendrite..
	unsigned int candidatedendrite = 0xFFFFFFFF;
	for (unsigned int maxstr = 0; maxstr < 256; maxstr++) {
		unsigned int i;
		for (i = 0; i < decnneu->dendrites[type].size(); i++) {
			if (decnneu->dendrites[type][i].strength <= maxstr) {
				candidatedendrite = i;
				break;
			}
		}
		if (candidatedendrite != 0xFFFFFFFF) break;
	}

	if (candidatedendrite == 0xFFFFFFFF) {
		// brain can't learn this!
		fmt::print("instinct failed: no candidate decision dendrite (!)\n");
		return;
	}

	// finally, wire the concept neuron up to the relevant decision
	oldDendrite &dend = decnneu->dendrites[type][candidatedendrite];
	dend.ltw = dend.stw = dend.strength = instinct.level;
	dend.src = conceptneu;
}

/* vim: set noet: */
