/*
 *  caosVM_agent.cpp
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
#include "openc2e.h"
#include "Agent.h"
#include <iostream>
using std::cerr;

/**
 ELAS (command) elas (integer)
*/
void caosVM::c_ELAS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(elas)
	// TODO
}

/**
 MVTO (command) x (integer) y (integer)
*/
void caosVM::c_MVTO() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	assert(targ);
	targ->moveTo(x, y);
}

/**
 VELX (variable)
*/
void caosVM::v_VELX() {
	VM_VERIFY_SIZE(0)
	assert(targ);
	result = targ->velx;
	result.setVariable(&targ->velx);
}

/**
 VELY (variable)
*/
void caosVM::v_VELY() {
	VM_VERIFY_SIZE(0)

	assert(targ);
	result = targ->vely;
	result.setVariable(&targ->vely);
}

/**
 OBST (float) direction (integer)
*/
void caosVM::v_OBST() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(direction) assert(direction >= 0); assert(direction <= 3);

	result.setFloat(0.0f);
}

/**
 TMVT (integer) x (float) y (float)
 
 returns 1 if TARG could move to (x, y) and still be in room system, otherwise returns 0
*/
void caosVM::v_TMVT() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	assert(targ);
	result.setInt(0); // TODO: don't hardcode
}
