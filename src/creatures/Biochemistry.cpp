/*
 *  Biochemistry.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat May 15 2010.
 *  Copyright (c) 2004-2010 Alyssa Milburn. All rights reserved.
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

#include "oldCreature.h"
#include "c2eCreature.h"
#include <cmath> // powf
#include "c2eBrain.h"
#include "oldBrain.h"

using std::shared_ptr;

void oldCreature::addChemical(unsigned char id, unsigned char val) {
	if (id == 0) return;

	// clipping..
	if ((int)chemicals[id] + val > 255) chemicals[id] = 255;
	else chemicals[id] += val;
}

void oldCreature::subChemical(unsigned char id, unsigned char val) {
	if (id == 0) return;

	// clipping..
	if ((int)chemicals[id] - val < 0) chemicals[id] = 0;
	else chemicals[id] -= val;
}

void c2eCreature::adjustChemical(unsigned char id, float value) {
	if (id == 0) return;
	
	chemicals[id] += value;

	if (chemicals[id] < 0.0f) chemicals[id] = 0.0f;
	else if (chemicals[id] > 1.0f) chemicals[id] = 1.0f;
}

// lookup table, snaffled from real creatures
// TODO: work out if these are meaningful values :)
unsigned int c1rates[32] = {
	0, 0x32A5, 0x71DD, 0xAABB, 0xD110, 0xE758, 0xF35C,
	0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999,
	0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999,
	0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999, 0xF999,
	0xFFFF
};

unsigned int oldCreature::calculateMultiplier(unsigned char rate) {
	return c1rates[rate];
}

unsigned int oldCreature::calculateTickMask(unsigned char rate) {
	if (rate < 7) return 0;
	else return (1 << ((unsigned int)rate - 7)) - 1;
}

void c1Creature::tickBiochemistry() {
	// TODO: untested

	if ((ticks % 5) != 0) return; // TODO: what is the correct tick rate?

	// process emitters
	for (std::vector<c1Emitter>::iterator i = emitters.begin(); i != emitters.end(); i++) {
		processEmitter(*i);
	}

	// process receptors
	for (std::vector<c1Receptor>::iterator i = receptors.begin(); i != receptors.end(); i++) {
		processReceptor(*i);
	}

	// process reactions
	for (std::vector<shared_ptr<c1Reaction> >::iterator i = reactions.begin(); i != reactions.end(); i++) {
		processReaction(**i);
	}

	oldCreature::tickBiochemistry();
}

void c2Creature::tickBiochemistry() {
	// TODO: untested
	
	if ((ticks % 2) != 0) return; // every 2 ticks (0.2s)

	// tick organs
	for (std::vector<shared_ptr<c2Organ> >::iterator x = organs.begin(); x != organs.end(); x++) {
		(*x)->tick();
	}

	oldCreature::tickBiochemistry();
}

void oldCreature::tickBiochemistry() {
	// process half-lives
	if (halflives) { // TODO: correct?
		for (unsigned int i = 0; i < 256; i++) {
			// TODO: this code hasn't been tested thoroughly, but seems to agree with basic testing

			// work out which rate we're dealing with
			unsigned char rate = halflives->halflives[i] / 8;
	
			// if the tickmask doesn't want us to change things this tick, don't!
			if ((biochemticks & calculateTickMask(rate)) != 0) continue;

			// do the actual adjustment
			chemicals[i] = (chemicals[i] * calculateMultiplier(rate)) / 65536;
		}
	}
	
	biochemticks++;
}

void c2eCreature::tickBiochemistry() {
	// only process biochem every 4 ticks
	// TODO: correct? should probably apply to brain too, at least
	if ((ticks % 4) != 0) return;
	
	// tick organs
	for (std::vector<shared_ptr<c2eOrgan> >::iterator x = organs.begin(); x != organs.end(); x++) {
		(*x)->tick();
	}

	// process half-lives for chemicals
	if (!halflives) return; // TODO: correct?
	for (unsigned int x = 0; x < 256; x++) {
		if (halflives->halflives[x] == 0) {
			// 0 is a special case for half-lives
			chemicals[x] = 0.0f;
		} else {
			// reaction rate = 1.0 - 0.5**(1.0 / 2.2**(rate * 32.0 / 255.0))
			float rate = 1.0 - powf(0.5, 1.0 / powf(2.2, (halflives->halflives[x] * 32.0) / 255.0));

			chemicals[x] -= chemicals[x] * rate;
		}
	}
}

unsigned char *oldCreature::getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l) {
	switch (o) {
		case 0: // brain
		{
			oldLobe *lobe = brain->getLobeByTissue(t);
			if (!lobe) return 0;

			if (receptor) {
				// receptor
				switch (l) {
					case 0: // threshold
						return lobe->getThresholdPointer();

					case 1: // leakage
						return lobe->getLeakageRatePointer();

					case 2: // gain
						return lobe->getInputGainPointer();

					default:
						if (l < 13) {
							l = l - 3;
							unsigned char dendriteset = l / 5;
							if (dendriteset == 1) l -= 5;

							switch (l) {
								case 0: // relax suscept
									return 0; // TODO

								case 1: // relax STW
									return 0; // TODO

								case 2: // relax LTW
									return 0; // TODO

								case 3: // strength gain rate
									return 0; // TODO

								case 4: // strength loss rate
									return 0; // TODO
							}

							return 0;
						}

						bool c2 = (dynamic_cast<c2Creature *>(this)); // TODO: evil
						unsigned char nochems = c2 ? 6 : 4;

						if (l - 13 < nochems) {
							unsigned char chemid = l - 13;

							return lobe->getChemPointer(chemid);
						}

						// The Genetics Kit limits to 0-15 in c1 and 0-39 in c2. We don't bother..
						unsigned int cellid = l - 13 - nochems;
						if (c2) {
							// TODO: this is because c2 has 16-212 *after* 0-39 in the genetics kit, is it needed?!
							if (cellid >= 40) cellid -= 24;
						}
						if (cellid >= lobe->getNoNeurons()) return 0;
						return &lobe->getNeuron(cellid)->state;
				}
			} else {
				// emitter
				switch (l) {
					case 0: // lobe activity
						return lobe->getLobeActivityPointer();

					case 1: // #loose dens/cells type 0
						return lobe->getLooseDendsPointer(0);

					case 2: // #loose dens/cells type 1
						return lobe->getLooseDendsPointer(1);

					default: // cell output
					{
						unsigned char cellid = l - 3;
						// The Genetics Kit limits to 0-15 in c1 and 0-39 in c2. We don't bother..
						if (cellid >= lobe->getNoNeurons()) return 0;
						return &lobe->getNeuron(cellid)->output;
					}
				}
			}
		}

		case 1: // creature
			switch (t) {
				case 0: // somantic
					if (receptor) {
						if (l > 6) break;
						return &lifestageloci[l];
					} else if (l == 0) return &muscleenergy;
					break;

				case 1: // circulatory
					if (l > 8) break;
					return &floatingloci[l];
			
				case 2: // reproductive
					if (receptor) {
						if (l == 0) return &fertile;
						else if (l == 1) return &receptive;
					} else {
						if (l == 0) return &fertile;
						else if (l == 1) return &pregnant;
					}
					break;
			
				case 3: // immune
					if (l == 0) return &dead;
					break;
			}
	}
	
	return 0;
}

unsigned char *c1Creature::getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l) {
	unsigned char *oldlocus = oldCreature::getLocusPointer(receptor, o, t, l);
	if (oldlocus) return oldlocus;

	switch (o) {
		case 1: // creature
			switch (t) {
				case 4: // sensorimotor
					if (receptor) {
						if (l < 8) return &involaction[l];
						else if (l < 16) return &gaitloci[l - 8];
					} else {
						if (l < 6) return &senses[l];
					}
					break;

				case 5: // drive levels
					if (l < 16) return &drives[l];
			}
			break;
	}

	std::cout << "c1Creature::getLocusPointer failed to interpret locus (" << (int)o << ", "
		<< (int)t << ", " << (int)l << ") of " << (receptor ? "receptor" : "emitter")
		<< std::endl;

	return 0;
}

unsigned char *c2Creature::getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l) {
	unsigned char *oldlocus = oldCreature::getLocusPointer(receptor, o, t, l);
	if (oldlocus) return oldlocus;

	switch (o) {
		case 1: // creature
			switch (t) {
				case 4: // sensorimotor
					if (receptor) {
						if (l < 8) return &involaction[l];
						else if (l < 24) return &gaitloci[l - 8];
					} else {
						if (l < 14) return &senses[l];
					}
					break;

				case 5: // drive levels
					if (l < 17) return &drives[l];
			}
			break;
	}

	std::cout << "c2Creature::getLocusPointer failed to interpret locus (" << (int)o << ", "
		<< (int)t << ", " << (int)l << ") of " << (receptor ? "receptor" : "emitter")
		<< std::endl;

	return 0;
}

float *c2eCreature::getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l) {
	switch (o) {
		case 0: // brain
			{
			c2eLobe *lobe = brain->getLobeByTissue(t);
			if (!lobe) break;

			unsigned int neuronid = o/3, stateno = o%3;
			if (neuronid >= lobe->getNoNeurons()) break;
			return &lobe->getNeuron(neuronid)->variables[stateno];
			}

		case 1: // creature
			switch (t) {
				case 0: // somatic
					if (receptor) {
						if (l > 6) break;
						return &lifestageloci[l];
					} else if (l == 0) return &muscleenergy;
					break;

				case 1: // circulatory
					if (l > 31) break;
					return &floatingloci[l];

				case 2: // reproductive
					{ int val = l;

					if (!receptor) { // emitter
						if (l == 0) return &fertile;
						else if (l == 1) return &pregnant;
						val = l - 2;
					}

					switch (val) {
						case 0: return &ovulate;
						case 1: return &receptive;
						case 2: return &chanceofmutation;
						case 3: return &degreeofmutation;
					} }
					break;
					
				case 3: // immune
					if (l == 0) return &dead;
					break;
				
				case 4: // sensorimotor
					{ int val = l;

					if (!receptor) { // emitter
						if (val < 14) return &senses[val];
						val -= 14;
					}
					if (val < 8) return &involaction[val];
					val -= 8;
					if (val < 16) return &gaitloci[val];
					}
					break;

				case 5: // drives
					if (l < 20) return &drives[l];
					break;
			}

	}

	std::cout << "c2eCreature::getLocusPointer failed to interpret locus (" << (int)o << ", "
		<< (int)t << ", " << (int)l << ") of " << (receptor ? "receptor" : "emitter")
		<< std::endl;
	return 0;
}
 
/*****************************************************************************/

