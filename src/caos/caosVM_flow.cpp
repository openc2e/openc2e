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

#include "caos_assert.h"
#include "caosVM.h"
#include <iostream>
#include <memory>
#include "World.h" // enum
#include <cmath>   // sqrt
#include <sstream>
#include "AgentHelpers.h"
#include "Vehicle.h" // EPAS
#include "caosScript.h" // CAOS

/**
 DOIF (command) condition (condition)
 %status maybe
 %variants c1 c2 cv c3 sm
 %stackdelta 0
 %cost c1,c2 0
 
 Part of a DOIF/ELIF/ELSE/ENDI block. Jumps to the next part of the block if condition is false, 
 otherwise continues executing the script.
*/
void c_DOIF(caosVM*) {
	// handled elsewhere
}

/**
 ELIF (command) condition (condition)
 %variants c3 cv sm openc2e
 %status maybe
 %stackdelta 0
 %cost c1,c2 0
 
 Part of a DOIF/ELIF/ELSE/ENDI block. If none of the previous DOIF/ELIF conditions have been true, and condition evaluates to true, then the code in the ELIF block is executed.
 If found outside a DOIF block, it is equivalent to a DOIF. If you take advantage of this behavior, fuzzie is of the opinion that you should be shot.
*/
void c_ELIF(caosVM*) {
	// handled elsewhere
}


/**
 ELSE (command)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 Part of a DOIF/ELIF/ELSE/ENDI block. If ELSE is present, it is jumped to when none of the previous DOIF/ELIF conditions are true.
*/
void c_ELSE(caosVM*) {
	// handled elsewhere
}

/**
 ENDI (command)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 The end of a DOIF/ELIF/ELSE/ENDI block.
*/
void c_ENDI(caosVM*) {
	// TODO: cost in c2e?
}

/**
 REPS (command) reps (integer)
 %status maybe
 %stackdelta 0
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0

 The start of a REPS...REPE loop. The body of the loop will be executed (reps) times.
*/
void c_REPS(caosVM *vm) {
	// Our expression parameter might push on a value that's a pointer
	// (or otherwise not an integer); deal with it
	
	VM_PARAM_INTEGER(n);
	vm->result.setInt(n+1); // we'll visit the DECJNZ first to handle 0 etc
}

/**
 REPE (command)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0

 The end of a REPS...REPE loop.
*/
void c_REPE(caosVM*) {
	// handled elsewhere
}

/**
 LOOP (command)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 The start of a LOOP...EVER or LOOP...UNTL loop.
*/
void c_LOOP(caosVM*) {
	// handled elsewhere
}

/**
 EVER (command)
 %status maybe
 %variants c1 c2 cv c3 sm
 
 Jumps back to the matching LOOP, no matter what.
*/
void c_EVER(caosVM*) {
	// handled elsewhere
}

/**
 UNTL (command) condition (condition)
 %status maybe
 %stackdelta 0
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 Jumps back to the matching LOOP unless the condition evaluates to true.
*/
void c_UNTL(caosVM*) {
	// handled elsewhere
}

/**
 GSUB (command) label (label)
 %stackdelta 0
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 Jumps to a subroutine defined by SUBR with label (label).
*/
void c_GSUB(caosVM*) {
	// TODO: is cost correct?
	// handled elsewhere
}

/**
 SUBR (command) label (label)
 %status maybe
 %stackdelta 0
 %variants c1 c2 cv c3 sm
 
 Defines the start of a subroute to be called with GSUB, with label (label).
 If the command is encountered during execution, it acts like a STOP.
*/
void c_SUBR(caosVM*) {
	// handled elsewhere
}

/**
 RETN (command)
 %stackdelta any
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 Returns from a subroutine called with GSUB.
*/
void c_RETN(caosVM *vm) {
	// TODO: is cost correct?
	if (vm->callStack.empty())
		throw creaturesException("RETN with an empty callstack");
	vm->nip = vm->callStack.back().nip;
	vm->callStack.back().valueStack.swap(vm->valueStack);
	vm->callStack.back().valueStack.clear(); // just in case
	vm->callStack.pop_back();
}

