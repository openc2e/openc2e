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
 %status maybe

 Sets the elasticity (in other words, bounciness) of the TARG agent.
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

 Returns the elasticity of the TARG agent.
*/
void caosVM::v_ELAS() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setInt(targ->elas);
}

/**
 MVTO (command) x (float) y (float)
 %status maybe

 Places the TARG agent at the given x/y position in the world (using the upper left hand corner of the agent).
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

 Changes the TARG agent's position by the given relative distances.
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

 Returns the current horizontal velocity, in pixels/tick, of the TARG agent.
*/
void caosVM::v_VELX() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	vm->valueStack.push_back(&targ->velx);
}

/**
 VELY (variable)
 %status maybe

 Returns the current vertical velocity, in pixels/tick, of the TARG agent.
*/
void caosVM::v_VELY() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	vm->valueStack.push_back(&targ->vely);
}

/**
 OBST (float) direction (integer)
 %status maybe

 Returns the distance from the TARG agent to the nearest wall that it might collide with in the given direction.
 (except right now it just gives the direction to the nearest wall at world edge - fuzzie)
*/
void caosVM::v_OBST() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(direction) caos_assert(direction >= 0); caos_assert(direction <= 3);

	/*
	 * TODO: CL's docs say to return "a very large number" if distance is greater than rnge - if (!collided)?
	 * also, this code is untested :) - fuzzie
	 */
	
	caos_assert(targ);
	
	Point src = targ->boundingBoxPoint(direction);
	Point dest = src;

	switch (direction) {
		case 0: // left
			dest.x -= targ->range; break;
		case 1: // right
			dest.x += targ->range; break;
		case 2: // top
			dest.y -= targ->range; break;
		case 3: // bottom
			dest.y += targ->range; break;
	}

	Room *ourRoom = world.map.roomAt(src.x, src.y);
	if (!ourRoom) {
		// TODO: is this correct behaviour?
		result.setFloat(0.0f);
		return;
	}

	unsigned int dummy1; Line dummy2; Point point;
	bool collided = world.map.collideLineWithRoomSystem(src, dest, ourRoom, point, dummy2, dummy1, targ->perm);

	switch (direction) {
		case 0: result.setFloat(src.x - point.x); break;
		case 1: result.setFloat(point.x - src.x); break;
		case 2: result.setFloat(src.y - point.y); break;
		case 3: result.setFloat(point.y - src.y); break;
	}
}

/**
 TMVT (integer) x (float) y (float)
 %status maybe
 
 Returns 1 if the TARG agent could move to (x, y) and still be in room system, or 0 if otherwise.
*/
void caosVM::v_TMVT() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	caos_assert(targ);
	// TODO: do this properly
	//Room *r1 = world.map.roomAt(x, y);
	//Room *r2 = world.map.roomAt(x + (targ->getWidth() / 2), y + (targ->getHeight() / 2));
	Room *r1 = world.map.roomAt(x + (targ->getWidth() / 2), y);
	Room *r2 = world.map.roomAt(x + (targ->getWidth() / 2), y + targ->getHeight());
	result.setInt((r1 && r2) ? 1 : 0);
}

/**
 TMVF (integer) x (float) y (float)
 %status stub
 
 Returns 1 if the TARG Creature could move foot to (x, y) and still be in room system, or 0 if otherwise.
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

 Sets the TARG agent's free-fall acceleration, in pixels/tick squared.
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
 
 Returns the TARG agent's free-fall acceleration, in pixels/tick squared.
*/
void caosVM::v_ACCG() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setFloat(targ->accg);
}

/**
 AERO (command) aero (float)
 %status maybe

 Sets the aerodynamics of the TARG agent to the given float value.
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

 Returns the aerodynamics of the TARG agent.
*/
void caosVM::v_AERO() {
	VM_VERIFY_SIZE(0)
	
	caos_assert(targ);
	result.setFloat(targ->aero);
}

/**
 RELX (float) first (agent) second (agent)
 %status maybe

 Returns the relative horizontal distance between the centers of the two given agents.
*/
void caosVM::v_RELX() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_VALIDAGENT(second)
	VM_PARAM_VALIDAGENT(first)

	float one = first->x + (first->getWidth() / 2.0);
	float two = second->x + (second->getWidth() / 2.0);

	result.setFloat(two - one);
}

