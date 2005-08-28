/*
 *  caosVM_core.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
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
#include "World.h"
#include <iostream>

using std::cout;
using std::cerr;

/**
 OUTX (command) val (string)
 %status maybe
*/
void caosVM::c_OUTX() {
	VM_PARAM_STRING(val)
	
	std::string oh = "\"";
	
	for (unsigned int i = 0; i < val.size(); i++) {
		switch (val[i]) {
			case '\r': oh += "\\r";
			case '\n': oh += "\\n";
			case '\t': oh += "\\t";
			default: oh + val[i];
		}
	}

	*outputstream << oh << "\"";
}

/**
 OUTS (command) val (string)
 %status maybe
*/
void caosVM::c_OUTS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(val)

	*outputstream << val;
}

/**
 OUTV (command) val (decimal)
 %status maybe
*/
void caosVM::c_OUTV() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_DECIMAL(val)

	if (val.hasFloat()) {
		// TODO: DS spits things like '4.000000' and '-5.000000', we don't
		*outputstream << val.floatValue;
	} else {
		*outputstream << val.intValue;
	}
}

/**
	GAME (variable) name (string)
	%status maybe

	returns game variable with name given
*/
void caosVM::v_GAME() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(name)

	caosVar &i = world.variables[name];
	valueStack.push_back(&i);
}

/**
 EAME (variable) name (anything)
 %status maybe

 returns temporary game variable with name given
*/
void caosVM::v_EAME() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VALUE(name)

	caosVar &i = world.eame_variables[name];
	valueStack.push_back(&i);
}


/**
 DELG (command) name (string)
 %status maybe

 deletes given game variable
*/
void caosVM::c_DELG() {
	VM_PARAM_STRING(name)

	std::map<std::string, caosVar>::iterator i = world.variables.find(name);
	if (i != world.variables.end())
		world.variables.erase(i);
	else
		std::cerr << "DELG got told to delete '" << name << "' but it doesn't exist!" << std::endl;
}

/**
 SCRP (command) family (integer) genus (integer) species (integer) event (integer)
 %status done
 %pragma noparse
*/

/**
 RSCR (command)
 %status done
 %pragma noparse 
*/

/**
 ISCR (command)
 %status stub

 XXX
*/
void caosVM::c_ISCR() {
	VM_VERIFY_SIZE(0)
	// STOP
}

/**
 ENDM (command)
 %status done
*/
void caosVM::c_ENDM() {
	stop();
}

/**
 RGAM (command)
 %status stub

 No-op for now.
 */
void caosVM::c_RGAM() {}

/**
 MOWS (integer)
 %status stub

 Returns whether the lawn was cut last sunday or not.
 */
void caosVM::v_MOWS() {
	result.setInt(0); // We're too busy coding to mow the lawn.
}

/* vim: set noet: */