/**
 NEXT (command)
 %status maybe
 %variants all
 %cost c1,c2 0

 The end of an ENUM...NEXT loop.
*/
void c_NEXT(caosVM *vm) {
	vm->targ = vm->owner;
}

/**
 NSCN (command)
 %status maybe
 %variants c2
 %cost c2 0

 The end of an ESCN...NSCN loop.
*/
void c_NSCN(caosVM *vm) {
    vm->targ = vm->owner;
}

/**
 ENUM (command) family (integer) genus (integer) species (integer)
 %status maybe
 %stackdelta any
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Loops through all agents with the given classifier.  0 on any field is a
 wildcard. The loop body is terminated by a NEXT.
*/
void c_ENUM(caosVM *vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	caosValue nullv; nullv.reset();
	vm->valueStack.push_back(nullv);
	
	for (auto & agent : world.agents) {
		std::shared_ptr<Agent> a = agent;
		if (!a) continue;
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		caosValue v; v.setAgent(a);
		vm->valueStack.push_back(v);
	}
}

/**
 ESEE (command) family (integer) genus (integer) species (integer)
 %status maybe
 %stackdelta any
 %variants c2 cv c3 sm
 
 Simular to ENUM, but iterates through agents visible to OWNR, or visible to TARG in an install script.
 An agent can be seen if it is within the range set by RNGE, and is visible (this includes the PERM value
 of walls that lie between them, and, if the agent is a Creature, it not having the 'invisible' attribute).
*/
void c_ESEE(caosVM *vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	Agent *seeing;
	if (vm->owner) seeing = vm->owner; else seeing = vm->targ;
	valid_agent(seeing);

	caosValue nullv; nullv.reset();
	vm->valueStack.push_back(nullv);

	std::vector<std::shared_ptr<Agent> > agents = getVisibleList(seeing, family, genus, species);
	for (auto & agent : agents) {
		caosValue v; v.setAgent(agent);
		vm->valueStack.push_back(v);
	}
}

/**
 ETCH (command) family (integer) genus (integer) species (integer)
 %stackdelta any
 %status maybe
 %variants c2 cv c3 sm

 Similar to ENUM, but iterates through the agents OWNR is touching, or TARG is touching in an install script.
*/
void c_ETCH(caosVM *vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	Agent *touching;
	if (vm->owner) touching = vm->owner; else touching = vm->targ;
	valid_agent(touching);
	
	caosValue nullv; nullv.reset();
	vm->valueStack.push_back(nullv);
	
	for (auto & agent : world.agents) {
		std::shared_ptr<Agent> a = agent;
		if (!a) continue;
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;
		if (a.get() == touching) continue;

		if (agentsTouching(a.get(), touching)) {
			caosValue v; v.setAgent(a);
			vm->valueStack.push_back(v);
		}
	}
}

/**
 EPAS (command) family (integer) genus (integer) species (integer)
 %stackdelta any
 %status maybe

 Similar to ENUM, but iterates through the OWNR vehicle's passengers.
*/
void c_EPAS(caosVM *vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	caos_assert(vm->owner);
	Vehicle *v = dynamic_cast<Vehicle *>(vm->owner.get());
	caos_assert(v);

	caosValue nullv; nullv.reset();
	vm->valueStack.push_back(nullv);

	for (auto a : v->passengers) {
			if (!a) continue; // TODO: hrr
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		caosValue v; v.setAgent(a); vm->valueStack.push_back(v);
	}
}

/**
 ECON (command) agent (agent)
 %stackdelta any
 %status stub

 Loops through all the agents in the connective system containing the given agent.
*/
void c_ECON(caosVM *vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_VALIDAGENT(agent)

	// TODO: should probably implement this (ECON)

	caosValue nullv; nullv.reset();
	vm->valueStack.push_back(nullv);
}

