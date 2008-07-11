/*
 *  Creature.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
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

#ifndef __CREATURE_H
#define __CREATURE_H

#include "Agent.h"
#include "genome.h"

#include <deque>

class CreatureAgent;
class Creature;

class Creature {
protected:
	CreatureAgent *parent;
	Agent *parentagent;
	shared_ptr<genomeFile> genome;
	
	// non-specific bits
	unsigned short genus;
	unsigned int variant;
	bool female;
	
	// state
	bool alive, asleep, dreaming, tickage;
	bool zombie;

	unsigned int ticks;
	unsigned int age; // in ticks
	lifestage stage;

	AgentRef attention;
	int attn, decn;
	
	std::vector<AgentRef> chosenagents;
	bool agentInSight(AgentRef a);
	void chooseAgents();
	virtual AgentRef selectRepresentativeAgent(int type, std::vector<AgentRef> possibles) { return AgentRef(); } // TODO: make pure virtual?

	// linguistic stuff

	// to-be-processed instincts
	std::deque<creatureInstinctGene *> unprocessedinstincts;

	// conscious flag? brain/motor enabled flags? flags for each 'faculty'?
	
	unsigned short tintinfo[5]; // red, green, blue, rotation, swap

	virtual void processGenes();
	virtual void addGene(gene *);
	
	Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a);
	void finishInit();

public:
	virtual ~Creature();
	virtual void tick();

	virtual void ageCreature();
	lifestage getStage() { return stage; }

	void setAsleep(bool asleep);
	bool isAsleep() { return asleep; }
	void setDreaming(bool dreaming);
	bool isDreaming() { return dreaming; }
	bool isFemale() { return female; }
	bool isAlive() { return alive; }
	void setZombie(bool z) { zombie = z; }
	bool isZombie() { return zombie; }
	unsigned int getAge() { return age; }
	shared_ptr<genomeFile> getGenome() { return genome; }

	unsigned short getGenus() { return genus; }
	unsigned int getVariant() { return variant; }
	unsigned short getTint(unsigned int id) { return tintinfo[id]; }

	unsigned int getNoCategories() { return chosenagents.size(); }
	AgentRef getChosenAgentForCategory(unsigned int cat) { assert(cat < chosenagents.size()); return chosenagents[cat]; }
	AgentRef getAttentionFocus() { return attention; }
	int getAttentionId() { return attn; }
	int getDecisionId() { return decn; }

	virtual unsigned int getGait() = 0;
	
	void born();
	void die();
	
	bool shouldProcessGene(gene *);
};

// c1/c2

class oldCreature : public Creature {
protected:
	// biochemistry
	unsigned char chemicals[256];

	class oldBrain *brain;
	
	unsigned int biochemticks;
	bioHalfLivesGene *halflives;

	// loci
	unsigned char floatingloci[8];
	unsigned char muscleenergy;
	unsigned char lifestageloci[7];
	unsigned char fertile, receptive, pregnant;
	unsigned char dead;
	unsigned char involaction[8];
	
	void addGene(gene *);
	void tickBrain();
	virtual void tickBiochemistry();

	inline unsigned int calculateTickMask(unsigned char);
	inline unsigned int calculateMultiplier(unsigned char);

	oldCreature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a);
	
	void processGenes();

public:
	void addChemical(unsigned char id, unsigned char val);
	void subChemical(unsigned char id, unsigned char val);
	unsigned char getChemical(unsigned char id) { return chemicals[id]; }	
	
	// TODO: is it really worth having drives outside oldCreature?
	virtual unsigned char getDrive(unsigned int id) = 0;

	oldBrain *getBrain() { return brain; }
};

// c1

struct c1Reaction {
	bioReactionGene *data;
	void init(bioReactionGene *);
};

struct c1Receptor {
	bioReceptorGene *data;
	unsigned char *locus;
	void init(bioReceptorGene *, class c1Creature *);
};

struct c1Emitter {
	bioEmitterGene *data;
	unsigned char *locus;
	void init(bioEmitterGene *, class c1Creature *);
};

class c1Creature : public oldCreature {
protected:
	std::vector<shared_ptr<c1Reaction> > reactions;
	std::vector<c1Receptor> receptors;
	std::vector<c1Emitter> emitters;
	
	// loci
	unsigned char senses[6];
	unsigned char gaitloci[8];
	unsigned char drives[16];

	void addGene(gene *);
	void tickBiochemistry();
	void processReaction(c1Reaction &);
	void processEmitter(c1Emitter &);
	void processReceptor(c1Receptor &);
	
public:
	c1Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a);

	void tick();

	unsigned char getDrive(unsigned int id) { assert(id < 16); return drives[id]; }
	
	unsigned char *getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l);

	unsigned int getGait();
};

// c2

struct c2Reaction {
	bioReactionGene *data;
	float rate;
	unsigned int receptors;
	void init(bioReactionGene *);
};

struct c2Receptor {
	bioReceptorGene *data;
	bool processed;
	float lastvalue;
	float *locus;
	unsigned int *receptors;
	float nominal, threshold, gain;
	void init(bioReceptorGene *, class c2Organ *, shared_ptr<c2Reaction>);
};

struct c2Emitter {
	bioEmitterGene *data;
	unsigned char sampletick;
	float *locus;
	float threshold, gain;
	void init(bioEmitterGene *, class c2Organ *);
};


class c2Organ {
protected:
	class c2Creature *parent;
	organGene *ourGene;

	std::vector<shared_ptr<c2Reaction> > reactions;
	std::vector<c2Receptor> receptors;
	std::vector<c2Emitter> emitters;

	// data
	unsigned char energycost, atpdamagecoefficient;

	// variables
	float lifeforce, shorttermlifeforce, longtermlifeforce;
	
	// locuses
	unsigned char biotick, damagerate, repairrate, clockrate, injurytoapply;
	unsigned int clockratereceptors, repairratereceptors, injuryreceptors;

	void processReaction(c2Reaction &);
	void processEmitter(c2Emitter &);
	void processReceptor(c2Receptor &, bool checkchem);
	
	unsigned char *getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l, unsigned int **receptors);

public:
	c2Organ(c2Creature *p, organGene *g);
	void tick();

	void processGenes();

	unsigned char getClockRate() { return clockrate; }
	unsigned char getRepairRate() { return repairrate; }
	unsigned char getDamageRate() { return damagerate; }
	unsigned char getEnergyCost() { return energycost; }
	unsigned char getInjuryToApply() { return injurytoapply; }
	float getInitialLifeforce() { return lifeforce; }
	float getShortTermLifeforce() { return shorttermlifeforce; }
	float getLongTermLifeforce() { return longtermlifeforce; }
	unsigned char getATPDamageCoefficient() { return atpdamagecoefficient; }
	
	unsigned int getReceptorCount() { return receptors.size(); }
	unsigned int getEmitterCount() { return emitters.size(); }
	unsigned int getReactionCount() { return reactions.size(); }
	
	void applyInjury(float);

};

class c2Creature : public oldCreature {
protected:
	// biochemistry
	std::vector<shared_ptr<c2Organ> > organs;

	// loci
	unsigned char senses[14];
	unsigned char gaitloci[16];
	unsigned char drives[17];
	unsigned char mutationchance, mutationdegree;
	
	void addGene(gene *);
	void tickBiochemistry();
	void processGenes();

public:
	c2Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a);

	void tick();
	
	unsigned char getDrive(unsigned int id) { assert(id < 17); return drives[id]; }
	
	unsigned char *getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l);
	
	unsigned int getGait();
};

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
	void init(bioReceptorGene *, class c2eOrgan *, shared_ptr<c2eReaction>);
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

	std::vector<shared_ptr<c2eReaction> > reactions;
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
	std::vector<shared_ptr<c2eOrgan> > organs;
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
	c2eCreature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a);

	void tick();

	void adjustChemical(unsigned char id, float value);
	float getChemical(unsigned char id) { return chemicals[id]; }
	void adjustDrive(unsigned int id, float value);
	float getDrive(unsigned int id) { assert(id < 20); return drives[id]; }

	void setInvolActionLatency(unsigned int id, unsigned int n) { assert(id < 8); involactionlatency[id] = n; }

	void handleStimulus(c2eStim &stim);
	void handleStimulus(unsigned int id, float strength);

	unsigned int noOrgans() { return organs.size(); }
	shared_ptr<c2eOrgan> getOrgan(unsigned int i) { assert(i < organs.size()); return organs[i]; }
	
	class c2eBrain *getBrain() { return brain; }

	float *getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l);
	
	unsigned int getGait();
};

#endif

/* vim: set noet: */
