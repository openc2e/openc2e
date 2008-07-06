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
#include "Engine.h" // version
#include <iostream>
#include <boost/format.hpp>
#include <climits>
using std::cerr;

/**
 ELAS (command) elas (integer)
 %status maybe

 Sets the elasticity (in other words, bounciness) of the TARG agent.
*/
void caosVM::c_ELAS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(elas)

	valid_agent(targ);
	targ->elas = elas;
}

/***
 ELAS (integer)
 %status maybe

 Returns the elasticity of the TARG agent.
*/
void caosVM::v_ELAS() {
	VM_VERIFY_SIZE(0)

	valid_agent(targ);
	result.setInt(targ->elas);
}

/**
 MVTO (command) x (float) y (float)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 1

 Places the TARG agent at the given x/y position in the world (using the upper left hand corner of the agent).
*/
void caosVM::c_MVTO() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)
	valid_agent(targ);
	targ->moveTo(x, y);
}

/**
 MVBY (command) x (float) y (float)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 1

 Changes the TARG agent's position by the given relative distances.
*/
void caosVM::c_MVBY() {
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	valid_agent(targ);
	targ->moveTo(targ->x + x, targ->y + y);
}

/**
 VELX (variable)
 %status maybe
 %pragma variants c2 cv c3 sm

 Returns the current horizontal velocity, in pixels/tick, of the TARG agent.
*/
void caosVM::v_VELX() {
	valid_agent(targ);
	valueStack.push_back(targ->velx);
}
void caosVM::s_VELX() {
	VM_PARAM_VALUE(newvalue)
	caos_assert(newvalue.hasDecimal());
	
	valid_agent(targ);	
	targ->velx = newvalue;
}

/**
 VELY (variable)
 %status maybe
 %pragma variants c2 cv c3 sm

 Returns the current vertical velocity, in pixels/tick, of the TARG agent.
*/
void caosVM::v_VELY() {
	valid_agent(targ);
	valueStack.push_back(targ->vely);
}
void caosVM::s_VELY() {
	VM_PARAM_VALUE(newvalue)
	caos_assert(newvalue.hasDecimal());
	
	valid_agent(targ);	
	targ->vely = newvalue;

	// a whole bunch of Creatures 2 scripts/COBs depend on this ('setv vely 0' to activate gravity)
	if (engine.version == 2) targ->grav.setInt(1);
}

/**
 OBST (float) direction (integer)
 %status maybe
 %pragma variants cv c3 sm

 Returns the distance from the TARG agent to the nearest wall that it might collide with in the given direction.
 (except right now it just gives the direction to the nearest wall at world edge - fuzzie)
*/
void caosVM::v_OBST() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(direction) caos_assert(direction >= 0); caos_assert(direction <= 3);

	/*
	 * TODO: CL's docs say to return "a very large number" if distance is greater than rnge - if (!collided)?
	 * TODO: c2 docs say "from the centre point of TARG".. same in c2e? or do we need a separate function?
	 * also, this code is untested :) - fuzzie
	 */

	valid_agent(targ);
	
	Point src = targ->boundingBoxPoint(direction);
	Point dest = src;

	switch (direction) {
		case 0: // left
			dest.x -= targ->range.getFloat(); break;
		case 1: // right
			dest.x += targ->range.getFloat(); break;
		case 2: // top
			dest.y -= targ->range.getFloat(); break;
		case 3: // bottom
			dest.y += targ->range.getFloat(); break;
	}

	shared_ptr<Room> ourRoom = world.map.roomAt(src.x, src.y);
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
 OBST (integer) direction (integer)
 %status maybe
 %pragma variants c2
 %pragma implementation caosVM::v_OBST_c2

 Returns the distance from the TARG agent to the nearest wall that it might collide with in the given direction.
*/
void caosVM::v_OBST_c2() {
	VM_PARAM_INTEGER(direction)
	
	valid_agent(targ);

	int dx = 0, dy = 0;
	switch (direction) {
		case 0: dx = -10000; break; // left
		case 1: dx = 10000; break; // right
		case 2: dy = -10000; break; // up
		case 3: dy = 10000; break; // down
		default: caos_assert(false);
	}

	Point src = targ->boundingBoxPoint(direction);

	Point deltapt(0,0);
	double delta = 1000000000;
	bool collided = false;

	MetaRoom *m = world.map.metaRoomAt(targ->x, targ->y);
	caos_assert(m);

	targ->findCollisionInDirection(direction, m, src, dx, dy, deltapt, delta, collided, false);

	if (!collided)
		result.setInt(INT_MAX);
	else
		result.setInt(abs(deltapt.x + deltapt.y)); // only one will be set
}

/**
 TMVB (integer) deltax (float) deltay (float)
 %status maybe

 Returns 1 if the TARG agent could move by (deltax, deltay) and still be in room system, or 0 otherwise.
*/
void caosVM::v_TMVB() {
	VM_PARAM_FLOAT(deltay)
	VM_PARAM_FLOAT(deltax)

	valid_agent(targ);

	if (targ->validInRoomSystem(Point(targ->x + deltax, targ->y + deltay), targ->getWidth(), targ->getHeight(), targ->perm))
		result.setInt(1);
	else
		result.setInt(0);
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

	valid_agent(targ);

	if (targ->validInRoomSystem(Point(x, y), targ->getWidth(), targ->getHeight(), targ->perm))
		result.setInt(1);
	else
		result.setInt(0);
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

	valid_agent(targ);
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

	valid_agent(targ);
	targ->accg = accel;
}