c2Organ::c2Organ(c2Creature *p, organGene *g) {
	parent = p; assert(parent);
	ourGene = g; assert(ourGene);
	lifeforce = ourGene->lifeforce * (1000000.0f / 255.0f);
	longtermlifeforce = shorttermlifeforce = lifeforce;

	biochemticks = 0;

	repairrate = 0;
	clockrate = ourGene->clockrate;
	injurytoapply = 0;
	damagerate = ourGene->damagerate;
	biotick = 0;
	atpdamagecoefficient = ourGene->atpdamagecoefficient * (lifeforce / (255.0f * 255.0f));
	
	// TODO: is genes.size() always the size we want?
	energycost = 2 + (ourGene->genes.size() / 10);

	clockratereceptors = repairratereceptors = injuryreceptors = 0;
}

c2eOrgan::c2eOrgan(c2eCreature *p, organGene *g) {
	parent = p; assert(parent);
	ourGene = g; assert(ourGene);
	lifeforce = ourGene->lifeforce * (1000000.0f / 255.0f);
	longtermlifeforce = shorttermlifeforce = lifeforce;

	repairrate = 0.0f;
	clockrate = ourGene->clockrate / 255.0f;
	injurytoapply = 0.0f;
	damagerate = ourGene->damagerate / 255.0f;
	biotick = ourGene->biotickstart / 255.0f;
	atpdamagecoefficient = ourGene->atpdamagecoefficient * (lifeforce / (255.0f * 255.0f));

	// TODO: is genes.size() always the size we want?
	energycost = (1.0f / 128.0f) + ourGene->genes.size() * (0.1f / 255.0f);
}

