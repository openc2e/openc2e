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
#include "Agent.h"
#include "Scriptorium.h"
#include "Catalogue.h"
#include <set>
#include <map>

struct cainfo {
	float gain;
	float loss;
	float diffusion;
};

class World {
protected:
	Agent *theHand;

public:
	Map map;
	std::multiset<Agent *, agentzorder> agents;
	std::map<unsigned int, std::map<unsigned int, cainfo> > carates;

	Scriptorium scriptorium;
	Catalogue catalogue;

	void addAgent(Agent *a);
	Agent *agentAt(unsigned int x, unsigned int y, bool needs_mouseable = false);
	Agent *hand() { return theHand; }
	
	World();
	void init();

	void tick();
};

extern World world;