/**
 ACCG (float)
 %status maybe
 
 Returns the TARG agent's free-fall acceleration, in pixels/tick squared.
*/
void caosVM::v_ACCG() {
	VM_VERIFY_SIZE(0)

	valid_agent(targ);
	result.setFloat(targ->accg.getFloat());
}

/**
 ACCG (variable)
 %status maybe
 %pragma variants c2
 %pragma implementation caosVM::v_ACCG_c2
 %pragma saveimpl caosVM::s_ACCG_c2
 
 Returns the TARG agent's free-fall acceleration, in pixels/tick squared.
*/
CAOS_LVALUE_TARG_SIMPLE(ACCG_c2, targ->accg)

/**
 AERO (command) aero (float)
 %status maybe

 Sets the aerodynamics of the TARG agent to the given float value.
*/
void caosVM::c_AERO() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(aero)

	valid_agent(targ);
	targ->aero = aero;
}

/**
 AERO (float)
 %status maybe

 Returns the aerodynamics of the TARG agent.
*/
void caosVM::v_AERO() {
	VM_VERIFY_SIZE(0)
	
	valid_agent(targ);
	result.setFloat(targ->aero.getFloat());
}

/**
 AERO (variable)
 %status maybe
 %pragma variants c2
 %pragma implementation caosVM::v_AERO_c2
 %pragma saveimpl caosVM::s_AERO_c2

 Returns the aerodynamics of the TARG agent.
*/
CAOS_LVALUE_TARG_SIMPLE(AERO_c2, targ->aero)

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
 RELX (integer)
 %status maybe
 %pragma variants c2
 %pragma implementation caosVM::v_RELX_c2

 Returns the relative horizontal distance between the script owner and the target agent.
*/
void caosVM::v_RELX_c2() {
	valid_agent(targ);
	valid_agent(owner);

	// TODO: correct?
	result.setInt((int)targ->x + (targ->getWidth() / 2) - ((int)owner->x + (owner->getWidth() / 2)));
}

/**
 RELY (integer)
 %status maybe
 %pragma variants c2
 %pragma implementation caosVM::v_RELY_c2
 
 Returns the relative vertical distance between the script owner and the target agent.
*/
void caosVM::v_RELY_c2() {
	valid_agent(targ);
	valid_agent(owner);

	// TODO: correct?
	result.setInt((int)targ->y + (targ->getHeight() / 2) - ((int)owner->y + (owner->getHeight() / 2)));
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

	valid_agent(targ);
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
	valid_agent(targ);

	if (!targ->tryMoveToPlaceAround(x, y))
		throw creaturesException(boost::str(boost::format("MVSF failed to find a safe place around (%d, %d)") % x % y));	
}

/**
 FRIC (float)
 %status maybe

 Returns the TARG agent's coefficient of friction as a percentage.
*/
void caosVM::v_FRIC() {
	VM_VERIFY_SIZE(0)
	
	valid_agent(targ);
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

	valid_agent(targ);
	targ->friction = friction;
}

/**
 FALL (integer)
 %status maybe

 Returns 1 if the TARG agent is moving due to gravity, or 0 if otherwise.
*/
void caosVM::v_FALL() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);

	// XXX: this probably isn't quite correct, but it's close enough for now.
	if (targ->falling)
		result.setInt(1);
	else
		result.setInt(0);
}

/**
 MOVS (variable)
 %status maybe
 %pragma variants c1 c2 cv c3 sm

 Returns an integer representing the motion status of the TARG agent.  0 is autonomous, 1 is moving by mouse, 2 is 
 floating, 3 is inside a vehicle, and 4 is being carried.
*/
void caosVM::v_MOVS() {
	// TODO: check these values match c1

	valid_agent(targ);

	caosVar r;

	// TODO: agents can possibly have multiple MOVS states right now, we should make sure to avoid that
	if (targ->carriedby) {
		if (targ->carriedby.get() == (Agent *)world.hand())
			r = 1;
		else
			r = 4;
	} else if (targ->invehicle)
		r = 3;
	else if (targ->floatable()) // TODO: good?
		r = 2;
	else
		r = 0;
	
	valueStack.push_back(r);
}
void caosVM::s_MOVS() {
	VM_PARAM_VALUE(newvalue)
	caos_assert(newvalue.hasInt());
	valid_agent(targ);
	
	// TODO: implement MOVS setting
}

/**
 FLTO (command) x (float) y (float)
 %status maybe

 Sets the TARG agent to float its top-left corner (x, y) away from the top-left corner of the FREL agent.
*/
void caosVM::c_FLTO() {
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	valid_agent(targ);
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

	valid_agent(targ);
	targ->floatTo(agent);
}

/**
 FLTX (float)
 %status maybe

 Returns the x value of the TARG agent's floating vector.
*/
void caosVM::v_FLTX() {
	valid_agent(targ);

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
	valid_agent(targ);
	
	if (targ->floatingagent)
		result.setFloat(targ->floatingagent->x - targ->x);
	else
		result.setFloat(world.camera.getX() - targ->x);
}

/**
 MCRT (command) x (integer) y (integer)
 %status stub
 %pragma variants c1 c2

 Remove limits from target object and move it to (x, y).
*/
void caosVM::c_MCRT() {
	c_MVTO(); // TODO
}

/**
 REST (variable)
 %status maybe
 %pragma variants c2
*/
CAOS_LVALUE_TARG_SIMPLE(REST, targ->rest)

/* vim: set noet: */
