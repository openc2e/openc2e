/*
 *  Agent.cpp
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
#include "World.h"
#include <iostream>

Agent::Agent(unsigned char f, unsigned char g, unsigned short s, unsigned int p) :
  visible(true), family(f), genus(g), species(s), zorder(p), vm(this), timerrate(0) {
  velx.setFloat(0.0f);
  vely.setFloat(0.0f);
}

void Agent::moveTo(unsigned int _x, unsigned int _y) {
	x = _x; y = _y;
}

void Agent::fireScript(unsigned char event) {
	script &s = world.scriptorium.getScript(family, genus, species, event);
	if (s.lines.empty()) return;
	vm.fireScript(s, (event == 9));
	vm.targ = this;
	std::cout << "Agent::fireScript fired " << (unsigned int)family << " " << (unsigned int)genus << " " << species << " ";
	const std::string n = world.catalogue.getAgentName(family, genus, species);
	if (n.size())
		std::cout << "(" << n << ") ";
	std::cout << (unsigned int)event << std::endl;
}

void Agent::tick() {
	if (timerrate) {
		tickssincelasttimer++;
		if (tickssincelasttimer == timerrate) {
			fireScript(9);
			tickssincelasttimer = 0;
		}
	}

	vm.tick();
}