void c2Organ::processGenes() {
	for (vector<gene *>::iterator i = ourGene->genes.begin(); i != ourGene->genes.end(); i++) {
		if (!parent->shouldProcessGene(*i)) continue;

		if (typeid(*(*i)) == typeid(bioReactionGene)) {
			reactions.push_back(shared_ptr<c2Reaction>(new c2Reaction()));
			reactions.back()->init((bioReactionGene *)(*i));
		} else if (typeid(*(*i)) == typeid(bioEmitterGene)) {
			emitters.push_back(c2Emitter());
			emitters.back().init((bioEmitterGene *)(*i), this);
		} else if (typeid(*(*i)) == typeid(bioReceptorGene)) {
			receptors.push_back(c2Receptor());
			receptors.back().init((bioReceptorGene *)(*i), this);
		}
	}
}

void c2eOrgan::processGenes() {
	shared_ptr<c2eReaction> r; // we need to store the previous reaction for possible receptor use
	// TODO: should this cope with receptors created at other lifestages? i doubt it.. - fuzzie

	for (vector<gene *>::iterator i = ourGene->genes.begin(); i != ourGene->genes.end(); i++) {
		if (!parent->shouldProcessGene(*i)) continue;

		if (typeid(*(*i)) == typeid(bioReactionGene)) {
			reactions.push_back(shared_ptr<c2eReaction>(new c2eReaction()));
			r = reactions.back();
			reactions.back()->init((bioReactionGene *)(*i));
		} else if (typeid(*(*i)) == typeid(bioEmitterGene)) {
			emitters.push_back(c2eEmitter());
			emitters.back().init((bioEmitterGene *)(*i), this);
		} else if (typeid(*(*i)) == typeid(bioReceptorGene)) {
			receptors.push_back(c2eReceptor());
			receptors.back().init((bioReceptorGene *)(*i), this, r);
		}
	}
}

