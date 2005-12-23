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

Creature *caosVM::getTargCreature() {
	caos_assert(targ);
	Creature *c = dynamic_cast<Creature *>(targ.get());
	caos_assert(c);
	return c;
}

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
 NOHH (command)
 %status stub

 tell target creature to stop holding hands with the pointer
*/
void caosVM::c_NOHH() {
	VM_VERIFY_SIZE(0)

	Creature *c = getTargCreature();
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

	Creature *c = getTargCreature();

	// TODO
}

/**
 ZOMB (integer)
 %status stub

 return 1 if target creature is zombified, or 0 otherwise
*/
void caosVM::v_ZOMB() {
	Creature *c = getTargCreature();
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

	Creature *c = getTargCreature();
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

	Creature *c = getTargCreature();

	// TODO: dynamic_cast to Creature *
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
	
	Creature *c = getTargCreature();

	// TODO
}

/**
 DEAD (command)
 %status stub

 kill target creature biologically
*/
void caosVM::c_DEAD() {
	Creature *c = getTargCreature();

	// TODO
}

/**
 DEAD (integer)
 %status stub

 return 1 if target creature is dead, or 0 otherwise
*/
void caosVM::v_DEAD() {
	Creature *c = getTargCreature();

	result.setInt(0); // TODO
}

/**
 NORN (command) creature (agent)
 %status stub
*/
void caosVM::c_NORN() {
	VM_PARAM_AGENT(creature)

	// TODO
}

/**
 NORN (agent)
 %status stub
*/
void caosVM::v_NORN() {
	result.setAgent(0); // TODO
}

/**
 URGE SIGN (command) noun_stim (float) verb_id (integer) verb_stim (float)
 %status stub

 Urge all creatures who can see OWNR to perform the given action on OWNR.
 The two stimuli parameters can range from -1.0 (discourage) to 1.0 (encourage).
*/
void caosVM::c_URGE_SIGN() {
	VM_PARAM_FLOAT(verb_stim)
	VM_PARAM_INTEGER(verb_id)
	VM_PARAM_FLOAT(noun_stim)

	// TODO
}

/**
 URGE WRIT (command) creature (agent) noun_id (integer) noun_stim (float) verb_id (integer) verb_stim (float)
 %status stub

 Urge the specified creature to perform the specified action (verb) on the specified object type (noun).
 Provide a stim greater than 1 to force, and an id of -1 and a stim greater than 1 to unforce.
*/
void caosVM::c_URGE_WRIT() {
	VM_PARAM_FLOAT(verb_stim)
	VM_PARAM_INTEGER(verb_id)
	VM_PARAM_FLOAT(noun_stim)
	VM_PARAM_INTEGER(noun_id)
	VM_PARAM_VALIDAGENT(creature)

	// TODO
}

/**
 DRIV (command) drive_id (integer) adjust (float)
 %status stub

 Modifies the level of a drive in target creature by adjust, which can range from -1.0 (decrease) to 1.0 (increase).
*/
void caosVM::c_DRIV() {
	VM_PARAM_FLOAT(adjust)
	VM_PARAM_INTEGER(drive_id)

	Creature *c = getTargCreature();

	// TODO
}

/**
 DRIV (float) drive_id (integer)
 %status stub

 Returns the level of a drive (0.0 to 1.0) in target creature.
*/
void caosVM::v_DRIV() {
	VM_PARAM_INTEGER(drive_id)

	Creature *c = getTargCreature();

	result.setFloat(0.0f); // TODO
}

/**
 CHEM (command) chemical_id (integer) adjust (float)
 %status stub

 Modifies the level of a chemical in target creature's bloodstream by adjust, which can range from -1.0 (decrease) to 1.0 (increase).
*/
void caosVM::c_CHEM() {
	VM_PARAM_FLOAT(adjust)
	VM_PARAM_INTEGER(chemical_id)

	Creature *c = getTargCreature();

	// TODO
}

/**
 CHEM (float) chemical_id (integer)
 %status stub

 Returns the level of a chemical (0.0 to 1.0) in target creature's bloodstream.
*/
void caosVM::v_CHEM() {
	VM_PARAM_INTEGER(chemical_id)
	
	Creature *c = getTargCreature();

	result.setFloat(0.0f); // TODO
}

/**
 ASLP (command) asleep (integer)
 %status stub

 If asleep is 1, make target creature sleep. If asleep is 0, make target creature wake.
*/
void caosVM::c_ASLP() {
	VM_PARAM_INTEGER(asleep)
	
	Creature *c = getTargCreature();

	// TODO
}

/**
 ASLP (integer)
 %status stub

 Returns 1 if target creature is asleep, or 0 otherwise.
*/
void caosVM::v_ASLP() {
	Creature *c = getTargCreature();

	result.setInt(0); // TODO
}

/**
 APPR (command)
 %status stub

 Make target creature approach the IT agent (or if none, an agent of that category using CAs), blocking until it makes it there or gives up.
*/
void caosVM::c_APPR() {
	Creature *c = getTargCreature();

	// TODO
}

/**
 UNCS (command) unconscious (integer)
 %status stub

 Make creature conscious if 0, or unconscious if 1.
*/
void caosVM::c_UNCS() {
	Creature *c = getTargCreature();

	// TODO
}

/**
 UNCS (integer)
 %status stub

 Return 1 if target creature is unconscious, or 0 otherwise.
*/
void caosVM::v_UNCS() {
	Creature *c = getTargCreature();

	result.setInt(0); // TODO
}

/* TODO: there's a string variation of this, which we have no way of handling right now. fixit! */
/**
 FACE (integer)
 %status stub

 Return the front-facing pose for the current facial expression of the target creature.
*/
void caosVM::v_FACE() {
	Creature *c = getTargCreature();

	result.setInt(0); // TODO
}

/**
 LIKE (command) creature (agent)
 %status stub
 
 Cause target creature to state an opinion about the specified creature.
*/
void caosVM::c_LIKE() {
	VM_PARAM_VALIDAGENT(creature)

	Creature *c = getTargCreature();
}

/**
 LIMB (string) bodypart (integer) genus (integer) gender (integer) age (integer) variant (integer)
 %status stub
 
 Returns the filename for the specified part of a creature, substituting as necessary.
*/
void caosVM::v_LIMB() {
	VM_PARAM_INTEGER(variant)
	VM_PARAM_INTEGER(age)
	VM_PARAM_INTEGER(gender)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(bodypart)
	
	Creature *c = getTargCreature();
	
	result.setString(""); // TODO
}

/**
 ORDR SHOU (command) speech (string)
 %status stub

 Makes target speak specified speech to all creatures in hearing range.
*/
void caosVM::c_ORDR_SHOU() {
	VM_PARAM_STRING(speech)

	caos_assert(targ);
	// TODO
}

/* vim: set noet: */
