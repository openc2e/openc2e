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
#include <list>
#include <map>

struct cainfo {
	float gain;
	float loss;
	float diffusion;
};

class World {
public:
	Map map;
	std::list<Agent *> agents;
	std::map<unsigned int, std::map<unsigned int, cainfo> > carates;

	void addAgent(Agent *a);
	
	World();
};

extern World world;