void c2Organ::tick() {
	if (longtermlifeforce <= 0.5f) return; // We're dead!

	biotick += clockrate;

	bool ticked = false;

	/*
	 * the variable we call 'biotick' here is actually 'clock' in c2 - biochemticks is actual biotick
	 * TODO: rename biotick to clock
	 */
	/*
	 * This seems to be the correct 'algorithm' for working out when the organ should be
	 * processed, despite the c2 Science Kit showing a different result. beware, the
	 * Brain Organ is only processed when the brain is (ie: half the speed of biochem)
	 */
	if (biotick >= 255) {
		biotick -= 255;
		biochemticks++;

		// TODO: is this the correct way to handle biotickstart?
		//if (biochemticks < ourGene->biotickstart) return;
		
		ticked = true;

		// chem 99 = ATP, chem 100 = ADP (hardcoded)
		unsigned char atplevel = parent->getChemical(99);
		if (atplevel >= energycost) {
			// energy consumption
			parent->subChemical(99, energycost);
			parent->addChemical(100, energycost);	
			
			// *** tick emitters
			for (vector<c2Emitter>::iterator i = emitters.begin(); i != emitters.end(); i++)
				processEmitter(*i);
			
			// *** tick reactions
			for (vector<shared_ptr<c2Reaction> >::iterator i = reactions.begin(); i != reactions.end(); i++)
				processReaction(**i);
		} else {
			// not enough energy!
			// TODO: apply atpdamagecoefficient
		}


		// *** long-term damage
		float diff = longtermlifeforce - shorttermlifeforce;
		longtermlifeforce = longtermlifeforce - (diff * (damagerate / 255.0)); // TODO: correct?
	
		// *** repair injuries
		// TODO: doesn't this mean STLF could go above LTLF?
		float repair = diff * (repairrate / 255.0); // TODO: correct?
		shorttermlifeforce += repair;

		// TODO: adjust injury chem, apply injury
	}

	// TODO: ensure we've ticked at least once
	//if (biochemticks < ourGene->biotickstart) return;
	if (biochemticks == 0) return;

	// *** tick receptors
	if (clockratereceptors != 0) clockrate = 0;
	if (repairratereceptors != 0) repairrate = 0;
	if (injuryreceptors != 0) injurytoapply = 0;
		
	for (vector<c2Receptor>::iterator i = receptors.begin(); i != receptors.end(); i++)
		processReceptor(*i, ticked);

	// *** decay life force
	if (ticked) {
		shorttermlifeforce -= shorttermlifeforce * (1.0f / 100000.0f);
		longtermlifeforce -= longtermlifeforce * (1.0f / 100000.0f);
	}
}

