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
#include "AgentHelpers.h"
#include "Vehicle.h" // EPAS
#include "caosScript.h" // CAOS

/**
 DOIF (command) condition (condition)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %pragma stackdelta 0
 %cost c1,c2 0
 
 Part of a DOIF/ELIF/ELSE/ENDI block. Jumps to the next part of the block if condition is false, 
 otherwise continues executing the script.
*/
void caosVM::c_DOIF() {
	// handled elsewhere
}

/**
 ELIF (command) condition (condition)
 %pragma variants all
 %status maybe
 %pragma stackdelta 0
 %cost c1,c2 0
 
 Part of a DOIF/ELIF/ELSE/ENDI block. If none of the previous DOIF/ELIF conditions have been true, and condition evaluates to true, then the code in the ELIF block is executed.
 If found outside a DOIF block, it is equivalent to a DOIF. If you take advantage of this behavior, fuzzie is of the opinion that you should be shot.
*/
void caosVM::c_ELIF() {
	// handled elsewhere
}


/**
 ELSE (command)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 Part of a DOIF/ELIF/ELSE/ENDI block. If ELSE is present, it is jumped to when none of the previous DOIF/ELIF conditions are true.
*/
void caosVM::c_ELSE() {
	// handled elsewhere
}

/**
 ENDI (command)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 The end of a DOIF/ELIF/ELSE/ENDI block.
*/
void caosVM::c_ENDI() {
	// TODO: cost in c2e?
}

/**
 REPS (command) reps (integer)
 %status maybe
 %pragma stackdelta 0
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0

 The start of a REPS...REPE loop. The body of the loop will be executed (reps) times.
*/
void caosVM::c_REPS() {
	// Our expression parameter might push on a value that's a pointer
	// (or otherwise not an integer); deal with it
	
	VM_PARAM_INTEGER(n);
	result.setInt(n+1); // we'll visit the DECJNZ first to handle 0 etc
}

/**
 REPE (command)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0

 The end of a REPS...REPE loop.
*/
void caosVM::c_REPE() {
	// handled elsewhere
}

/**
 LOOP (command)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 The start of a LOOP...EVER or LOOP...UNTL loop.
*/
void caosVM::c_LOOP() {
	// handled elsewhere
}

/**
 EVER (command)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 
 Jumps back to the matching LOOP, no matter what.
*/
void caosVM::c_EVER() {
	// handled elsewhere
}

/**
 UNTL (command) condition (condition)
 %status maybe
 %pragma stackdelta 0
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 Jumps back to the matching LOOP unless the condition evaluates to true.
*/
void caosVM::c_UNTL() {
	// handled elsewhere
}

/**
 GSUB (command) label (label)
 %pragma stackdelta 0
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 Jumps to a subroutine defined by SUBR with label (label).
*/
void caosVM::c_GSUB() {
	// TODO: is cost correct?
	// handled elsewhere
}

/**
 SUBR (command) label (label)
 %status maybe
 %pragma stackdelta 0
 %pragma variants c1 c2 cv c3 sm
 
 Defines the start of a subroute to be called with GSUB, with label (label).
 If the command is encountered during execution, it acts like a STOP.
*/
void caosVM::c_SUBR() {
	// handled elsewhere
}

/**
 RETN (command)
 %pragma stackdelta any
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 Returns from a subroutine called with GSUB.
*/
void caosVM::c_RETN() {
	// TODO: is cost correct?
	if (callStack.empty())
		throw creaturesException("RETN with an empty callstack");
	nip = callStack.back().nip;
	callStack.back().valueStack.swap(valueStack);
	callStack.back().valueStack.clear(); // just in case
	callStack.pop_back();
}

/**
 NEXT (command)
 %status maybe
 %pragma variants all
 %cost c1,c2 0

 The end of an ENUM...NEXT loop.
*/
void caosVM::c_NEXT() {
	targ = owner;
}

/**
 ENUM (command) family (integer) genus (integer) species (integer)
 %status maybe
 %pragma stackdelta any
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Loops through all agents with the given classifier.  0 on any field is a
 wildcard. The loop body is terminated by a NEXT.
*/
void caosVM::c_ENUM() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	caosVar nullv; nullv.reset();
	valueStack.push_back(nullv);
	
	for (std::list<std::shared_ptr<Agent> >::iterator i
			= world.agents.begin(); i != world.agents.end(); i++) {
		std::shared_ptr<Agent> a = (*i);
		if (!a) continue;
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		caosVar v; v.setAgent(a);
		valueStack.push_back(v);
	}
}

/**
 ESEE (command) family (integer) genus (integer) species (integer)
 %status maybe
 %pragma stackdelta any
 %pragma variants c2 cv c3 sm
 
 Simular to ENUM, but iterates through agents visible to OWNR, or visible to TARG in an install script.
 An agent can be seen if it is within the range set by RNGE, and is visible (this includes the PERM value
 of walls that lie between them, and, if the agent is a Creature, it not having the 'invisible' attribute).
*/
void caosVM::c_ESEE() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	Agent *seeing;
	if (owner) seeing = owner; else seeing = targ;
	valid_agent(seeing);

	caosVar nullv; nullv.reset();
	valueStack.push_back(nullv);

	std::vector<std::shared_ptr<Agent> > agents = getVisibleList(seeing, family, genus, species);
	for (std::vector<std::shared_ptr<Agent> >::iterator i = agents.begin(); i != agents.end(); i++) {
		caosVar v; v.setAgent(*i);
		valueStack.push_back(v);
	}
}

