/*
 *  Creature.h
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

#include "Agent.h"
#include "genome.h"

class Organ {
public:
};

class Brain {
public:
};

class Creature : public Agent {
protected:
	std::vector<Organ *> organs;
	Brain brain;

	// biochemistry
	float chemicals[256];

	// non-specific bits
	unsigned int variant;
	bool female;
	genomeFile *genome;
	
	bool alive, asleep, dreaming;

	bool zombie;

	unsigned int age; // in ticks
	lifestage stage;

	Agent *attention, *focus;

	// clothes
	// linguistic stuff

	// drives
	// to-be-processed instincts
	// conscious flag? brain/motor enabled flags? flags for each 'faculty'?

public:
	Creature(genomeFile *g, unsigned char _family, bool is_female, unsigned char _variant);
};

/* vim: set noet: */