void c2eOrgan::tick() {
	if (longtermlifeforce <= 0.5f) return; // We're dead!

	biotick += clockrate;

	bool ticked = false;
	
	// if it's our turn to tick..
	if (biotick >= 1.0f) {
		ticked = true;
		// .. push the biotick back down
		biotick -= 1.0f;

		// *** energy consumption
		// chem 35 = ATP, chem 36 = ADP (TODO: fix hardcoding)
		float atplevel = parent->getChemical(35);
		bool hadenergy = false;
		if (atplevel >= energycost) {
			hadenergy = true;
			parent->adjustChemical(35, -energycost);
			parent->adjustChemical(36, energycost);
			
			// *** tick emitters
			for (vector<c2eEmitter>::iterator i = emitters.begin(); i != emitters.end(); i++)
				processEmitter(*i);
			
			// *** tick reactions
			for (vector<shared_ptr<c2eReaction> >::iterator i = reactions.begin(); i != reactions.end(); i++)
				processReaction(**i);
		} else {
			// *** out of energy damage	
			applyInjury(atpdamagecoefficient);
		}

		// *** long-term damage
		float diff = longtermlifeforce - shorttermlifeforce;
		longtermlifeforce = longtermlifeforce - (diff * damagerate); // damagerate always <= 1.0

		// *** repair injuries
		// TODO: doesn't this mean STLF could go above LTLF?
		float repair = diff * repairrate; // repairrate always <= 1.00
		shorttermlifeforce += repair;
		// adjust Injury chemical (TODO: de-hardcode)
		parent->adjustChemical(127, -repair / lifeforce);

		if (injurytoapply)
			applyInjury(injurytoapply);
	}
	
	// *** tick receptors	
	for (vector<shared_ptr<c2eReaction> >::iterator i = reactions.begin(); i != reactions.end(); i++) (*i)->receptors = 0;
	clockratereceptors = 0; repairratereceptors = 0; injuryreceptors = 0;
		
	for (vector<c2eReceptor>::iterator i = receptors.begin(); i != receptors.end(); i++)
		processReceptor(*i, ticked);
	
	for (vector<shared_ptr<c2eReaction> >::iterator i = reactions.begin(); i != reactions.end(); i++) if ((*i)->receptors > 0) (*i)->rate /= (*i)->receptors;
	if (clockratereceptors > 0) clockrate /= clockratereceptors;
	if (repairratereceptors > 0) repairrate /= repairratereceptors;
	if (injuryreceptors > 0) injurytoapply /= injuryreceptors;
	
	// *** decay life force
	// TODO: is this correct, or should these be equal to c2 values above?
	// TODO: do we damage on every tick, or only when processed (like c2)?
	shorttermlifeforce -= shorttermlifeforce * (1.0f / 1000000.0f);
	longtermlifeforce -= longtermlifeforce * (1.0f / 1000000.0f);
}

void c2eOrgan::applyInjury(float value) {
	shorttermlifeforce -= value;
	if (shorttermlifeforce < 0.0f)
		shorttermlifeforce = 0.0f;
	// adjust Injury chemical (TODO: de-hardcode)
	parent->adjustChemical(127, value / lifeforce);
}

void c1Creature::processReaction(c1Reaction &d) {
	// TODO: untested

	bioReactionGene &g = *d.data;

	// TODO: this might not all be correct

	// work out which rate we're dealing with
	unsigned char rate = g.rate / 8;
	
	// if the tickmask doesn't want us to change things this tick, don't!
	if ((biochemticks & calculateTickMask(rate)) != 0) return;

	unsigned char ratio = 255, ratio2 = 255;
	if (g.reactant[0] != 0) {
		assert(g.quantity[0] != 0); // TODO
		ratio = getChemical(g.reactant[0]) / g.quantity[0];
	}
	if (g.reactant[1] != 0) {
		assert(g.quantity[1] != 0); // TODO
		ratio2 = getChemical(g.reactant[1]) / g.quantity[1];
	}

	// pick lowest ratio, if zero then return
	if (ratio2 < ratio) ratio = ratio2;
	if (ratio == 0) return;
	
	// calculate the actual adjustment (can't go out of bounds)
	ratio = ratio - ((ratio * calculateMultiplier(rate)) / 65536);

	// change chemical levels
	subChemical(g.reactant[0], ratio * g.quantity[0]);
	subChemical(g.reactant[1], ratio * g.quantity[1]);
	addChemical(g.reactant[2], ratio * g.quantity[2]);
	addChemical(g.reactant[3], ratio * g.quantity[3]);
}

