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
#include "Vehicle.h"
#include "World.h"
#include <iostream>
using std::cerr;

/**
 RTAR (command) family (integer) genus (integer) species (integer)

 set targ to random agent with given family/genus/species
 */ 
void caosVM::c_RTAR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	targ = 0;
	// todo
}

/**
 NEW: SIMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)

 create a new simple agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, at the screen depth given by plane
*/
void caosVM::c_NEW_SIMP() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species) assert(species >= 0); assert(species <= 255);
	VM_PARAM_INTEGER(genus) assert(genus >= 0); assert(genus <= 255);
	VM_PARAM_INTEGER(family) assert(family >= 0); assert(family <= 65535);

	SimpleAgent *a = new SimpleAgent(family, genus, species, plane, first_image, image_count);
	a->setImage(sprite_file);
	world.addAgent(a);
	targ = a;
}

/**
 NEW: COMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)

 create a new composite agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane
*/
void caosVM::c_NEW_COMP() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species) assert(species >= 0); assert(species <= 255);
	VM_PARAM_INTEGER(genus) assert(genus >= 0); assert(genus <= 255);
	VM_PARAM_INTEGER(family) assert(family >= 0); assert(family <= 65535);

	CompoundAgent *a = new CompoundAgent(family, genus, species, plane, sprite_file, first_image, image_count);
	world.addAgent(a);
	targ = a;
}

/**
 NEW: VHCL (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)

 create a new vehicle agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane
*/
void caosVM::c_NEW_VHCL() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species) assert(species >= 0); assert(species <= 255);
	VM_PARAM_INTEGER(genus) assert(genus >= 0); assert(genus <= 255);
	VM_PARAM_INTEGER(family) assert(family >= 0); assert(family <= 65535);

	Vehicle *a = new Vehicle(family, genus, species, plane, sprite_file, first_image, image_count);
	world.addAgent(a);
	targ = a;
}

/**
 TARG (agent)

 return TARG
*/
void caosVM::v_TARG() {
	VM_VERIFY_SIZE(0)
	result.setAgent(targ);
}

/**
 OWNR (agent)
 
 return OWNR
*/
void caosVM::v_OWNR() {
	VM_VERIFY_SIZE(0)
	result.setAgent(owner);
}

/**
 NULL (agent)

 return null (zero) agent
*/
void caosVM::v_NULL() {
	VM_VERIFY_SIZE(0)
	result.setAgent(0);
}

/**
 POSE (command) pose (integer)
*/
void caosVM::c_POSE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pose)
	// TODO
}

/**
 ATTR (command) attr (integer)

 set attributes of TARG agent
*/
void caosVM::c_ATTR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(attr)
	assert(targ);
	targ->setAttributes(attr);
}

/**
 ATTR (integer)

 return attributes of TARG agent
*/
void caosVM::v_ATTR() {
	VM_VERIFY_SIZE(0)
	assert(targ);
	result.setInt(targ->getAttributes());
}

/**
 TICK (command) tickrate (integer)
*/
void caosVM::c_TICK() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(tickrate)
	assert(targ);
	targ->setTimerRate(tickrate);
}

/**
 BHVR (command) bhvr (integer)
*/
void caosVM::c_BHVR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(bhvr)
	// TODO
}

/**
 TARG (command) agent (agent)

 set TARG to given agent
*/
void caosVM::c_TARG() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(a)
	targ = a;
}

/**
 FROM (agent)
*/
void caosVM::v_FROM() {
	VM_VERIFY_SIZE(0)
	result.setAgent(0);
	// TODO
}

/**
 POSE (integer)
*/
void caosVM::v_POSE() {
	VM_VERIFY_SIZE(0)
	// TODO
}

/**
 KILL (command) agent (agent)

 destroy the agent in question. you can't destroy PNTR.
 remember, use DEAD first for creatures!
*/
void caosVM::c_KILL() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(a)
	// TODO: KILL
}

