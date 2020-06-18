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

#include "AgentRef.h"
#include "fileformats/genomeFile.h"

#include <cassert>
#include <deque>
#include <memory>

class Agent;
class CreatureAgent;

class Creature {
protected:
	CreatureAgent *parent;
	Agent *parentagent;
	std::shared_ptr<genomeFile> genome;

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
	// TODO: make pure virtual?
	virtual AgentRef selectRepresentativeAgent(int type, std::vector<AgentRef> possibles) {
		(void)type;
		(void)possibles;
		return AgentRef();
	}

	// linguistic stuff

	// to-be-processed instincts
	std::deque<creatureInstinctGene *> unprocessedinstincts;

	// conscious flag? brain/motor enabled flags? flags for each 'faculty'?
	
	unsigned short tintinfo[5]; // red, green, blue, rotation, swap

	virtual void processGenes();
	virtual void addGene(gene *);

	Creature(std::shared_ptr<genomeFile> g, bool is_female, unsigned char _variant, CreatureAgent *a);
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
	std::shared_ptr<genomeFile> getGenome() { return genome; }

	unsigned short getGenus() { return genus; }
	unsigned int getVariant() { return variant; }
	unsigned short getTint(unsigned int id) { return tintinfo[id]; }

	size_t getNoUnprocessedInstincts() { return unprocessedinstincts.size(); }

	size_t getNoCategories() { return chosenagents.size(); }
	AgentRef getChosenAgentForCategory(unsigned int cat) { assert(cat < chosenagents.size()); return chosenagents[cat]; }
	AgentRef getAttentionFocus() { return attention; }
	int getAttentionId() { return attn; }
	int getDecisionId() { return decn; }

	virtual unsigned int getGait() = 0;
	
	void born();
	void die();
	
	bool shouldProcessGene(gene *);
};

#endif

/* vim: set noet: */