void c2Organ::processReaction(c2Reaction &d) {
	// TODO: untested

	bioReactionGene &g = *d.data;

	// TODO: this might not all be correct

	// work out which rate we're dealing with
	unsigned char rate = g.rate / 8;
	
	// if the tickmask doesn't want us to change things this tick, don't!
	if ((biochemticks & parent->calculateTickMask(rate)) != 0) return;

	unsigned char ratio = 255, ratio2 = 255;
	if (g.reactant[0] != 0) {
		assert(g.quantity[0] != 0); // TODO
		ratio = parent->getChemical(g.reactant[0]) / g.quantity[0];
	}
	if (g.reactant[1] != 0) {
		assert(g.quantity[1] != 0); // TODO
		ratio2 = parent->getChemical(g.reactant[1]) / g.quantity[1];
	}

	// pick lowest ratio, if zero then return
	if (ratio2 < ratio) ratio = ratio2;
	if (ratio == 0) return;
	
	// calculate the actual adjustment (can't go out of bounds)
	ratio = ratio - ((ratio * parent->calculateMultiplier(rate)) / 65536);

	// change chemical levels
	parent->subChemical(g.reactant[0], ratio * g.quantity[0]);
	parent->subChemical(g.reactant[1], ratio * g.quantity[1]);
	parent->addChemical(g.reactant[2], ratio * g.quantity[2]);
	parent->addChemical(g.reactant[3], ratio * g.quantity[3]);
}

void c2eOrgan::processReaction(c2eReaction &d) {
	bioReactionGene &g = *d.data;
	
	// TODO: this might not all be correct

	float ratio = 1.0f, ratio2 = 1.0f;
	if (g.reactant[0] != 0) {
		assert(g.quantity[0] != 0); // TODO
		ratio = parent->getChemical(g.reactant[0]) / (float)g.quantity[0];
	}
	if (g.reactant[1] != 0) {
		assert(g.quantity[1] != 0); // TODO
		ratio2 = parent->getChemical(g.reactant[1]) / (float)g.quantity[1];
	}

	// pick lowest ratio, if zero then return
	if (ratio2 < ratio) ratio = ratio2;
	if (ratio == 0.0f) return;

	// calculate the actual adjustment
	float rate = 1.0 - powf(0.5, 1.0 / powf(2.2, (1.0 - d.rate) * 32.0));
	ratio = ratio * rate;

	// change chemical levels
	parent->adjustChemical(g.reactant[0], -(ratio * (float)g.quantity[0]));
	parent->adjustChemical(g.reactant[1], -(ratio * (float)g.quantity[1]));
	parent->adjustChemical(g.reactant[2], ratio * (float)g.quantity[2]);
	parent->adjustChemical(g.reactant[3], ratio * (float)g.quantity[3]);
}

void c1Creature::processEmitter(c1Emitter &d) {
	// TODO: untested

	bioEmitterGene &g = *d.data;

	if ((biochemticks % g.rate) != 0) return;

	if (!d.locus) return;
	unsigned char f = *d.locus;
	if (g.clear) *d.locus = 0;
	if (g.invert) f = 255 - f;

	if (g.digital) {
		if (f < g.threshold) return;
		addChemical(g.chemical, g.gain);
	} else {
		int r = (((int)f - g.threshold) * g.gain) / 255;
		
		// clip the result of the calculation to unsigned char, and reassign it
		if (r < 0) r = 0; else if (r > 255) r = 255;
		f = r;

		addChemical(g.chemical, f);
	}
}

