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
#include "AgentRef.h"
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
 MVTO (command) x (float) y (float)
*/
void caosVM::c_MVTO() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)
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
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	assert(targ);
	result.setInt(1); // TODO: don't hardcode
}

/**
 TMVF (integer) x (float) y (float)
 
 returns 1 if target create could move foot to (x, y) and still be in room system, otherwise returns 0
*/
void caosVM::v_TMVF() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	assert(targ);
	result.setInt(1); // TODO: don't hardcode
}

/**
 ACCG (command) accel (float)
*/
void caosVM::c_ACCG() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(accel)

	assert(targ);
	targ->accg = accel;
}

/**
 ACCG (float)
*/
void caosVM::v_ACCG() {
	VM_VERIFY_SIZE(0)

	assert(targ);
	result.setFloat(targ->accg);
}

/**
 AERO (command) aero (float)
*/
void caosVM::c_AERO() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(aero)

	assert(targ);
	targ->aero = aero;
}

/**
 AERO (float)
*/
void caosVM::v_AERO() {
	VM_VERIFY_SIZE(0)
	
	assert(targ);
	result.setFloat(targ->aero);
}

/**
 RELX (float) first (agent) second (agent)
*/
void caosVM::v_RELX() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_AGENT(second)
	VM_PARAM_AGENT(first)

	// TODO: should we divide by 2.0 to get float answer?
	float one = first->x + (first->getWidth() / 2);
	float two = second->x + (second->getWidth() / 2);

	result.setFloat(two - one);
}

/**
 RELY (float) first (agent) second (agent)
*/
void caosVM::v_RELY() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_AGENT(second)
	VM_PARAM_AGENT(first)

	// TODO: should we divide by 2.0 to get float answer? check it with the second lift in the norn terrarium..
	float one = first->y + (first->getHeight() / 2);
	float two = second->y + (second->getHeight() / 2);

	result.setFloat(two - one);
}

/**
 VELO (command) xvel (float) yvel (float)
*/
void caosVM::c_VELO() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(vely)
	VM_PARAM_FLOAT(velx)

	assert(targ);
	targ->velx.reset();
	targ->velx.setFloat(velx);
	targ->vely.reset();
	targ->vely.setFloat(vely);
}

/**
 MVSF (command) x (float) y (float)

 move the target agent to an area inside the room system at about (x, y)
*/
void caosVM::c_MVSF() {
	// TODO
	c_MVTO();
}

/**
 FRIC (float)
*/
void caosVM::v_FRIC() {
	VM_VERIFY_SIZE(0)
	
	assert(targ);
	result.setFloat(targ->friction);
}

/**
 FRIC (command) friction (integer)
*/
void caosVM::c_FRIC() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(friction) caos_assert(friction >= 0); caos_assert(friction <= 100);

	assert(targ);
	targ->friction = friction;
}

/**
 FALL (integer)

 Return 1 if TARG is moving due to gravity, 0 otherwise
*/
void caosVM::v_FALL() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);

	float accg = targ->accg;
	float vely = targ->vely.getFloat();
	if (accg != 0 && vely != 0) {
		// XXX: this probably isn't quite correct, but it's close enough for
		// now. Hook it into the physics system properly later
		result.setInt(1);
	} else
		result.setInt(0);
}
