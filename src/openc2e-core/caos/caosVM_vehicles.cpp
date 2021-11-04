/*
 *  caosVM_vehicles.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on 02/02/2005.
 *  Copyright 2005 Alyssa Milburn. All rights reserved.
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

#include "AgentHelpers.h" // agentsTouching
#include "Vehicle.h"
#include "World.h"
#include "caosVM.h"
#include "common/throw_ifnot.h"

#include <iostream>
#include <memory>

/**
 CABN (command) left (integer) top (integer) right (integer) bottom (integer)
 %status maybe
 %variants c1 c2 cv c3
*/
void c_CABN(caosVM* vm) {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(bottom)
	VM_PARAM_INTEGER(right)
	VM_PARAM_INTEGER(top)
	VM_PARAM_INTEGER(left)

	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);
	v->setCabinRect(left, top, right, bottom);
}

/**
 CABW (command) cap (integer)
 %status maybe
*/
void c_CABW(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(cap)

	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);
	v->setCapacity(cap);
}

/**
 SPAS (command) vehicle (agent) passenger (agent)
 %status maybe
 %variants c1 c2 cv c3

 make specified vehicle agent pick up specified passenger
*/
void c_SPAS(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_AGENT(passenger)
	VM_PARAM_AGENT(vehicle)

	valid_agent(vehicle);
	valid_agent(passenger);

	// TODO: ensure passenger is a creature for c1/c2?

	Vehicle* v = dynamic_cast<Vehicle*>(vehicle.get());
	THROW_IFNOT(v);
	v->addCarried(passenger);
}

/**
 GPAS (command) family (integer) genus (integer) species (integer) options (integer)
 %status maybe

 pick up all nearby agents matching classifier, as passengers to target vehicle
 options = 0 to pick up based on agent bounding rect, or 1 to pick up based on cabin rect
*/
void c_GPAS(caosVM* vm) {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(options)
	// TODO: assert ranges for these
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);

	// TODO: see other GPAS below
	// TODO: are we sure c2e grabs passengers by agent rect?
	// TODO: do we need to check greedycabin attr for anything?
	for (auto& agent : world.agents) {
		std::shared_ptr<Agent> a = agent;
		if (!a)
			continue;
		if (a.get() == v)
			continue;
		if (family && family != a->family)
			continue;
		if (genus && genus != a->genus)
			continue;
		if (species && species != a->species)
			continue;

		if (agentsTouching(a.get(), v)) {
			v->addCarried(a);
		}
	}
}

/**
 GPAS (command)
 %status maybe
 %variants c1 c2
*/
void c_GPAS_c2(caosVM* vm) {
	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);

	// TODO: are we sure c1/c2 grab passengers by agent rect?
	// TODO: do we need to check greedycabin attr for anything?
	for (auto& agent : world.agents) {
		std::shared_ptr<Agent> a = agent;
		if (!a)
			continue;
		if (a.get() == v)
			continue;
		if (a->family != 4)
			continue; // only pickup creatures (TODO: good check?)

		if (agentsTouching(a.get(), v)) {
			v->addCarried(a);
		}
	}
}

/**
 DPAS (command) family (integer) genus (integer) species (integer)
 %status maybe

 drop all agents matching classifier from target vehicle
*/
void c_DPAS(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	// TODO: assert ranges for these
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);

	for (unsigned int i = 0; i < v->passengers.size(); i++) {
		AgentRef a = v->passengers[i];
		if (!a)
			continue;
		if (a.get() == v)
			continue;
		if (family && family != a->family)
			continue;
		if (genus && genus != a->genus)
			continue;
		if (species && species != a->species)
			continue;
		v->drop(a);
		i--; // since we dropped the last one out of the list
	}
}

/**
 DPAS (command)
 %status maybe
 %variants c1 c2
*/
void c_DPAS_c2(caosVM* vm) {
	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);

	for (unsigned int i = 0; i < v->passengers.size(); i++) {
		AgentRef a = v->passengers[i];
		if (!a)
			continue;
		if (a.get() == v)
			continue;
		if (a->family != 4)
			continue; // only drop creatures (TODO: good check?)
		v->drop(a);
		i--; // since we dropped the last one out of the list
	}
}

/**
 CABP (command) plane (integer)
 %status maybe
*/
void c_CABP(caosVM* vm) {
	VM_PARAM_INTEGER(plane)

	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);

	v->cabinplane = plane;
}

/**
 CABV (command) room_id (integer)
 %status stub
*/
void c_CABV(caosVM* vm) {
	VM_PARAM_INTEGER(room_id)

	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);

	// TODO
}

/**
 CABV (integer)
 %status stub
*/
void v_CABV(caosVM* vm) {
	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);

	vm->result.setInt(-1); // TODO
}

/**
 RPAS (command) vehicle (agent) passenger (agent)
 %status maybe
*/
void c_RPAS(caosVM* vm) {
	VM_PARAM_AGENT(passenger)
	VM_PARAM_AGENT(vehicle)

	Vehicle* v = dynamic_cast<Vehicle*>(vehicle.get());
	THROW_IFNOT(v);

	for (unsigned int i = 0; i < v->passengers.size(); i++) {
		AgentRef a = v->passengers[i];
		if (!a)
			continue;
		if (a == passenger) {
			v->drop(a);
			return;
		}
	}
}

/**
 XVEC (variable)
 %status stub
 %variants c1 c2
*/
CAOS_LVALUE_TARG(XVEC,
				 Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
				 THROW_IFNOT(v), v->xvec, v->xvec = newvalue)

/**
 YVEC (variable)
 %status stub
 %variants c1 c2
*/
CAOS_LVALUE_TARG(YVEC,
				 Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
				 THROW_IFNOT(v), v->yvec, v->yvec = newvalue)

/**
 BUMP (integer)
 %status stub
 %variants c1 c2
*/
void v_BUMP(caosVM* vm) {
	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);

	vm->result.setInt(v->getBump());
}

/**
 TELE (command) x (integer) y (integer)
 %status stub
 %variants c1 c2

 Teleport occupants of target vehicle to (x, y).
*/
void c_TELE(caosVM* vm) {
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);

	// TODO
}

/**
 DPS2 (command) gravity (integer)
 %status stub
 %variants c2

 Drop passengers of targ vehicle, like DPAS. If gravity is zero, passengers do not get gravity activated.
*/
void c_DPS2(caosVM* vm) {
	VM_PARAM_INTEGER(gravity)

	valid_agent(vm->targ);
	Vehicle* v = dynamic_cast<Vehicle*>(vm->targ.get());
	THROW_IFNOT(v);

	// TODO
}

/* vim: set noet: */