void c2Organ::processEmitter(c2Emitter &d) {
	// TODO: untested

	bioEmitterGene &g = *d.data;

	if ((biochemticks % g.rate) != 0) return;

	if (!d.locus) return;
	unsigned char f = *d.locus;
	if (g.clear) *d.locus = 0;
	if (g.invert) f = 255 - f;

	if (g.digital) {
		if (f < g.threshold) return;
		parent->addChemical(g.chemical, g.gain);
	} else {
		int r = (((int)f - g.threshold) * g.gain) / 255;
		
		// clip the result of the calculation to unsigned char, and reassign it
		if (r < 0) r = 0; else if (r > 255) r = 255;
		f = r;

		parent->addChemical(g.chemical, f);
	}
}

void c2eOrgan::processEmitter(c2eEmitter &d) {
	bioEmitterGene &g = *d.data;
	
	if (d.sampletick != g.rate) {
		assert(d.sampletick < g.rate);
		d.sampletick++;
		return;
	}
	
	d.sampletick = 0;

	if (!d.locus) return;
	float f = *d.locus;
	if (g.clear) *d.locus = 0.0f;
	if (g.invert) f = 1.0f - f;

	if (g.digital) {
		if (f < d.threshold) return;
		parent->adjustChemical(g.chemical, d.gain);
	} else {
		f = (f - d.threshold) * d.gain;
		if (f > 0.0f) // TODO: correct check?
			parent->adjustChemical(g.chemical, f);
	}
}

void c1Creature::processReceptor(c1Receptor &d) {
	// TODO: untested

	bioReceptorGene &g = *d.data;

	// TODO: same issues as c2eOrgan::processReceptor below, probably

	if (!d.locus) return;

	unsigned char f = getChemical(g.chemical);
	int r;
	if (g.digital)
		r = f > g.threshold ? g.gain : 0;
	else
		// TODO: int promotion correct to makke this work out?
		r = (((int)f - g.threshold) * g.gain) / 255;
	
	if (g.inverted) r = g.nominal - r;
	else r += g.nominal;
	
	// clip the result of the calculation to unsigned char, and reassign it
	if (r < 0) r = 0; else if (r > 255) r = 255;
	f = r;

	if (f == 0 && g.organ == 1 && g.tissue == 3 && g.locus == 0) // evil check for "Die if non-zero!" locus
		return;
	else
		*d.locus = f;
}

void c2Organ::processReceptor(c2Receptor &d, bool checkchem) {
	// TODO: untested

	bioReceptorGene &g = *d.data;

	// TODO: same issues as c2eOrgan::processReceptor below, probably

	if (checkchem) {
		d.processed = true;
		d.lastvalue = parent->getChemical(g.chemical);
	}

	if (!d.processed) return;
	if (!d.locus) return;

	unsigned char f = d.lastvalue;
	int r;
	if (g.digital)
		r = f > g.threshold ? g.gain : 0;
	else
		// TODO: int promotion correct to makke this work out?
		r = (((int)f - g.threshold) * g.gain) / 255;
	
	if (g.inverted) r = g.nominal - r;
	else r += g.nominal;
	
	// clip the result of the calculation to unsigned char, and reassign it
	if (r < 0) r = 0; else if (r > 255) r = 255;
	f = r;

	if (f == 0 && g.organ == 1 && g.tissue == 3 && g.locus == 0) // evil check for "Die if non-zero!" locus
		return;
	else if (d.receptors)
		*d.locus += (f / *d.receptors);
	else
		*d.locus = f;
}


void c2eOrgan::processReceptor(c2eReceptor &d, bool checkchem) {
	bioReceptorGene &g = *d.data;
	
	/*
	 * TODO: This code has issues..
	 *
	 * eg, if you have two receptors pointing at a non-local locus,
	 * we just stomp over it in order, so the last receptor always get it
	 * while c2e seems to alternate between them (possibly organ clockrate stuff)
	 *
	 */
	
	if (checkchem) {
		d.processed = true;
		d.lastvalue = parent->getChemical(g.chemical);
	}

	if (!d.processed) return;
	if (!d.locus) return;

	float f;
	if (g.digital)
		f = d.lastvalue > d.threshold ? d.gain : 0.0f;
	else
		f = (d.lastvalue - d.threshold) * d.gain;
	if (g.inverted) f *= -1.0f;
	f += d.nominal;

	if (f < 0.0f) f = 0.0f; else if (f > 1.0f) f = 1.0f; // TODO: correct?

	if (f == 0.0f && g.organ == 1 && g.tissue == 3 && g.locus == 0) // evil check for "Die if non-zero!" locus
		return;
	else if (d.receptors) {
		if (*d.receptors == 0) *d.locus = 0.0f;
		(*d.receptors)++;
		*d.locus += f;
	} else
		*d.locus = f;
}