/**
 ANIM (command) poselist (byte-string)

 set the animation string for TARG, in the format '1 2 3 4'
 if it ends with '255', loop back to beginning; if it ends with '255 X', loop back to frame X

 <i>todo: compound agent stuff</i>
*/
void caosVM::c_ANIM() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(poselist)

	// todo: compound agent stuff
	assert(targ);

	// todo: !!	
	if (typeid(*targ) != typeid(SimpleAgent)) return;
 	SimpleAgent *a = (SimpleAgent *)targ;
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
}

/**
 ABBA (integer)
*/
void caosVM::v_ABBA() {
	VM_VERIFY_SIZE(0)
	// TODO
}

/**
 BASE (command) index (integer)
*/
void caosVM::c_BASE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(index)
	// TODO
}

/**
 BASE (integer)
*/
void caosVM::v_BASE() {
	VM_VERIFY_SIZE(0)
	// TODO
}

/**
 BHVR (integer)
*/
void caosVM::v_BHVR() {
	VM_VERIFY_SIZE(0)
	// TODO
}

/**
 CARR (agent)
*/
void caosVM::v_CARR() {
	VM_VERIFY_SIZE(0)
	// TODO
	result.setAgent(0);
}

/**
 FMLY (integer)

 return family of TARG agent
*/
void caosVM::v_FMLY() {
	VM_VERIFY_SIZE(0)
	assert(targ);
	result.setInt(targ->family);
}

/**
 GNUS (integer)

 return genus of TARG agent
*/
void caosVM::v_GNUS() {
	VM_VERIFY_SIZE(0)
	assert(targ);
	result.setInt(targ->genus);
}

/**
 SPCS (integer)

 return species of TARG agent
*/
void caosVM::v_SPCS() {
	VM_VERIFY_SIZE(0)
	assert(targ);
	result.setInt(targ->species);
}

/**
 PLNE (integer)

 return plane (z-order) of TARG agent
*/
void caosVM::v_PLNE() {
	VM_VERIFY_SIZE(0)
	assert(targ);
	result.setInt(targ->zorder);
}

/**
 PNTR (agent)

 return the pointer agent (the hand)
*/
void caosVM::v_PNTR() {
	VM_VERIFY_SIZE(0)
	result.setAgent(world.hand());
}

/**
 MESG WRIT (command) agent (agent) message_id (integer)
*/
void caosVM::c_MESG_WRIT() {
	VM_VERIFY_SIZE(2)
}

/**
 MESG WRT+ (command) agent (agent) message_id (integer) param_1 (anything) param_2 (anything) delay (integer)
*/
void caosVM::c_MESG_WRT() {
	VM_VERIFY_SIZE(5)
}

/**
 TOTL (integer) family (integer) genus (integer) species (integer)
*/
void caosVM::v_TOTL() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) assert(species >= 0); assert(species <= 255);
	VM_PARAM_INTEGER(genus) assert(genus >= 0); assert(genus <= 255);
	VM_PARAM_INTEGER(family) assert(family >= 0); assert(family <= 65535);

	unsigned int x = 0;
	for (std::multiset<Agent *, agentzorder>::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if ((*i)->family == family)
			if ((*i)->genus == genus)
				if ((*i)->species == species)
					x++;
	}
	result.setInt(x);
}

/**
 SHOW (command) visibility (integer)
 
 set visibility of agent to cameras. 0 = invisible, 1 = visible.
*/
void caosVM::c_SHOW() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(visibility)
	assert((visibility == 0) || (visibility == 1));
	assert(targ);
	targ->visible = visibility;
}

/**
 POSX (float)
*/
void caosVM::v_POSX() {
	VM_VERIFY_SIZE(0)
	assert(targ);
	result.setFloat(targ->x);
}

/**
 POSY (float)
*/
void caosVM::v_POSY() {
	VM_VERIFY_SIZE(0)
	assert(targ);
	result.setFloat(targ->y);
}
