/*
 *  caosVM_flow.cpp
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
#include <iostream>
#include "openc2e.h"
#include "World.h" // enum
#include <cmath>   // sqrt

/**
 DOIF (command) condition (condition)
 %pragma parser new DoifParser()
 
 Part of a DOIF/ELIF/ELSE/ENDI block. Jump to the next part of the block if condition is false, otherwise continue executing.
*/

/**
 ELIF (command) condition (condition)
 %pragma noparse
 
 Part of a DOIF/ELIF/ELSE/ENDI block. If none of the previous DOIF/ELIF conditions have been true, and condition evaluates to true, then the code in the ELIF block is executed.
*/


/**
 ELSE (command)
 %pragma noparse
 
 Part of a DOIF/ELIF/ELSE/ENDI block. If ELSE is present, it is jumped to when none of the previous DOIF/ELIF conditions are true.
*/

/**
 ENDI (command)
 %pragma noparse
 
 The end of a DOIF/ELIF/ELSE/ENDI block.
*/

/**
 REPS (command) reps (integer)
 %pragma parser new parseREPS()

 The start of a REPS...REPE loop. The body of the loop will be executed (reps)
 times.
*/

/**
 REPE (command)
 %pragma noparse

 The end of a REPS...REPE loop.
*/

/**
 LOOP (command)
 %pragma parser new parseLOOP()
 
 The start of a LOOP..EVER or LOOP..UNTL loop.
*/

/**
 EVER (command)
 %pragma noparse
 
 Jump back to the matching LOOP, no matter what.
*/

/**
 UNTL (command) condition (condition)
 %pragma noparse
 
 Jump back to the matching LOOP unless the condition evaluates to true.
*/

/**
 GSUB (command) label (label)
 %pragma parser new parseGSUB()
 %pragma retc -1
 
 Jump to a subroutine defined by SUBR with label 'label'.
*/

/**
 SUBR (command) label (label)
 %pragma parser new parseSUBR()
 
 Define the start of a subroute to be called with GSUB, with label 'label'.
 If the command is encountered during execution, it acts like a STOP.
*/

/**
 RETN (command)
 %pragma retc -1
 
 Return from a subroutine called with GSUB.
*/
void caosVM::c_RETN() {
	if (callStack.empty())
		throw creaturesException("RETN with an empty callstack");
	nip = callStack.back().nip;
	valueStack = callStack.back().valueStack;
	callStack.pop_back();
}

/**
 NEXT (command)
 %pragma noparse

*/

/**
 ENUM (command) family (integer) genus (integer) species (integer)
 %pragma parserclass ENUMhelper
 %pragma retc -1

 Loop through all agents with the given classifier. 0 on any field is a
 wildcard. The loop body is terminated by a NEXT.
*/
void caosVM::c_ENUM() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) assert(species >= 0); assert(species <= 65535);
	VM_PARAM_INTEGER(genus) assert(genus >= 0); assert(genus <= 255);
	VM_PARAM_INTEGER(family) assert(family >= 0); assert(family <= 255);

	caosVar nullv;
	nullv.reset();
	valueStack.push_back(nullv);
	
	for (std::multiset<Agent *, agentzorder>::iterator i
			= world.agents.begin(); i != world.agents.end(); i++) {
		Agent *a = (*i);
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		caosVar v; v.setAgent(a);
		valueStack.push_back(v);
	}
}

/**
 ESEE (command) family (integer) genus (integer) species (integer)
 %pragma parserclass ENUMhelper
 %pragma retc -1
 
 like ENUM, but iterate through agents OWNR can see (todo: document exact rules)
*/
void caosVM::c_ESEE() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) assert(species >= 0); assert(species <= 65535);
	VM_PARAM_INTEGER(genus) assert(genus >= 0); assert(genus <= 255);
	VM_PARAM_INTEGER(family) assert(family >= 0); assert(family <= 255);
	
	caosVar nullv;
	nullv.reset();
	valueStack.push_back(nullv);
	
	for (std::multiset<Agent *, agentzorder>::iterator i
			= world.agents.begin(); i != world.agents.end(); i++) {
		Agent *a = (*i);
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		// XXX: measure from center?
		double deltax = (*i)->x - owner->x;
		double deltay = (*i)->y - owner->y;
		deltax *= deltax;
		deltay *= deltay;

		double distance = sqrt(deltax + deltay);
		if (distance > owner->range) continue;

		caosVar v; v.setAgent(a);
		valueStack.push_back(v);
	}
}

/**
 ETCH (command) family (integer) genus (integer) species (integer)
 %pragma parserclass ENUMhelper
 %pragma retc -1

 like ENUM, but iterate through agents OWNR is touching
*/
void caosVM::c_ETCH() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) assert(species >= 0); assert(species <= 65535);
	VM_PARAM_INTEGER(genus) assert(genus >= 0); assert(genus <= 255);
	VM_PARAM_INTEGER(family) assert(family >= 0); assert(family <= 255);

	// TODO: should probably implement this (ESEE)
	
	caosVar nullv; nullv.reset();
	valueStack.push_back(nullv);
}

/**
 EPAS (command) family (integer) genus (integer) species (integer)
 %pragma parserclass ENUMhelper
 %pragma retc -1

 like ENUM, but iterate through OWNR vehicle's passengers
*/
void caosVM::c_EPAS() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) assert(species >= 0); assert(species <= 65535);
	VM_PARAM_INTEGER(genus) assert(genus >= 0); assert(genus <= 255);
	VM_PARAM_INTEGER(family) assert(family >= 0); assert(family <= 255);

	// TODO: should probably implement this (ESEE)

	caosVar nullv; nullv.reset();
	valueStack.push_back(nullv);
}

/* vim: set noet: */