/**
 ETCH (command) family (integer) genus (integer) species (integer)
 %pragma stackdelta any
 %status maybe
 %pragma variants c2 cv c3 sm

 Similar to ENUM, but iterates through the agents OWNR is touching, or TARG is touching in an install script.
*/
void caosVM::c_ETCH() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	Agent *touching;
	if (owner) touching = owner; else touching = targ;
	valid_agent(touching);
	
	caosVar nullv; nullv.reset();
	valueStack.push_back(nullv);
	
	for (std::list<std::shared_ptr<Agent> >::iterator i
			= world.agents.begin(); i != world.agents.end(); i++) {
		std::shared_ptr<Agent> a = (*i);
		if (!a) continue;
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;
		if (a.get() == touching) continue;

		if (agentsTouching(a.get(), touching)) {
			caosVar v; v.setAgent(a);
			valueStack.push_back(v);
		}
	}
}

/**
 EPAS (command) family (integer) genus (integer) species (integer)
 %pragma stackdelta any
 %status maybe

 Similar to ENUM, but iterates through the OWNR vehicle's passengers.
*/
void caosVM::c_EPAS() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	caos_assert(owner);
	Vehicle *v = dynamic_cast<Vehicle *>(owner.get());
	caos_assert(v);

	caosVar nullv; nullv.reset();
	valueStack.push_back(nullv);

	for (std::vector<AgentRef>::iterator i = v->passengers.begin(); i != v->passengers.end(); i++) {
		AgentRef a = *i;
		if (!a) continue; // TODO: hrr
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		caosVar v; v.setAgent(a); valueStack.push_back(v);
	}
}

/**
 ECON (command) agent (agent)
 %pragma stackdelta any
 %status stub

 Loops through all the agents in the connective system containing the given agent.
*/
void caosVM::c_ECON() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_VALIDAGENT(agent)

	// TODO: should probably implement this (ECON)

	caosVar nullv; nullv.reset();
	valueStack.push_back(nullv);
}

/**
 CALL (command) script_no (integer) p1 (any) p2 (any)
 %status maybe
 %pragma variants c2 cv c3 sm

 Calls script_no on OWNR, then waits for it to return. The invoked script
 will inherit the caller's INST setting, but any changes it makes to it will
 be reversed once it returns - so eg if you call a script when in INST mode,
 it calls OVER and returns, you'll still be in INST.
 
 Script variables (VAxx) will not be preserved - you'll have to use OVxx
 for any parameters.
 */
void caosVM::c_CALL() {
	VM_PARAM_VALUE(p2)
	VM_PARAM_VALUE(p1)
	VM_PARAM_INTEGER(script_no)

	valid_agent(owner);
	caos_assert(script_no >= 0 && script_no < 65536);

	shared_ptr<script> s = owner->findScript(script_no);
	if (!s) return;
	caosVM *newvm = world.getVM(owner);
	newvm->trace = trace;

	ensure_assert(newvm->fireScript(s, false));
	newvm->inst = inst;
	newvm->_p_[0] = p1;
	newvm->_p_[1] = p2;
	owner->pushVM(newvm);
	stop_loop = true;
}

/**
 CAOS (string) inline (integer) state_trans (integer) p1 (anything) p2 (anything) commands (string) throws (integer) catches (integer) report (variable)
 %status maybe

 Runs commands as caos code immediately. If inline, copy _IT_ VAxx TARG OWNR, etc. If state_trans, copy FROM and 
 OWNR.  If an error occurs, it catches it and stuffs it in the report. (XXX: non-conforming)
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
	
	std::istringstream iss(commands);
	caosScript s("c3", "CAOS command"); // XXX: variant

	caosVM *sub = world.getVM(NULL);
	sub->resetCore();
	
	if (inl) {
		var.ensure(100);
		sub->var.ensure(100);
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
		s.parse(iss);
		if (inl) {
			// Inline CAOS calls are expensive, mmmkay?
			for (int i = 0; i < 100; i++)
				sub->var[s.installer->mapVAxx(i)] = var[currentscript->mapVAxx(i)];
		}

		s.installScripts();
		
		std::ostringstream oss;
		sub->outputstream = &oss;
		
		sub->runEntirely(s.installer);
		
		result.setString(oss.str());
		sub->outputstream = 0;
	} catch (std::exception &e) {
		sub->outputstream = 0; // very important that this isn't pointing onto dead stack when the VM is freed
		
		// TODO: 'catches' should be handled seperately and set report to the error# and string to ***
		// but we have no idea what error# to provide right now (see errors in CAOS.catalogue)
		if (!throws || catches) {
			report->setString(e.what());
			result.setString("###");
		} else {
			world.freeVM(sub);
			throw;
		}
	}
	
	world.freeVM(sub);
}


/* vim: set noet: */
