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
 OUTS (command) val (string)
*/
void caosVM::c_OUTS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(val)

	// TODO
	for (unsigned int i = 0; i < val.size(); i++) {
		if (val[i] == '\\') {
			assert((i + 1) < val.size());
			i++;
			if (val[i] == 'n') {
				*outputstream << '\n';
			} else {
				*outputstream << val[i];
			}
		} else {
			*outputstream << val[i];
		}
	}
}

/**
 OUTV (command) val (decimal)
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

	returns game variable with name given (unchecked)
*/
void caosVM::v_GAME() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(name)

	caosVar &i = world.variables[name];
	valueStack.push_back(&i);
}

/**
 SCRP (command) family (integer) genus (integer) species (integer) event (integer)
 %pragma noparse
*/

/**
 RSCR (command)
 %pragma noparse
 
*/

/**
 ISCR (command)

 XXX
*/
void caosVM::c_ISCR() {
	VM_VERIFY_SIZE(0)
	// STOP
}

/**
 ENDM (command)
*/
void caosVM::c_ENDM() {
	stop();
}


/**
 RGAM (command)

 No-op for now.
 */
void caosVM::c_RGAM() {}

/**
 MOWS (integer)

 Returns whether the lawn was cut last sunday or not.
 */
void caosVM::v_MOWS() {
	result.setInt(0); // We're too busy coding to mow the lawn.
}

/* vim: set noet: */
