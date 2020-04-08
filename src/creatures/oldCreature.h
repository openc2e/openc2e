/*
 *  oldCreature.h
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

#ifndef __OLDCREATURE_H
#define __OLDCREATURE_H

#include "Creature.h"

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
	unsigned int involactionlatency[8];
	
	void addGene(gene *);
	void tickBrain();
	virtual void tickBiochemistry();

	oldCreature(std::shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a);

	void processGenes();

	AgentRef selectRepresentativeAgent(int type, std::vector<AgentRef> possibles);

public:
	// TODO: inline?
	unsigned int calculateTickMask(unsigned char);
	unsigned int calculateMultiplier(unsigned char);

	void addChemical(unsigned char id, unsigned char val);
	void subChemical(unsigned char id, unsigned char val);
	unsigned char getChemical(unsigned char id) { return chemicals[id]; }	
	
	// TODO: is it really worth having drives outside oldCreature?
	virtual unsigned char getDrive(unsigned int id) = 0;
	
	void handleStimulus(unsigned int id);

	oldBrain *getBrain() { return brain; }
	
	unsigned char *getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l);
	
	void setInvolActionLatency(unsigned int id, unsigned int n) { assert(id < 8); involactionlatency[id] = n; }
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
	std::vector<std::shared_ptr<c1Reaction> > reactions;
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
	c1Creature(std::shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a);

	void tick();

	unsigned char getDrive(unsigned int id) { assert(id < 16); return drives[id]; }
	
	unsigned char *getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l);

	unsigned int getGait();
};

// c2

struct c2Reaction {
	bioReactionGene *data;
	void init(bioReactionGene *);
};

struct c2Receptor {
	bioReceptorGene *data;
	bool processed;
	unsigned char lastvalue;
	unsigned char *locus;
	unsigned int *receptors;
	void init(bioReceptorGene *, class c2Organ *);
};

struct c2Emitter {
	bioEmitterGene *data;
	unsigned char *locus;
	void init(bioEmitterGene *, class c2Organ *);
};

class c2Organ {
protected:
	friend struct c2Receptor;
	friend struct c2Emitter;

	class c2Creature *parent;
	organGene *ourGene;

	std::vector<std::shared_ptr<c2Reaction> > reactions;
	std::vector<c2Receptor> receptors;
	std::vector<c2Emitter> emitters;

	// data
	unsigned char energycost, atpdamagecoefficient;

	// variables
	float lifeforce, shorttermlifeforce, longtermlifeforce;
	
	// locuses
	unsigned char damagerate, repairrate, clockrate, injurytoapply;
	unsigned int clockratereceptors, repairratereceptors, injuryreceptors;

	unsigned int biotick, biochemticks;

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
	std::vector<std::shared_ptr<c2Organ> > organs;

	// loci
	unsigned char senses[14];
	unsigned char gaitloci[16];
	unsigned char drives[17];
	unsigned char mutationchance, mutationdegree;
	
	void addGene(gene *);
	void tickBiochemistry();
	void processGenes();

public:
	c2Creature(std::shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a);

	void tick();
	
	unsigned char getDrive(unsigned int id) { assert(id < 17); return drives[id]; }
	
	unsigned char *getLocusPointer(bool receptor, unsigned char o, unsigned char t, unsigned char l);
	
	unsigned int getGait();
};

#endif

/* vim: set noet: */
