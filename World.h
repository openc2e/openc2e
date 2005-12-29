/*
 *  World.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#include "Map.h"
#include "Scriptorium.h"
#include "Catalogue.h"
#include "Camera.h"
#include "Agent.h"
#include "renderable.h"
#include <set>
#include <map>

struct cainfo {
	float gain;
	float loss;
	float diffusion;
};

struct scriptevent {
	unsigned short scriptno;
	AgentRef agent, from;
	caosVar p[2];
};

class World {
protected:
	class PointerAgent *theHand;
	std::vector<scriptevent> scriptqueue;

public:

	bool quitting, saving;
	
	Map map;

	std::multiset<Agent *, agentzorder> zorder;
	std::multiset<renderable *, renderablezorder> renders;
	std::list<Agent *> agents;
	
	std::map<unsigned int, std::map<unsigned int, cainfo> > carates;
	std::map<std::string, caosVar> variables;
	std::map<caosVar, caosVar> eame_variables; // non-serialised
	std::vector<caosVM *> vmpool;
	std::vector<Agent *> killqueue;
	std::map<int, Agent *> unidmap;
	
	Scriptorium scriptorium;
	Catalogue catalogue;
	float pace;
	unsigned int ticktime, tickcount;
	MainCamera camera;

	AgentRef focusagent; unsigned int focuspart;
	void setFocus(class CompoundAgent *a, class TextEntryPart *p);

	Agent *agentAt(unsigned int x, unsigned int y, bool needs_mouseable = false);
	class PointerAgent *hand() { return theHand; }
	
	caosVM *getVM(Agent *owner);
	void freeVM(caosVM *);
	void queueScript(unsigned short event, AgentRef agent, AgentRef from = AgentRef(), caosVar p0 = caosVar(), caosVar p1 = caosVar());
	
	World();
	void init();

	void tick();

	int World::getUNID(Agent *whofor);
	void World::freeUNID(int unid);

	Agent *lookupUNID(int unid);
};

extern World world;
/* vim: set noet: */
