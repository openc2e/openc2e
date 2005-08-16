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
#include "Creature.h"
using std::cerr;

/**
 STIM SHOU (command) stimulus (integer) strength (float)
 %status stub

 send stimulus to all creatures who can hear OWNR
*/
void caosVM::c_STIM_SHOU() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(strength)
	VM_PARAM_INTEGER(stimulus)
	
	// TODO
}

/**
 STIM SIGN (command) stimulus (integer) strength (float)
 %status stub

 send stimulus to all creatures who can see OWNR
*/
void caosVM::c_STIM_SIGN() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(strength)
	VM_PARAM_INTEGER(stimulus)
	
	// TODO
}

/**
 STIM TACT (command) stimulus (integer) strength (float)
 %status stub

 send stimulus to all creatures who are touching OWNR
*/
void caosVM::c_STIM_TACT() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(strength)
	VM_PARAM_INTEGER(stimulus)
	
	// TODO
}

/**
 STIM WRIT (command) creature (agent) stimulus (integer) strength (float)
 %status stub

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
 SWAY SHOU (command) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) drive (integer) adjust (float) 
 %status stub

 Adjust these four drives in all creatures which can hear OWNR
*/
void caosVM::c_SWAY_SHOU() {
	VM_VERIFY_SIZE(8)
	VM_PARAM_FLOAT(adjust4)
	VM_PARAM_INTEGER(drive4)
	VM_PARAM_FLOAT(adjust3)
	VM_PARAM_INTEGER(drive3)
	VM_PARAM_FLOAT(adjust2)
	VM_PARAM_INTEGER(drive2)
	VM_PARAM_FLOAT(adjust1)
	VM_PARAM_INTEGER(drive1)

	caos_assert(owner);
	//TODO
}

/**
 ASLP (command) sleeping (integer)
 %status stub

 Set whether the creature in TARG is asleep
*/
void caosVM::c_ASLP() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(asleep)

	caos_assert(targ);

	//TODO
}

/**
 NOHH (command)
 %status stub

 tell target creature to stop holding hands with the pointer
*/
void caosVM::c_NOHH() {
	VM_VERIFY_SIZE(0)

	// TODO
}

/**
 ZOMB (command) zombie (integer)
 %status stub

 pass 1 to disconnect brain and motor of target creature, and 0 to undo
*/
void caosVM::c_ZOMB() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(zombie)

	// TODO
}

/**
 ZOMB (integer)
 %status stub

 return 1 if target creature is zombified, or 0 otherwise
*/
void caosVM::v_ZOMB() {
	result.setInt(0); // TODO
}

/**
 DIRN (command) direction (integer)
 %status stub

 change target creature to face a different direction
*/
void caosVM::c_DIRN() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(zombie)

	// TODO
}

/**
 HHLD (agent)
 %status stub

 return creatures holding hands with pointer, or NULL if none
*/
void caosVM::v_HHLD() {
	VM_VERIFY_SIZE(0)

	result.setAgent(0); // TODO
}

/**
 MVFT (command) x (float) y (float)
 %status stub

 move target creature foot to (x, y)
*/
void caosVM::c_MVFT() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	caos_assert(targ);

	// TODO: dynamic_cast to Creature *
}
	
/**
 CHEM (command) chemical (integer) adjustment (float)
 %status stub
*/
void caosVM::c_CHEM() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(adjustment)
	VM_PARAM_INTEGER(chemical)

	caos_assert(targ);

	// TODO
}

/**
 CREA (integer) agent (agent)
 %status done

 return 1 if agent is a creature, or 0 otherwise
*/
void caosVM::v_CREA() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(agent)

	Creature *c = dynamic_cast<Creature *>(agent);
	if (c) result.setInt(1);
	else result.setInt(0);
}

/**
 VOCB (command)
 %status stub

 make target creature learn all vocabulary immediately
*/
void caosVM::c_VOCB() {
	VM_VERIFY_SIZE(0)
	
	caos_assert(targ);
	Creature *c = dynamic_cast<Creature *>(targ.get());
	caos_assert(c);

	// TODO
}

/**
 DEAD (command)
 %status stub

 kill target creature biologically
*/
void caosVM::c_DEAD() {
	caos_assert(targ);
	Creature *c = dynamic_cast<Creature *>(targ.get());
	caos_assert(c);

	// TODO
}

/*
 DEAD (integer)
 %status stub

 return 1 if target creature is dead, or 0 otherwise
*/
void caosVM::v_DEAD() {
	caos_assert(targ);
	Creature *c = dynamic_cast<Creature *>(targ.get());
	caos_assert(c);

	result.setInt(0); // TODO
}

/*
 NORN (command) creature (agent)
 %status stub
*/
void caosVM::c_NORN() {
	VM_PARAM_AGENT(creature)

	// TODO
}

/*
 NORN (agent)
 %status stub
*/
void caosVM::v_NORN() {
	result.setAgent(0); // TODO
}

/* vim: set noet: */
