/*
 *  c2eCreature.h
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

#ifndef __C2ECREATURE_H
#define __C2ECREATURE_H

#include "Creature.h"

// c2e

struct c2eReaction {
	bioReactionGene *data;
	float rate;
	unsigned int receptors;
	void init(bioReactionGene *);
};

struct c2eReceptor {
	bioReceptorGene *data;
	bool processed;
	float lastvalue;
	float *locus;
	unsigned int *receptors;
	float nominal, threshold, gain;
	void init(bioReceptorGene *, class c2eOrgan *, std::shared_ptr<c2eReaction>);
};

struct c2eEmitter {
	bioEmitterGene *data;
	unsigned char sampletick;
	float *locus;
	float threshold, gain;
	void init(bioEmitterGene *, class c2eOrgan *);
};

class c2eOrgan {
protected:
	friend struct c2eReceptor;
	friend struct c2eEmitter;

	class c2eCreature *parent;	
	organGene *ourGene;

	std::vector<std::shared_ptr<c2eReaction> > reactions;
	std::vector<c2eReceptor> receptors;
	std::vector<c2eEmitter> emitters;

	// data
	float energycost, atpdamagecoefficient;
	
	// variables
	float lifeforce, shorttermlifeforce, longtermlifeforce;
	
	// locuses
	float biotick, damagerate, repairrate, clockrate, injurytoapply;
	unsigned int clockratereceptors, repairratereceptors, injuryreceptors;

	void processReaction(c2eReaction &);
	void processEmitter(c2eEmitter &);
	void processReceptor(c2eReceptor &, bool checkchem);
	
	float *getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l, unsigned int **receptors);

public:
	c2eOrgan(c2eCreature *p, organGene *g);
	void tick();

	void processGenes();

	float getClockRate() { return clockrate; }
	float getRepairRate() { return repairrate; }
	float getDamageRate() { return damagerate; }
	float getEnergyCost() { return energycost; }
	float getInjuryToApply() { return injurytoapply; }
	float getInitialLifeforce() { return lifeforce; }
	float getShortTermLifeforce() { return shorttermlifeforce; }
	float getLongTermLifeforce() { return longtermlifeforce; }
	float getATPDamageCoefficient() { return atpdamagecoefficient; }
	
	unsigned int getReceptorCount() { return receptors.size(); }
	unsigned int getEmitterCount() { return emitters.size(); }
	unsigned int getReactionCount() { return reactions.size(); }
	
	void applyInjury(float);
};

struct c2eStim {
	int noun_id;
	float noun_amount;
	int verb_id;
	float verb_amount;

	int drive_id[4];
	float drive_amount[4];
	bool drive_silent[4];
	
	c2eStim() { noun_id = -1; verb_id = -1; drive_id[0] = -1; drive_id[1] = -1; drive_id[2] = -1; drive_id[3] = -1; }
	void setupDriveStim(unsigned int num, int id, float amt, bool si) { drive_id[num] = id; drive_amount[num] = amt; drive_silent[num] = si; }
};

class c2eCreature : public Creature {
protected:
	// brain config: should possibly be global
	std::vector<unsigned int> mappinginfo;
	
	// biochemistry
	std::vector<std::shared_ptr<c2eOrgan> > organs;
	float chemicals[256];

	// loci
	float lifestageloci[7];
	float muscleenergy;
	float floatingloci[32];
	float fertile, pregnant, ovulate, receptive, chanceofmutation, degreeofmutation;
	float dead;
	float senses[14], involaction[8], gaitloci[16];
	float drives[20];

	unsigned int involactionlatency[8];

	bioHalfLivesGene *halflives;

	class c2eBrain *brain;

	void tickBrain();
	bool processInstinct();
	void tickBiochemistry();
	void processGenes();
	void addGene(gene *);

	int reverseMapVerbToNeuron(unsigned int verb);
	AgentRef selectRepresentativeAgent(int type, std::vector<AgentRef> possibles);

public:
	c2eCreature(std::shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a);

	void tick();

	void adjustChemical(unsigned char id, float value);
	float getChemical(unsigned char id) { return chemicals[id]; }
	void adjustDrive(unsigned int id, float value);
	float getDrive(unsigned int id) { assert(id < 20); return drives[id]; }

	void setInvolActionLatency(unsigned int id, unsigned int n) { assert(id < 8); involactionlatency[id] = n; }

	void handleStimulus(c2eStim &stim);
	void handleStimulus(unsigned int id, float strength);

	unsigned int noOrgans() { return organs.size(); }
	std::shared_ptr<c2eOrgan> getOrgan(unsigned int i) { assert(i < organs.size()); return organs[i]; }
	
	class c2eBrain *getBrain() { return brain; }

	float *getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l);
	
	unsigned int getGait();
};

#endif

/* vim: set noet: */