/**
 ESCN (command) family (integer) genus (integer) species (integer)
 %stackdelta any
 %status stub
 %variants c2

 Enumerate all specified scenery objects.
*/
void c_ESCN(caosVM *vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	// TODO: should implement this. C2 has a concept of "scenery objects" created
	// with NEW: SCEN.

	caosValue nullv; nullv.reset();
	vm->valueStack.push_back(nullv);
}

/**
 CALL (command) script_no (integer) p1 (anything) p2 (anything)
 %status maybe
 %variants c3 cv sm

 Calls script_no on OWNR, then waits for it to return. The invoked script
 will inherit the caller's INST setting, but any changes it makes to it will
 be reversed once it returns - so eg if you call a script when in INST mode,
 it calls OVER and returns, you'll still be in INST.
 
 Script variables (VAxx) will not be preserved - you'll have to use OVxx
 for any parameters.
 */
void c_CALL(caosVM *vm) {
	VM_PARAM_VALUE(p2)
	VM_PARAM_VALUE(p1)
	VM_PARAM_INTEGER(script_no)

	valid_agent(vm->owner);
	caos_assert(script_no >= 0 && script_no < 65536);

	std::shared_ptr<script> s = vm->owner->findScript(script_no);
	if (!s) return;
	caosVM *newvm = world.getVM(vm->owner);
	newvm->trace = vm->trace;

	ensure_assert(newvm->fireScript(s, false));
	newvm->inst = vm->inst;
	newvm->_p_[0] = p1;
	newvm->_p_[1] = p2;
	vm->owner->pushVM(newvm);
	vm->stop_loop = true;
}

/**
 CAOS (string) inline (integer) state_trans (integer) p1 (anything) p2 (anything) commands (string) throws (integer) catches (integer) report (variable)
 %status maybe

 Runs commands as caos code immediately. If inline, copy _IT_ VAxx TARG OWNR, etc. If state_trans, copy FROM and 
 OWNR.  If an error occurs, it catches it and stuffs it in the report. (XXX: non-conforming)
*/

// XXX: exception catching is very broken right now

void v_CAOS(caosVM *vm) {
	// XXX: capture output
	VM_PARAM_VARIABLE(report)
	VM_PARAM_INTEGER(catches)
	VM_PARAM_INTEGER(throws)
	VM_PARAM_STRING(commands)
	VM_PARAM_VALUE(p2)
	VM_PARAM_VALUE(p1)
	VM_PARAM_INTEGER(state_trans)
	VM_PARAM_INTEGER(inl)
	
	caosScript s("c3", "CAOS command"); // XXX: variant

	caosVM *sub = world.getVM(NULL);
	sub->resetCore();
	
	if (inl) {
		sub->targ = vm->targ;
		sub->_it_ = vm->_it_;
		sub->part = vm->part;
		sub->owner = vm->owner;
		// sub->from = from;
	}
	
	if (state_trans) {
		sub->owner = vm->owner;
		// sub->from = from;
	}
	
	sub->_p_[0] = p1;
	sub->_p_[1] = p2;

	try {
		s.parse(commands);
		if (inl) {
			// Inline CAOS calls are expensive, mmmkay?
			for (int i = 0; i < 100; i++)
				sub->var[s.installer->mapVAxx(i)] = vm->var[vm->currentscript->mapVAxx(i)];
		}

		s.installScripts();
		
		std::ostringstream oss;
		sub->outputstream = &oss;
    
		
		sub->runEntirely(s.installer);
		
		vm->result.setString(oss.str());
		sub->outputstream = 0;
	} catch (creaturesException &e) {
		sub->outputstream = 0; // very important that this isn't pointing onto dead stack when the VM is freed
		
		// TODO: 'catches' should be handled seperately and set report to the error# and string to ***
		// but we have no idea what error# to provide right now (see errors in CAOS.catalogue)
		if (!throws || catches) {
			report->setString(e.what());
			vm->result.setString("###");
		} else {
			world.freeVM(sub);
			throw;
		}
	}
	
	world.freeVM(sub);
}


/* vim: set noet: */
