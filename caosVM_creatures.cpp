/*
 *  caosVM_creatures.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Jun 01 2004.
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
#include <iostream>
#include "openc2e.h"
using std::cerr;

/**
 STIM WRIT (command) creature (agent) stimulus (integer) strength (float)

 send stimulus to specific creature
*/
void caosVM::c_STIM_WRIT() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(strength)
	VM_PARAM_INTEGER(stimulus)
	VM_PARAM_AGENT(creature)
	
	// TODO
}

/**
 NOHH (command)

 tell target creature to stop holding hands with the pointer
*/
void caosVM::c_NOHH() {
	VM_VERIFY_SIZE(0)

	// TODO
}

/**
 ZOMB (command) zombie (integer)

 pass 1 to disconnect brain and motor of target creature, and 0 to undo
*/
void caosVM::c_ZOMB() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(zombie)

	// TODO
}

/**
 DIRN (command) direction (integer)

 change target creatures to face a different direction
*/
void caosVM::c_DIRN() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(zombie)

	// TODO
}

/**
 HHLD (agent)

 return creatures holding hands with pointer, or NULL if none
*/
void caosVM::v_HHLD() {
	VM_VERIFY_SIZE(0)

	result.setAgent(0);
}

/**
 MVFT (command) x (float) y (float)

 move target creature foot to (x, y)
*/
void caosVM::c_MVFT() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	caos_assert(targ);

	// TODO: dynamic_cast to Creature *
}
	
