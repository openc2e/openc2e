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
#include <sstream>

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
 %status stub

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
 %status stub

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

/**
 CALL (command) script_no (integer) p1 (any) p2 (any)

 <p>Calls script_no on OWNR, then waits for it to return. The invoked script
 will inherit the caller's INST setting, but any changes it makes to it will
 be reversed once it returns - so eg if you call a script when in INST mode,
 it calls OVEr and returns, you'll still be in INST.</p>
 
 <p>Script variables (VAxx) will not be preserved - you'll have to use OVxx
 for any parameters.</p>
 */
void caosVM::c_CALL() {
	VM_PARAM_VALUE(p2)
	VM_PARAM_VALUE(p1)
	VM_PARAM_INTEGER(script_no)

	caos_assert(owner);
	caos_assert(script_no > 0 && script_no < 65536);

	script *s = owner->findScript(script_no);
	if (!s) return;
	caosVM *newvm = world.getVM(owner);
	
	if (newvm->fireScript(s, false)) { // TODO: include OWNR? presumably not..
		newvm->setTarg(owner);
		newvm->inst = inst;
		newvm->_p_[0] = p1;
		newvm->_p_[1] = p2;
		owner->pushVM(newvm);
	} else
		world.freeVM(newvm);

	stop_loop = true;
}

/**
 CAOS (string) inline (integer) state_trans (integer) p1 (anything) p2 (anything) commands (string) throws (integer) catches (integer) report (variable)

 Run commands as caos code. If inline, copy _IT_ VAxx TARG OWNR, etc. If state_trans, copy FROM and OWNR. if !throws or catches, catch errors and stuff them in report (XXX: non-conforming)
*/

// XXX: exception catching is very broken right now

void caosVM::v_CAOS() {
	// XXX: capture output
	VM_PARAM_VARIABLE(report)
	VM_PARAM_INTEGER(catches)
	VM_PARAM_INTEGER(throws)
	VM_PARAM_STRING(commands)
	VM_PARAM_VALUE(p2)
	VM_PARAM_VALUE(p1)
	VM_PARAM_INTEGER(state_trans)
	VM_PARAM_INTEGER(inl)
	
	caosVM *sub = world.getVM(NULL);
	if (inl) {
		for (int i = 0; i < 100; i++)
			sub->var[i] = var[i];
		sub->targ = targ;
		sub->_it_ = _it_;
		sub->part = part;
		sub->owner = owner;
		// sub->from = from;
	}
	if (state_trans) {
		sub->owner = owner;
		// sub->from = from;
	}
	sub->_p_[0] = p1;
	sub->_p_[1] = p2;
	try {
		std::istringstream iss(commands);
		std::ostringstream oss;
		caosScript *s = new caosScript("c3", "CAOS command"); // XXX: variant
		s->parse(iss);
		s->installScripts();
		sub->outputstream = &oss;
		sub->runEntirely(s->installer);
		result.setString(oss.str());
	} catch (caosException &e) {
		if (!throws || catches) {
			report->setString(e.what());
			result.setString("###");
		} else {
			world.freeVM(sub);
			throw e;
		}
	}
	world.freeVM(sub);
}


/* vim: set noet: */
