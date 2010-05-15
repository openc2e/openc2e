/*
 *  caosVM_genetics.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Fri Dec 9 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
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
#include "World.h"
#include "creatures/CreatureAgent.h"
#include <fstream>

/**
 GENE CLON (command) dest_agent (agent) dest_slot (integer) src_agent (agent) src_slot (integer)
 %status stub
 
 Clone a genome. A new moniker is created.
*/
void caosVM::c_GENE_CLON() {
	VM_PARAM_INTEGER(src_slot)
	VM_PARAM_VALIDAGENT(src_agent)
	VM_PARAM_INTEGER(dest_slot)
	VM_PARAM_VALIDAGENT(dest_agent)

	// TODO
}

/**
 GENE CROS (command) dest_agent (agent) dest_slot (integer) mum_agent (agent) mum_slot (integer) dad_agent (agent) dad_slot (integer) mum_mutation_chance (integer) mum_mutation_degree (integer) dad_mutation_chance (integer) dad_mutation_degree (integer)
 %status stub

 Cross two genomes, creating a new one.
*/
void caosVM::c_GENE_CROS() {
	VM_PARAM_INTEGER(dad_mutation_degree)
	VM_PARAM_INTEGER(dad_mutation_chance)
	VM_PARAM_INTEGER(mum_mutation_degree)
	VM_PARAM_INTEGER(mum_mutation_chance)
	VM_PARAM_INTEGER(dad_slot)
	VM_PARAM_VALIDAGENT(dad_agent)
	VM_PARAM_INTEGER(mum_slot)
	VM_PARAM_VALIDAGENT(mum_agent)
	VM_PARAM_INTEGER(dest_slot)
	VM_PARAM_VALIDAGENT(dest_agent)

	// TODO
}

/**
 GENE KILL (command) agent (agent) slot (integer)
 %status stub

 Delete a genome from a slot.
*/
void caosVM::c_GENE_KILL() {
	VM_PARAM_INTEGER(slot)
	VM_PARAM_VALIDAGENT(agent)

	// TODO
}

/**
 GENE LOAD (command) agent (agent) slot (integer) genefile (string)
 %status maybe

 Load a genome file into a slot. You can use * and ? wildcards in the filename.
*/
void caosVM::c_GENE_LOAD() {
	VM_PARAM_STRING(genefile)
	VM_PARAM_INTEGER(slot)
	VM_PARAM_VALIDAGENT(agent)

	shared_ptr<genomeFile> p = world.loadGenome(genefile);
	if (!p)
		throw creaturesException("failed to find genome file '" + genefile + "'");
	
	caos_assert(p->getVersion() == 3);

	agent->genome_slots[slot] = p;
	world.newMoniker(p, genefile, agent);
}

/**
 GENE MOVE (command) dest_agent (agent) dest_slot (integer) src_agent (agent) src_slot (integer)
 %status maybe

 Move a genome to another slot.
*/
void caosVM::c_GENE_MOVE() {
	VM_PARAM_INTEGER(src_slot)
	VM_PARAM_VALIDAGENT(src_agent)
	VM_PARAM_INTEGER(dest_slot)
	VM_PARAM_VALIDAGENT(dest_agent)

	std::map<unsigned int, shared_ptr<class genomeFile> >::iterator i = src_agent->genome_slots.find(src_slot);
	caos_assert(i != src_agent->genome_slots.end());

	std::string moniker = world.history.findMoniker(i->second);
	assert(moniker != std::string("")); // internal consistency, i think..

	dest_agent->genome_slots[dest_slot] = src_agent->genome_slots[src_slot];
	src_agent->genome_slots.erase(i);
	world.history.getMoniker(moniker).moveToAgent(dest_agent);
}

/**
 GTOS (string) slot (integer)
 %status maybe
 
 Return the moniker stored in the given gene slot of the target agent.
*/
void caosVM::v_GTOS() {
	VM_PARAM_INTEGER(slot)

	valid_agent(targ);
	if (targ->genome_slots.find(slot) == targ->genome_slots.end()) {
		result.setString(""); // CV needs this, at least
	} else {
		shared_ptr<class genomeFile> g = targ->genome_slots[slot];
		result.setString(world.history.findMoniker(g));
	}
}

/**
 MTOA (agent) moniker (string)
 %status maybe

 Return the agent which has the given moniker stored in a gene slot, or NULL if none.
*/
void caosVM::v_MTOA() {
	VM_PARAM_STRING(moniker)

	caos_assert(world.history.hasMoniker(moniker));
	result.setAgent(world.history.getMoniker(moniker).owner);
}

/**
 MTOC (agent) moniker (string)
 %status maybe

 Return the live creature with the given moniker, or NULL if none.
*/
void caosVM::v_MTOC() {
	VM_PARAM_STRING(moniker)

	result.setAgent(0);
	if (!world.history.hasMoniker(moniker)) return;
	Agent *a = world.history.getMoniker(moniker).owner;
	if (!a) return;
	CreatureAgent *c = dynamic_cast<CreatureAgent *>(a);
	assert(c); // TODO: is this assert valid? can history events have non-creature owners?
	result.setAgent(a);
}

/**
 NEW: GENE (command) mum (integer) dad (integer) destination (variable)
 %status stub
 %pragma variants c1 c2
*/
void caosVM::c_NEW_GENE() {
	VM_PARAM_VARIABLE(destination)
	VM_PARAM_INTEGER(dad)
	VM_PARAM_INTEGER(mum)

	destination->setInt(mum); // TODO
}

/* vim: set noet: */
