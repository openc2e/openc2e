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

#include "Agent.h"
#include "AgentRef.h"
#include "Camera.h" // FLTX/FLTY
#include "Engine.h" // version
#include "Map.h"
#include "World.h"
#include "caosVM.h"
#include "caos_assert.h"

#include <climits>
#include <fmt/core.h>
#include <memory>

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

/**
 ELAS (command) elas (integer)
 %status maybe

 Sets the elasticity (in other words, bounciness) of the TARG agent.
*/
void c_ELAS(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(elas)

	valid_agent(vm->targ);
	vm->targ->elas = elas;
}

/***
 ELAS (integer)
 %status maybe

 Returns the elasticity of the TARG agent.
*/
void v_ELAS(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->elas);
}

/**
 MVTO (command) x (float) y (float)
 %status maybe
 %variants c1 c2 cv c3 sm

 Places the TARG agent at the given x/y position in the world (using the upper left hand corner of the agent).
*/
void c_MVTO(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)
	valid_agent(vm->targ);
	vm->targ->moveTo(x, y);
}

/**
 MVBY (command) x (float) y (float)
 %status maybe
 %variants c1 c2 cv c3 sm

 Changes the TARG agent's position by the given relative distances.
*/
void c_MVBY(caosVM* vm) {
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	valid_agent(vm->targ);
	vm->targ->moveTo(vm->targ->x + x, vm->targ->y + y);
}

/**
 VELX (variable)
 %status maybe
 %variants c2 cv c3 sm

 Returns the current horizontal velocity, in pixels/tick, of the TARG agent.
*/
void v_VELX(caosVM* vm) {
	valid_agent(vm->targ);
	vm->valueStack.push_back(caosValue(vm->targ->velx));
}
void s_VELX(caosVM* vm) {
	VM_PARAM_FLOAT(newvalue)
	valid_agent(vm->targ);
	vm->targ->velx = newvalue;
	vm->targ->falling = true;
}

/**
 VELY (variable)
 %status maybe
 %variants c2 cv c3 sm

 Returns the current vertical velocity, in pixels/tick, of the TARG agent.
*/
void v_VELY(caosVM* vm) {
	valid_agent(vm->targ);
	vm->valueStack.push_back(caosValue(vm->targ->vely));
}
void s_VELY(caosVM* vm) {
	VM_PARAM_FLOAT(newvalue)
	valid_agent(vm->targ);
	vm->targ->vely = newvalue;
	vm->targ->falling = true;
}

/**
 OBST (float) direction (integer)
 %status maybe
 %variants cv c3 sm

 Returns the distance from the TARG agent to the nearest wall that it might collide with in the given direction.
 (except right now it just gives the direction to the nearest wall at world edge - fuzzie)
*/
void v_OBST(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(direction)
	caos_assert(direction >= 0);
	caos_assert(direction <= 3);

	/*
	 * TODO: CL's docs say to return "a very large number" if distance is greater than rnge - if (!collided)?
	 * TODO: c2 docs say "from the centre point of TARG".. same in c2e? or do we need a separate function?
	 * also, this code is untested :) - fuzzie
	 */

	valid_agent(vm->targ);

	Point src = vm->targ->boundingBoxPoint(direction);
	Point dest = src;

	switch (direction) {
		case 0: // left
			dest.x -= vm->targ->range;
			break;
		case 1: // right
			dest.x += vm->targ->range;
			break;
		case 2: // top
			dest.y -= vm->targ->range;
			break;
		case 3: // bottom
			dest.y += vm->targ->range;
			break;
	}

	std::shared_ptr<Room> ourRoom = world.map->roomAt(src.x, src.y);
	if (!ourRoom) {
		// TODO: is this correct behaviour?
		vm->result.setFloat(0.0f);
		return;
	}

	unsigned int dummy1;
	Line dummy2;
	Point point;
	bool collided = world.map->collideLineWithRoomSystem(src, dest, ourRoom, point, dummy2, dummy1, vm->targ->perm);
	// TODO: do something with collided?
	(void)collided;

	switch (direction) {
		case 0: vm->result.setFloat(src.x - point.x); break;
		case 1: vm->result.setFloat(point.x - src.x); break;
		case 2: vm->result.setFloat(src.y - point.y); break;
		case 3: vm->result.setFloat(point.y - src.y); break;
	}
}