/**
 RELY (float) first (agent) second (agent)
 %status maybe

 Returns the relative vertical distance between the centers of the two given agents.
*/
void caosVM::v_RELY() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_VALIDAGENT(second)
	VM_PARAM_VALIDAGENT(first)

	float one = first->y + (first->getHeight() / 2.0);
	float two = second->y + (second->getHeight() / 2.0);

	result.setFloat(two - one);
}

/**
 VELO (command) xvel (float) yvel (float)
 %status maybe

 Sets the horizontal and vertical velocity of the TARG agent, in pixels/tick.
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
 %status maybe

 Move the target agent to an area inside the room system at about (x, y).
 This allows 'safe' moves.
*/
void caosVM::c_MVSF() {
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)
	caos_assert(targ);

	// TODO: this is a silly hack, to cater for simplest case (where we just need to nudge the agent up a bit)
	unsigned int tries = 0;
	while (tries < 150) {
		Room *r1 = world.map.roomAt(x + (targ->getWidth() / 2), y - tries);
		Room *r2 = world.map.roomAt(x + (targ->getWidth() / 2), y + targ->getHeight() - tries);
		if (r1 && r2) {
			// if they're in the same room, we're fine, otherwise, make sure they're at least connected
			// (hacky, but who cares, given the whole thing is a hack)
			if (r1 == r2 || r1->doors.find(r2) != r1->doors.end()) {
				targ->moveTo(x, y - tries);
				return;
			}
		}
		tries++;
	}
	
	throw creaturesException("MVSF failed to find a safe place");
}

/**
 FRIC (float)
 %status maybe

 Returns the TARG agent's coefficient of friction as a percentage.
*/
void caosVM::v_FRIC() {
	VM_VERIFY_SIZE(0)
	
	caos_assert(targ);
	result.setFloat(targ->friction);
}

/**
 FRIC (command) friction (integer)
 %status maybe
 
 Sets the TARG agent's coefficient of friction, or the percentage of motion that will be lost as it slides on a 
 surface.
*/
void caosVM::c_FRIC() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(friction) caos_assert(friction >= 0); caos_assert(friction <= 100);

	caos_assert(targ);
	targ->friction = friction;
}

/**
 FALL (integer)
 %status maybe

 Returns 1 if the TARG agent is moving due to gravity, or 0 if otherwise.
*/
void caosVM::v_FALL() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);

	// XXX: this probably isn't quite correct, but it's close enough for now.
	if (targ->falling)
		result.setInt(1);
	else
		result.setInt(0);
}

/**
 MOVS (integer)
 %status stub

 Returns an integer representing the motion status of the TARG agent.  0 is autonomous, 1 is moving by mouse, 2 is 
 floating, 3 is inside a vehicle, and 4 is being carried.
*/
void caosVM::v_MOVS() {
	caos_assert(targ);

	result.setInt(0); // TODO
}

/**
 FLTO (command) x (float) y (float)
 %status maybe

 Sets the TARG agent to float its top-left corner (x, y) away from the top-left corner of the FREL agent.
*/
void caosVM::c_FLTO() {
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	caos_assert(targ)
	targ->floatTo(x, y);
}

/**
 FREL (command) agent (agent)
 %status maybe

 Sets the agent the TARG agent floats relative to. You must set the 'floatable' attribute for this to work.
 The default is NULL, which means the target agent floats relative to the main camera.
*/
void caosVM::c_FREL() {
	VM_PARAM_AGENT(agent)

	caos_assert(targ)
	targ->floatTo(agent);
}

/**
 FLTX (float)
 %status maybe

 Returns the x value of the TARG agent's floating vector.
*/
void caosVM::v_FLTX() {
	caos_assert(targ);

	if (targ->floatingagent)
		result.setFloat(targ->floatingagent->x - targ->x);
	else
		result.setFloat(world.camera.getX() - targ->x);
}

/**
 FLTY (float)
 %status maybe

 Returns the y value of the TARG agent's floating vector.
*/
void caosVM::v_FLTY() {
	caos_assert(targ);
	
	if (targ->floatingagent)
		result.setFloat(targ->floatingagent->x - targ->x);
	else
		result.setFloat(world.camera.getX() - targ->x);
}

/* vim: set noet: */
