/*
 *  caosVM_agent.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun May 30 2004.
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

#include "caosVM.h"
#include "openc2e.h"
#include "CompoundAgent.h"
#include "SimpleAgent.h"
#include "World.h"
#include <iostream>
using std::cerr;

/*
 RTAR (command) family (integer) genus (integer) species (integer)

 set targ to random agent with given family/genus/species
 */ 
void caosVM::c_RTAR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	targ.reset();
	// todo
}

/*
 NEW: SIMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)

 create a new simple object with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, at the screen depth given by plane
*/
void caosVM::c_NEW_SIMP() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	SimpleAgent *a = new SimpleAgent(family, genus, species, plane, first_image, image_count);
	a->setImage(sprite_file);
	world.addAgent(a);
	targ.reset();
	targ.setAgent(a);
}

void caosVM::c_NEW_COMP() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	CompoundAgent *a = new CompoundAgent(family, genus, species, plane, first_image, image_count);
	a->setImage(sprite_file);
	world.addAgent(a);
	targ.reset();
	targ.setAgent(a);
}

void caosVM::v_TARG() {
	VM_VERIFY_SIZE(0)
	result = targ;
	result.setVariable(&targ);
}

void caosVM::v_NULL() {
	VM_VERIFY_SIZE(0)
	result.setAgent(0);
}

void caosVM::c_POSE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pose)
	cerr << "unimplemented: POSE\n";
}

void caosVM::c_ATTR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(attr)
	assert(targ.hasAgent());
	targ.agentValue->setAttributes(attr);
}

void caosVM::c_TICK() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(tickrate)
	cerr << "unimplemented: TICK\n";
}

void caosVM::c_BHVR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(bhvr)
	cerr << "unimplemented: BHVR\n";
}

void caosVM::c_TARG() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(a)
	targ.reset();
	targ.setAgent(a);
}

void caosVM::v_FROM() {
	VM_VERIFY_SIZE(0)
	cerr << "unimplemented: FROM\n";
}

void caosVM::v_POSE() {
	VM_VERIFY_SIZE(0)
	cerr << "unimplemented: POSE\n";
}

void caosVM::c_KILL() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(a)
	cerr << "unimplemented: KILL\n";
}

void caosVM::c_NEXT() {
	VM_VERIFY_SIZE(0)
	cerr << "unimplemented: NEXT\n";
}

void caosVM::c_SCRX() {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(family)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(event)
	cerr << "unimplemented: SCRX\n";
}

void caosVM::c_ANIM() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(poselist)

	// todo: compound agent stuff
	assert(targ.hasAgent());

	// todo: !!
	SimpleAgent *a = (SimpleAgent *)targ.agentValue;
	a->animation.clear();

	std::string oh;
	for (unsigned int i = 0; i < poselist.size(); i++) {
		if (poselist[i] != ' ') {
			if (!isdigit(poselist[i])) throw badParamException();
			oh += poselist[i];
		} else {
			unsigned int j = (unsigned int)atoi(oh.c_str());
			a->animation.push_back(j);
			oh.clear();
		}
	}
	if (!oh.empty()) {
		unsigned int j = (unsigned int)atoi(oh.c_str());
		a->animation.push_back(j);
	}
	if (!a->animation.empty()) { a->setFrameNo(0); }
	if (a->animation.empty()) { std::cout << "warning: ANIM produced an empty animation string\n"; }
}