unsigned char *c2Organ::getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l, unsigned int**receptors) {
	if (receptors) *receptors = 0;

	switch (o) {
		case 2: // organ
			if (t == 0)
				switch (l) {
					case 0: // clock rate
						if (receptors) {
							*receptors = &clockratereceptors;
							clockratereceptors++;
						}
						return &clockrate;
					case 1: // repair rate
						if (receptors) {
							*receptors = &repairratereceptors;
							repairratereceptors++;
						}
						return &repairrate;
					case 2: // injury to apply
						if (receptors) {
							*receptors = &injuryreceptors;
							injuryreceptors++;
						}
						return &injurytoapply;
				}
			break;
	}

	return parent->getLocusPointer(receptor, o, t, l);
}

float *c2eOrgan::getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l, unsigned int**receptors) {
	if (receptors) *receptors = 0;

	switch (o) {
		case 2: // organ
			if (t == 0)
				switch (l) {
					case 0: // clock rate
						if (receptors) *receptors = &clockratereceptors;
						return &clockrate;
					case 1: // repair rate
						if (receptors) *receptors = &repairratereceptors;
						return &repairrate;
					case 2: // injury to apply
						if (receptors) *receptors = &injuryreceptors;
						return &injurytoapply;
				}
			break;
		case 3: // reaction
			if (t == 0 && l == 0) { // reaction rate
				shared_ptr<c2eReaction> r = reactions.back();
				if (!r) {
					std::cout << "c2eOrgan::getLocusPointer failed to find a reaction" << std::endl;
					return 0;
				} else {
					if (receptors) *receptors = &r->receptors;
					return &r->rate;
				}
			}
	}

	return parent->getLocusPointer(receptor, o, t, l);
}

void c1Reaction::init(bioReactionGene *g) {
	data = g;
}

void c2Reaction::init(bioReactionGene *g) {
	data = g;
}

void c2eReaction::init(bioReactionGene *g) {
	data = g;

	// rate is stored in genome as 0 fastest, 255 slowest
	// reversed in game, i think .. TODO: check this
	rate = 1.0 - (g->rate / 255.0);
}

void c1Receptor::init(bioReceptorGene *g, c1Creature *parent) {
	data = g;
	locus = parent->getLocusPointer(true, g->organ, g->tissue, g->locus);
}

void c2Receptor::init(bioReceptorGene *g, c2Organ *parent) {
	data = g;
	processed = false;
	locus = parent->getLocusPointer(true, g->organ, g->tissue, g->locus, &receptors);
}
	
void c2eReceptor::init(bioReceptorGene *g, c2eOrgan *parent, shared_ptr<c2eReaction> r) {
	data = g;
	processed = false;
	nominal = g->nominal / 255.0f;
	threshold = g->threshold / 255.0f;
	gain = g->gain / 255.0f;
	locus = parent->getLocusPointer(true, g->organ, g->tissue, g->locus, &receptors);
}

void c1Emitter::init(bioEmitterGene *g, c1Creature *parent) {
	data = g;
	locus = parent->getLocusPointer(false, g->organ, g->tissue, g->locus);
}

void c2Emitter::init(bioEmitterGene *g, c2Organ *parent) {
	data = g;
	locus = parent->getLocusPointer(false, g->organ, g->tissue, g->locus, 0);
}

void c2eEmitter::init(bioEmitterGene *g, c2eOrgan *parent) {
	data = g;
	sampletick = 0;
	threshold = g->threshold / 255.0f;
	gain = g->gain / 255.0f;
	locus = parent->getLocusPointer(false, g->organ, g->tissue, g->locus, 0);
}

/* vim: set noet: */
