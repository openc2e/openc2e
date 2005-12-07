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
#include "World.h"
#include <iostream>
using std::cerr;

/**
 ELAS (command) elas (integer)
 %status stub
*/
void caosVM::c_ELAS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(elas)

	caos_assert(targ);
	targ->elas = elas;
}

/***
 ELAS (integer)
 %status maybe
*/
void caosVM::v_ELAS() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setInt(targ->elas);
}

/**
 MVTO (command) x (float) y (float)
 %status maybe
*/
void caosVM::c_MVTO() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)
	caos_assert(targ);
	targ->moveTo(x, y);
}

/**
 MVBY (command) x (float) y (float)
 %status maybe
*/
void caosVM::c_MVBY() {
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	caos_assert(targ);
	targ->moveTo(targ->x + x, targ->y + y);
}

/**
 VELX (variable)
 %status maybe
*/
void caosVM::v_VELX() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	vm->valueStack.push_back(&targ->velx);
}

/**
 VELY (variable)
 %status maybe
*/
void caosVM::v_VELY() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	vm->valueStack.push_back(&targ->vely);
}

/**
 OBST (float) direction (integer)
 %status maybe

 returns distance from target agent to nearest wall that it might collide with in given direction
 (except right now it just gives the direction to the nearest wall at world edge - fuzzie)
*/
void caosVM::v_OBST() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(direction) caos_assert(direction >= 0); caos_assert(direction <= 3);

	// TODO: fix 'might collide with' issue
	// note: this code is mostly copied from GRID - fuzzie

	// TODO: this code should calculate distance *from agent*, not *from centre point*, i expect
	// someone check with DS? - fuzzie

	float agentx = targ->x + (targ->getWidth() / 2);
	float agenty = targ->y + (targ->getHeight() / 2);
	Room *sourceroom = world.map.roomAt(agentx, agenty);
	if (!sourceroom) {
		// (should we REALLY check for it being in the room system, here?)
		//cerr << targ->identify() << " tried using OBST but isn't in the room system!\n";
		result.setInt(-1);
		return;
	}

	int distance = 0;
	
	if ((direction == 0) || (direction == 1)) {
		int movement = (direction == 0 ? -1 : 1);

		int x = agentx;
		Room *r = 0;
		while (true) {
			x += movement;
			if (r) {
				if (!r->containsPoint(x, agenty))
					r = world.map.roomAt(x, agenty);
			} else {
				r = world.map.roomAt(x, agenty);
			}
			if (!r)
				break;
			distance++;
		}
	} else if ((direction == 2) || (direction == 3)) {
		int movement = (direction == 2 ? -1 : 1);
	
		int y = agenty;
		Room *r = 0;
		while (true) {
			y += movement;
			if (r) {
				if (!r->containsPoint(agentx, y))
					r = world.map.roomAt(agentx, y);
			} else {
				r = world.map.roomAt(agentx, y);
			}
			if (!r)
				break;
			distance++;
		}
	} else cerr << "OBST got an unknown direction!\n";

	result.setInt(distance);
}

/**
 TMVT (integer) x (float) y (float)
 %status maybe
 
 returns 1 if TARG could move to (x, y) and still be in room system, otherwise returns 0
*/
void caosVM::v_TMVT() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	caos_assert(targ);
	// TODO: do this properly
	Room *r1 = world.map.roomAt(x, y);
	Room *r2 = world.map.roomAt(x + (targ->getWidth() / 2), y + (targ->getHeight() / 2));
	result.setInt((r1 && r2) ? 1 : 0);
}

/**
 TMVF (integer) x (float) y (float)
 %status stub
 
 returns 1 if target create could move foot to (x, y) and still be in room system, otherwise returns 0
*/
void caosVM::v_TMVF() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	caos_assert(targ);
	result.setInt(1); // TODO: don't hardcode
}

/**
 ACCG (command) accel (float)
 %status maybe
*/
void caosVM::c_ACCG() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(accel)

	caos_assert(targ);
	targ->accg = accel;
}

/**
 ACCG (float)
 %status maybe
*/
void caosVM::v_ACCG() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setFloat(targ->accg);
}

/**
 AERO (command) aero (float)
 %status maybe
*/
void caosVM::c_AERO() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(aero)

	caos_assert(targ);
	targ->aero = aero;
}

/**
 AERO (float)
 %status maybe
*/
void caosVM::v_AERO() {
	VM_VERIFY_SIZE(0)
	
	caos_assert(targ);
	result.setFloat(targ->aero);
}

/**
 RELX (float) first (agent) second (agent)
 %status maybe
*/
void caosVM::v_RELX() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_AGENT(second)
	VM_PARAM_AGENT(first)

	float one = first->x + (first->getWidth() / 2.0);
	float two = second->x + (second->getWidth() / 2.0);

	result.setFloat(two - one);
}

/**
 RELY (float) first (agent) second (agent)
 %status maybe
*/
void caosVM::v_RELY() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_AGENT(second)
	VM_PARAM_AGENT(first)

	float one = first->y + (first->getHeight() / 2.0);
	float two = second->y + (second->getHeight() / 2.0);

	result.setFloat(two - one);
}

/**
 VELO (command) xvel (float) yvel (float)
 %status maybe
*/
void caosVM::c_VELO() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(vely)
	VM_PARAM_FLOAT(velx)

	caos_assert(targ);
	targ->velx.reset();
	targ->velx.setFloat(velx);
	targ->vely.reset();
	targ->vely.setFloat(vely);
}

/**
 MVSF (command) x (float) y (float)
 %status stub

 move the target agent to an area inside the room system at about (x, y)
*/
void caosVM::c_MVSF() {
	// TODO
	c_MVTO();
}

/**
 FRIC (float)
 %status maybe
*/
void caosVM::v_FRIC() {
	VM_VERIFY_SIZE(0)
	
	caos_assert(targ);
	result.setFloat(targ->friction);
}

/**
 FRIC (command) friction (integer)
 %status maybe
*/
void caosVM::c_FRIC() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(friction) caos_assert(friction >= 0); caos_assert(friction <= 100);

	caos_assert(targ);
	targ->friction = friction;
}

/**
 FALL (integer)
 %status stub

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

/**
 MOVS (integer)
 %status stub
*/
void caosVM::v_MOVS() {
	caos_assert(targ);

	result.setInt(0); // TODO
}

/**
 FLTO (command) x (float) y (float)
 %status maybe

 Set the target agent to float its top-left corner (x, y) away from the top-left corner of the FREL agent.
*/
void caosVM::c_FLTO() {
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	caos_assert(targ)
	targ->floatingx = x;
	targ->floatingy = y;
}

/**
 FREL (command) agent (agent)
 %status maybe

 Set the agent the target agent floats relative to. You must set the 'floatable' attribute for this to work.
 The default is NULL, which means the target agent floats relative to the main camera.
*/
void caosVM::c_FREL() {
	VM_PARAM_AGENT(agent)

	caos_assert(targ)
	targ->floatingagent = agent;
}

/* vim: set noet: */
