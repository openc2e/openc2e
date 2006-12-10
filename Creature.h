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

#include "Agent.h"
#include "genome.h"
#include <boost/shared_ptr.hpp>

using boost::shared_ptr;

class CreatureAgent;
class Creature;

class Creature {
protected:
	CreatureAgent *parent;
	shared_ptr<genomeFile> genome;
	
	// non-specific bits
	unsigned int variant;
	bool female;
	
	bool alive, asleep, dreaming, tickage;
	bool zombie;

	unsigned int age; // in ticks
	lifestage stage;

	AgentRef attention, focus;

	// linguistic stuff

	// drives
	// to-be-processed instincts
	// conscious flag? brain/motor enabled flags? flags for each 'faculty'?
	
	void processGenes();
	virtual void addGene(gene *);

public:
	Creature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant);
	void setAgent(CreatureAgent *a);
	virtual ~Creature();
	virtual void tick();

	virtual void ageCreature();
	lifestage getStage() { return stage; }

	unsigned int getVariant() { return variant; }
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
	
	void born();
	void die();
};

// c2e

struct c2eReaction {
	bioReaction *data;
	float rate;
	unsigned int receptors;
	void init(bioReaction *);
};

struct c2eReceptor {
	bioReceptor *data;
	bool processed;
	float lastvalue;
	float *locus;
	unsigned int *receptors;
	float nominal, threshold, gain;
	void init(bioReceptor *, class c2eOrgan *, shared_ptr<c2eReaction>);
};

struct c2eEmitter {
	bioEmitter *data;
	unsigned char sampletick;
	float *locus;
	float threshold, gain;
	void init(bioEmitter *, class c2eOrgan *);
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

class c2eCreature : public Creature {
protected:
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

	bioHalfLives *halflives;
	unsigned int biochemticks;

	void tickBiochemistry();
	void addGene(gene *);

public:
	c2eCreature(shared_ptr<genomeFile> g, bool is_female, unsigned char _variant);

	void tick();

	void adjustChemical(unsigned char id, float value);
	float getChemical(unsigned char id) { return chemicals[id]; }
	void adjustDrive(unsigned int id, float value);
	float getDrive(unsigned int id) { return drives[id]; }

	unsigned int noOrgans() { return organs.size(); }
	shared_ptr<c2eOrgan> getOrgan(unsigned int i) { return organs[i]; }

	float *getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l);
};

/* vim: set noet: */
