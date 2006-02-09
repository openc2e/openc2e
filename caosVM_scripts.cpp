/*
 *  caosVM_scripts.cpp
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
#include "World.h"
#include <iostream>
using std::cerr;

/**
 INST (command)
 %status maybe

 Forces the following commands to be executed in one tick, rather than scheduling them, until SLOW or the end 
 of the script is encountered.
 */
void caosVM::c_INST() {
	VM_VERIFY_SIZE(0)
	inst = true;
	// TODO: do we need a state similar to locked? i commented it out because it doesn't seem right - fuzzie
	//locked = true;
}

/**
 SLOW (command)
 %status maybe

 Reverses the effects of INST.
 */
void caosVM::c_SLOW() {
	VM_VERIFY_SIZE(0)
	
	inst = false;
}

/**
 LOCK (command)
 %status maybe

 Prevent the script from being interrupted by another until UNLK or the end of the script is encountered.
 */
void caosVM::c_LOCK() {
	VM_VERIFY_SIZE(0)
	lock = true;
}

/**
 UNLK (command)
 %status maybe

 Reverses the effects of LOCK.
 */
void caosVM::c_UNLK() {
	VM_VERIFY_SIZE(0)
	
	lock = false;
}

class blockUntilTime : public blockCond {
	protected:
		unsigned int end;
	public:
		bool operator()() {
			if (world.tickcount < end)
				return true;
			return false;
		}

		blockUntilTime(int delta) : end(world.tickcount + delta) {}
};

/**
 WAIT (command) ticks (integer)
 %status maybe

 Stops the script from running for the given number of ticks.
 */
void caosVM::c_WAIT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(ticks)

	caos_assert(ticks > 0); // todo: is this right?
	startBlocking(new blockUntilTime(ticks));
}

/**
 STOP (command)
 %status maybe
 %pragma retc -1

 Aborts the script.
*/
void caosVM::c_STOP() {
	VM_VERIFY_SIZE(0)
	stop();
}

/**
 SCRX (command) family (integer) genus (integer) species (integer) event (integer)
 %status maybe

 Deletes the event script in question from the scriptoruium.
*/
void caosVM::c_SCRX() {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(event) 
	caos_assert(event >= 0); 
	caos_assert(event <= 255);
	VM_PARAM_INTEGER(species) 
	caos_assert(species >= 0); 
	caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) 
	caos_assert(genus >= 0); 
	caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) 
	caos_assert(family >= 0); 
	caos_assert(family <= 255);
	world.scriptorium.delScript(family, genus, species, event);
}

/**
 CODE (integer)
 %status maybe

 Returns script number running in the TARG agent. Returns -1 if target is not
 running anything (or if it's running something that's not an event script).
*/
void caosVM::v_CODE() {
	caos_assert(targ);
	int res;
	if (targ->vm && targ->vm->currentscript)
		res = targ->vm->currentscript->scrp;
	else
		res = -1;
	
	result.setInt(res);
}

/**
 CODF (integer)
 %status maybe

 Returns script family running in the TARG agent. Returns -1 if target is not
 running anything (or if it's running something that's not an event script).
*/
void caosVM::v_CODF() {
	caos_assert(targ);
	int res;
	if (targ->vm && targ->vm->currentscript)
		res = targ->vm->currentscript->fmly;
	else
		res = -1;
	
	result.setInt(res);
}

/**
 CODG (integer)
 %status maybe

 Returns script genus running in the target. Returns -1 if target is not
 running anything (or if it's running something that's not an event script).
*/
void caosVM::v_CODG() {
	caos_assert(targ);
	int res;
	if (targ->vm && targ->vm->currentscript)
		res = targ->vm->currentscript->gnus;
	else
		res = -1;
	
	result.setInt(res);
}

/**
 CODS (integer)
 %status maybe

 Returns script species running in the target. Returns -1 if target is not
 running anything (or if it's running something that's not an event script).
*/
void caosVM::v_CODS() {
	caos_assert(targ);
	int res;
	if (targ->vm && targ->vm->currentscript)
		res = targ->vm->currentscript->spcs;
	else
		res = -1;
	
	result.setInt(res);
}

/**
 JECT (command) file (string) flags (integer)
 %status stub

 Inject a script from the current bootstrap. 'file' must be the full filename.
 Flags can be 1 for remove script, 2 for event scripts and 4 for install script.
*/
void caosVM::c_JECT() {
	VM_PARAM_INTEGER(flags)
	VM_PARAM_STRING(file)

	// TODO
}

/* vim: set noet: */