/**
 OBST (integer) direction (integer)
 %status maybe
 %variants c2

 Returns the distance from the TARG agent to the nearest wall that it might collide with in the given direction.
*/
void v_OBST_c2(caosVM* vm) {
	VM_PARAM_INTEGER(direction)

	valid_agent(vm->targ);

	int dx = 0, dy = 0;
	switch (direction) {
		case 0: dx = -10000; break; // left
		case 1: dx = 10000; break; // right
		case 2: dy = -10000; break; // up
		case 3: dy = 10000; break; // down
		default: caos_assert(false);
	}

	Point src = vm->targ->boundingBoxPoint(direction);

	Point deltapt(0, 0);
	double delta = 1000000000;
	bool collided = false;

	MetaRoom* m = world.map->metaRoomAt(vm->targ->x, vm->targ->y);
	caos_assert(m);

	vm->targ->findCollisionInDirection(direction, m, src, dx, dy, deltapt, delta, collided, false);

	if (!collided)
		vm->result.setInt(INT_MAX);
	else
		vm->result.setInt(abs(deltapt.x + deltapt.y)); // only one will be set
}

/**
 OBSV (integer) direction (integer)
 %status stub
 %variants c2

 Returns the permeability of the first door the TARG agent would collide with in
 the given direction.
*/
void v_OBSV(caosVM* vm) {
	VM_PARAM_INTEGER(direction)

	valid_agent(vm->targ);
	// TODO: implement
	vm->result.setInt(INT_MAX);
}

