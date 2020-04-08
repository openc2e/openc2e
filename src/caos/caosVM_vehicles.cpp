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

#include "caosVM.h"
#include <iostream>
#include "openc2e.h"
#include "Vehicle.h"
#include "World.h"
#include "AgentHelpers.h" // agentsTouching

/**
 CABN (command) left (integer) top (integer) right (integer) bottom (integer)
 %status maybe
 %pragma variants c1 c2 cv c3
*/
void caosVM::c_CABN() {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(bottom)
	VM_PARAM_INTEGER(right)
	VM_PARAM_INTEGER(top)
	VM_PARAM_INTEGER(left)

	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);
	v->setCabinRect(left, top, right, bottom);
}

/**
 CABW (command) cap (integer)
 %status maybe
*/
void caosVM::c_CABW() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(cap)

	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);
	v->setCapacity(cap);
}

/**
 SPAS (command) vehicle (agent) passenger (agent)
 %status maybe
 %pragma variants c1 c2 cv c3

 make specified vehicle agent pick up specified passenger
*/
void caosVM::c_SPAS() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_AGENT(passenger)
	VM_PARAM_AGENT(vehicle)

	valid_agent(vehicle);
	valid_agent(passenger);
	
	// TODO: ensure passenger is a creature for c1/c2?

	Vehicle *v = dynamic_cast<Vehicle *>(vehicle.get());
	caos_assert(v);
	v->addCarried(passenger);
}

/**
 GPAS (command) family (integer) genus (integer) species (integer) options (integer)
 %status maybe

 pick up all nearby agents matching classifier, as passengers to target vehicle
 options = 0 to pick up based on agent bounding rect, or 1 to pick up based on cabin rect
*/
void caosVM::c_GPAS() {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(options)
	// TODO: assert ranges for these
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);
	
	// TODO: see other GPAS below
	// TODO: are we sure c2e grabs passengers by agent rect?
	// TODO: do we need to check greedycabin attr for anything?
	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		std::shared_ptr<Agent> a = (*i);
		if (!a) continue;
		if (a.get() == v) continue;
		if (family && family != a->family) continue;
		if (genus && genus != a->genus) continue;
		if (species && species != a->species) continue;

		if (agentsTouching(a.get(), v)) {
			v->addCarried(a);
		}
	}
}

/**
 GPAS (command)
 %status maybe
 %pragma variants c1 c2
 %pragma implementation caosVM::c_GPAS_c2
*/
void caosVM::c_GPAS_c2() {
	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);

	// TODO: are we sure c1/c2 grab passengers by agent rect?
	// TODO: do we need to check greedycabin attr for anything?
	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		std::shared_ptr<Agent> a = (*i);
		if (!a) continue;
		if (a.get() == v) continue;
		if (a->family != 4) continue; // only pickup creatures (TODO: good check?)

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
void caosVM::c_DPAS() {
	VM_VERIFY_SIZE(3)
	// TODO: assert ranges for these
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);

	for (unsigned int i = 0; i < v->passengers.size(); i++) {
		AgentRef a = v->passengers[i];
		if (!a) continue;
		if (a.get() == v) continue;
		if (family && family != a->family) continue;
		if (genus && genus != a->genus) continue;
		if (species && species != a->species) continue;
		v->drop(a);
		i--; // since we dropped the last one out of the list
	}
}

/**
 DPAS (command)
 %status maybe
 %pragma variants c1 c2
 %pragma implementation caosVM::c_DPAS_c2
*/
void caosVM::c_DPAS_c2() {
	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);

	for (unsigned int i = 0; i < v->passengers.size(); i++) {
		AgentRef a = v->passengers[i];
		if (!a) continue;
		if (a.get() == v) continue;
		if (a->family != 4) continue; // only drop creatures (TODO: good check?)
		v->drop(a);
		i--; // since we dropped the last one out of the list
	}
}

/**
 CABP (command) plane (integer)
 %status maybe
*/
void caosVM::c_CABP() {
	VM_PARAM_INTEGER(plane)
	
	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);

	v->cabinplane = plane;
}

/**
 CABV (command) room_id (integer)
 %status stub
*/
void caosVM::c_CABV() {
	VM_PARAM_INTEGER(room_id)
	
	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);
	
	// TODO
}

/**
 CABV (integer)
 %status stub
*/
void caosVM::v_CABV() {
	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);

	result.setInt(-1); // TODO
}

/**
 RPAS (command) vehicle (agent) passenger (agent)
 %status maybe
*/
void caosVM::c_RPAS() {
	VM_PARAM_AGENT(passenger)
	VM_PARAM_AGENT(vehicle)

	Vehicle *v = dynamic_cast<Vehicle *>(vehicle.get());
	caos_assert(v);

	for (unsigned int i = 0; i < v->passengers.size(); i++) {
		AgentRef a = v->passengers[i];
		if (!a) continue;
		if (a == passenger) {
			v->drop(a);
			return;
		}
	}
}

/**
 XVEC (variable)
 %status stub
 %pragma variants c1 c2
*/
CAOS_LVALUE_TARG(XVEC,
		Vehicle *v = dynamic_cast<Vehicle *>(targ.get()); caos_assert(v)
	,	v->xvec
	,	v->xvec = newvalue
	)

/**
 YVEC (variable)
 %status stub
 %pragma variants c1 c2
*/
CAOS_LVALUE_TARG(YVEC,
		Vehicle *v = dynamic_cast<Vehicle *>(targ.get()); caos_assert(v)
	,	v->yvec
	,	v->yvec = newvalue
	)

/**
 BUMP (integer)
 %status stub
 %pragma variants c1 c2
*/
void caosVM::v_BUMP() {
	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);

	result.setInt(v->getBump());
}

/**
 TELE (command) x (integer) y (integer)
 %status stub
 %pragma variants c1 c2

 Teleport occupants of target vehicle to (x, y).
*/
void caosVM::c_TELE() {
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);

	// TODO
}

/**
 DPS2 (command) gravity (integer)
 %status stub
 %pragma variants c2

 Drop passengers of targ vehicle, like DPAS. If gravity is zero, passengers do not get gravity activated.
*/
void caosVM::c_DPS2() {
	VM_PARAM_INTEGER(gravity)

	valid_agent(targ);
	Vehicle *v = dynamic_cast<Vehicle *>(targ.get());
	caos_assert(v);

	// TODO
}

/* vim: set noet: */