/**
 TMVB (integer) deltax (float) deltay (float)
 %status maybe

 Returns 1 if the TARG agent could move by (deltax, deltay) and still be in room system, or 0 otherwise.
*/
void v_TMVB(caosVM* vm) {
	VM_PARAM_FLOAT(deltay)
	VM_PARAM_FLOAT(deltax)

	valid_agent(vm->targ);

	if (vm->targ->validInRoomSystem(Point(vm->targ->x + deltax, vm->targ->y + deltay), vm->targ->getWidth(), vm->targ->getHeight(), vm->targ->perm))
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 TMVT (integer) x (float) y (float)
 %status maybe
 
 Returns 1 if the TARG agent could move to (x, y) and still be in room system, or 0 if otherwise.
*/
void v_TMVT(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	valid_agent(vm->targ);

	if (vm->targ->validInRoomSystem(Point(x, y), vm->targ->getWidth(), vm->targ->getHeight(), vm->targ->perm))
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 TMVF (integer) x (float) y (float)
 %status stub
 
 Returns 1 if the TARG Creature could move foot to (x, y) and still be in room system, or 0 if otherwise.
*/
void v_TMVF(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	valid_agent(vm->targ);
	vm->result.setInt(1); // TODO: don't hardcode
}

/**
 ACCG (command) accel (float)
 %status maybe

 Sets the TARG agent's free-fall acceleration, in pixels/tick squared.
*/
void c_ACCG(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(accel)

	valid_agent(vm->targ);
	vm->targ->accg = accel;

	vm->targ->falling = true;
}

/**
 ACCG (float)
 %status maybe
 
 Returns the TARG agent's free-fall acceleration, in pixels/tick squared.
*/
void v_ACCG(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setFloat(vm->targ->accg);
}

/**
 ACCG (variable)
 %status maybe
 %variants c2
 
 Returns the TARG agent's free-fall acceleration, in pixels/tick squared.
*/
CAOS_LVALUE_TARG_SIMPLE(ACCG_c2, vm->targ->accg)

/**
 AERO (command) aero (float)
 %status maybe

 Sets the aerodynamics of the TARG agent to the given float value.
*/
void c_AERO(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(aero)

	valid_agent(vm->targ);
	vm->targ->aero = aero;
}

/**
 AERO (float)
 %status maybe

 Returns the aerodynamics of the TARG agent.
*/
void v_AERO(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setFloat(vm->targ->aero);
}

/**
 AERO (variable)
 %status maybe
 %variants c2

 Returns the aerodynamics of the TARG agent.
*/
CAOS_LVALUE_TARG_SIMPLE(AERO_c2, vm->targ->aero)

/**
 RELX (float) first (agent) second (agent)
 %status maybe

 Returns the relative horizontal distance between the centers of the two given agents.
*/
void v_RELX(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_VALIDAGENT(second)
	VM_PARAM_VALIDAGENT(first)

	float one = first->x + (first->getWidth() / 2.0);
	float two = second->x + (second->getWidth() / 2.0);

	vm->result.setFloat(two - one);
}

/**
 RELY (float) first (agent) second (agent)
 %status maybe

 Returns the relative vertical distance between the centers of the two given agents.
*/
void v_RELY(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_VALIDAGENT(second)
	VM_PARAM_VALIDAGENT(first)

	float one = first->y + (first->getHeight() / 2.0);
	float two = second->y + (second->getHeight() / 2.0);

	vm->result.setFloat(two - one);
}

/**
 RELX (integer)
 %status maybe
 %variants c2

 Returns the relative horizontal distance between the script owner and the target agent.
*/
void v_RELX_c2(caosVM* vm) {
	valid_agent(vm->targ);
	valid_agent(vm->owner);

	// TODO: correct?
	vm->result.setInt((int)vm->targ->x + (vm->targ->getWidth() / 2) - ((int)vm->owner->x + (vm->owner->getWidth() / 2)));
}

/**
 RELY (integer)
 %status maybe
 %variants c2
 
 Returns the relative vertical distance between the script owner and the target agent.
*/
void v_RELY_c2(caosVM* vm) {
	valid_agent(vm->targ);
	valid_agent(vm->owner);

	// TODO: correct?
	vm->result.setInt((int)vm->targ->y + (vm->targ->getHeight() / 2) - ((int)vm->owner->y + (vm->owner->getHeight() / 2)));
}

/**
 VELO (command) xvel (float) yvel (float)
 %status maybe

 Sets the horizontal and vertical velocity of the TARG agent, in pixels/tick.
*/
void c_VELO(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(vely)
	VM_PARAM_FLOAT(velx)

	valid_agent(vm->targ);
	vm->targ->velx = velx;
	vm->targ->vely = vely;

	vm->targ->falling = true;
}

/**
 MVSF (command) x (float) y (float)
 %status maybe

 Move the target agent to an area inside the room system at about (x, y).
 This allows 'safe' moves.
*/
void c_MVSF(caosVM* vm) {
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)
	valid_agent(vm->targ);

	if (!vm->targ->tryMoveToPlaceAround(x, y))
		throw creaturesException(fmt::format("MVSF failed to find a safe place around ({}, {})", x, y));
}

/**
 FRIC (float)
 %status maybe

 Returns the TARG agent's coefficient of friction as a percentage.
*/
void v_FRIC(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setFloat(vm->targ->friction);
}

/**
 FRIC (command) friction (integer)
 %status maybe
 
 Sets the TARG agent's coefficient of friction, or the percentage of motion that will be lost as it slides on a 
 surface.
*/
void c_FRIC(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(friction)
	caos_assert(friction >= 0);
	caos_assert(friction <= 100);

	valid_agent(vm->targ);
	vm->targ->friction = friction;
}

/**
 FALL (integer)
 %status maybe

 Returns 1 if the TARG agent is moving due to gravity, or 0 if otherwise.
*/
void v_FALL(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);

	// XXX: this probably isn't quite correct, but it's close enough for now.
	if (vm->targ->falling)
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 MOVS (variable)
 %status maybe
 %variants c1 c2 cv c3 sm

 Returns an integer representing the motion status of the TARG agent.  0 is autonomous, 1 is moving by mouse, 2 is 
 floating, 3 is inside a vehicle, and 4 is being carried.
*/
void v_MOVS(caosVM* vm) {
	// TODO: check these values match c1

	valid_agent(vm->targ);

	caosValue r;

	// TODO: agents can possibly have multiple MOVS states right now, we should make sure to avoid that
	if (vm->targ->carriedby) {
		if (vm->targ->carriedby.get() == (Agent*)world.hand())
			r = 1;
		else
			r = 4;
	} else if (vm->targ->invehicle)
		r = 3;
	else if (vm->targ->floatable()) // TODO: good?
		r = 2;
	else
		r = 0;

	vm->valueStack.push_back(r);
}
void s_MOVS(caosVM* vm) {
	VM_PARAM_VALUE(newvalue)
	caos_assert(newvalue.hasInt());
	valid_agent(vm->targ);

	// TODO: implement MOVS setting
}

/**
 FLTO (command) x (float) y (float)
 %status maybe

 Sets the TARG agent to float its top-left corner (x, y) away from the top-left corner of the FREL agent.
*/
void c_FLTO(caosVM* vm) {
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	valid_agent(vm->targ);
	vm->targ->floatTo(x, y);
}

/**
 FREL (command) agent (agent)
 %status maybe

 Sets the agent the TARG agent floats relative to. You must set the 'floatable' attribute for this to work.
 The default is NULL, which means the target agent floats relative to the main camera.
*/
void c_FREL(caosVM* vm) {
	VM_PARAM_AGENT(agent)

	valid_agent(vm->targ);
	vm->targ->floatTo(agent);
}

/**
 FLTX (float)
 %status maybe

 Returns the x value of the TARG agent's floating vector.
*/
void v_FLTX(caosVM* vm) {
	valid_agent(vm->targ);

	if (vm->targ->floatingagent)
		vm->result.setFloat(vm->targ->floatingagent->x - vm->targ->x);
	else
		vm->result.setFloat(engine.camera->getX() - vm->targ->x);
}

/**
 FLTY (float)
 %status maybe

 Returns the y value of the TARG agent's floating vector.
*/
void v_FLTY(caosVM* vm) {
	valid_agent(vm->targ);

	if (vm->targ->floatingagent)
		vm->result.setFloat(vm->targ->floatingagent->x - vm->targ->x);
	else
		vm->result.setFloat(engine.camera->getX() - vm->targ->x);
}

/**
 MCRT (command) x (integer) y (integer)
 %status stub
 %variants c1 c2

 Remove limits from target object and move it to (x, y).
*/
void c_MCRT(caosVM* vm) {
	c_MVTO(vm); // TODO
}

/**
 REST (variable)
 %status maybe
 %variants c2
*/
CAOS_LVALUE_TARG_SIMPLE(REST, vm->targ->rest)

/**
 AVEL (command) angularvelocity (float)
 %status maybe

 Set the angular velocity for the target agent.
*/
void c_AVEL(caosVM* vm) {
	VM_PARAM_FLOAT(angularvelocity)

	valid_agent(vm->targ);

	vm->targ->avel = angularvelocity;
}

/**
 AVEL (float)
 %status maybe

 Returns the angular velocity for the target agent.
*/
void v_AVEL(caosVM* vm) {
	valid_agent(vm->targ);

	vm->result.setFloat(vm->targ->avel);
}

/**
 FVEL (command) forwardvelocity (float)
 %status maybe

 Set the forward velocity for the target agent.
*/
void c_FVEL(caosVM* vm) {
	VM_PARAM_FLOAT(forwardvelocity)

	valid_agent(vm->targ);
	vm->targ->fvel = forwardvelocity;
}

/**
 FVEL (float)
 %status maybe

 Returns the forward velocity for the target agent.
*/
void v_FVEL(caosVM* vm) {
	valid_agent(vm->targ);

	vm->result.setFloat(vm->targ->fvel);
}

/**
 SVEL (command) sidewaysvelocity (float)
 %status maybe

 Set the sideways velocity for the target agent.
*/
void c_SVEL(caosVM* vm) {
	VM_PARAM_FLOAT(sidewaysvelocity)

	valid_agent(vm->targ);
	vm->targ->svel = sidewaysvelocity;
}

/**
 SVEL (float)
 %status maybe

 Returns the sideways velocity for the target agent.
*/
void v_SVEL(caosVM* vm) {
	valid_agent(vm->targ);

	vm->result.setFloat(vm->targ->svel);
}

/**
 ADMP (command) angulardamping (float)
 %status maybe

 Set the angular damping (0.0 to 1.0, fraction to damp per tick) for the target agent.
*/
void c_ADMP(caosVM* vm) {
	VM_PARAM_FLOAT(angulardamping)
	caos_assert(angulardamping >= 0.0f);
	caos_assert(angulardamping <= 1.0f);

	valid_agent(vm->targ);
	vm->targ->admp = angulardamping;
}

/**
 ADMP (float)
 %status maybe

 Returns the angular damping (0.0 to 1.0, fraction to damp per tick) for the target agent.
*/
void v_ADMP(caosVM* vm) {
	valid_agent(vm->targ);

	vm->result.setFloat(vm->targ->admp);
}

/**
 FDMP (command) forwarddamping (float)
 %status maybe

 Set the forward damping (0.0 to 1.0, fraction to damp per tick) for the target agent.
*/
void c_FDMP(caosVM* vm) {
	VM_PARAM_FLOAT(forwarddamping)
	caos_assert(forwarddamping >= 0.0f);
	caos_assert(forwarddamping <= 1.0f);

	valid_agent(vm->targ);
	vm->targ->fdmp = forwarddamping;
}

/**
 FDMP (float)
 %status maybe

 Returns the forward damping (0.0 to 1.0, fraction to damp per tick) for the target agent.
*/
void v_FDMP(caosVM* vm) {
	valid_agent(vm->targ);

	vm->result.setFloat(vm->targ->fdmp);
}

/**
 SDMP (command) sidewaysdamping (float)
 %status maybe

 Set the sideways damping (0.0 to 1.0, fraction to damp per tick) for the target agent.
*/
void c_SDMP(caosVM* vm) {
	VM_PARAM_FLOAT(sidewaysdamping)
	caos_assert(sidewaysdamping >= 0.0f);
	caos_assert(sidewaysdamping <= 1.0f);

	valid_agent(vm->targ);
	vm->targ->sdmp = sidewaysdamping;
}

/**
 SDMP (float)
 %status maybe

 Returns the sideways damping (0.0 to 1.0, fraction to damp per tick) for the target agent.
*/
void v_SDMP(caosVM* vm) {
	valid_agent(vm->targ);

	vm->result.setFloat(vm->targ->sdmp);
}

/**
 SPIN (command) angle (float)
 %status maybe

 Set the angle (from 0.0 to 1.0) the target agent is facing.
 TODO: This seems to not affect velocity in original c2e, sometimes. Needs more investigation.
*/
void c_SPIN(caosVM* vm) {
	VM_PARAM_FLOAT(angle)

	valid_agent(vm->targ);
	angle = fmodf(angle, 1.0f);
	if (angle < 0.0f)
		angle += 0.0f;
	vm->targ->spin = angle;
}

/**
 SPIN (float)
 %status maybe

 Returns the angle (from 0.0 to 1.0) the target agent is facing.
*/
void v_SPIN(caosVM* vm) {
	valid_agent(vm->targ);

	vm->result.setFloat(vm->targ->spin);
}

/**
 ANGL (float) x (float) y (float)
 %status maybe

 Calculates the angle (from 0.0 to 1.0) between the target agent and the specified coordinates.
 TODO: This seems not to work in original c2e unless there's been an angular calculation on the target agent. Needs more investigation.
*/
void v_ANGL(caosVM* vm) {
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	valid_agent(vm->targ);
	float srcx = vm->targ->x + (vm->targ->getWidth() / 2.0f);
	float srcy = vm->targ->y + (vm->targ->getHeight() / 2.0f);

	float distx = x - srcx;
	float disty = y - srcy;

	if (disty == 0) {
		if (distx > 0)
			vm->result.setFloat(0.25f);
		else if (distx < 0)
			vm->result.setFloat(0.75f);
		else
			vm->result.setFloat(0.0f);
		return;
	}

	if (disty < 0) {
		if (distx < 0)
			vm->result.setFloat(0.75f + atanf(distx / disty) * 0.25f / (M_PI / 2.0f));
		else
			vm->result.setFloat(atanf(distx / -disty) * 0.25f / (M_PI / 2.0f));
	} else {
		if (distx > 0)
			vm->result.setFloat(0.25f + atanf(distx / disty) * 0.25f / (M_PI / 2.0f));
		else
			vm->result.setFloat(0.5f + atanf(-distx / disty) * 0.25f / (M_PI / 2.0f));
	}
}

/**
 ROTN (command) sprites (integer) rotations (integer)
 %status maybe

 Enable automatic sprite adjustments for the current agent (by changing the BASE of part 0), based on the current rotation (SPIN).
 'sprites' specifies the number of sprites for each rotation.
 'rotations' specifies the number of different angles provided in the sprite file. They should begin facing north and continue clockwise.
 The total number of sprites used will be sprites * rotations.
*/
void c_ROTN(caosVM* vm) {
	VM_PARAM_INTEGER(rotations)
	caos_assert(rotations > 0);
	VM_PARAM_INTEGER(sprites)
	caos_assert(sprites > 0);

	valid_agent(vm->targ);
	// TODO: sanity checking
	vm->targ->spritesperrotation = sprites;
	vm->targ->numberrotations = rotations;
}

/* vim: set noet: */
