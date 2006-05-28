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
#include "Creature.h"
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
 %status stub

 Load a genome file into a slot. You can use * and ? wildcards in the filename.
*/
void caosVM::c_GENE_LOAD() {
	VM_PARAM_STRING(genefile)
	VM_PARAM_INTEGER(slot)
	VM_PARAM_VALIDAGENT(agent)

	// TODO: wildcards
	
	std::string gfilename = world.findFile(std::string("/Genetics/") + genefile + ".gen");
	if (gfilename.empty())
		throw creaturesException("failed to find genome file '" + genefile + '"');
	
	shared_ptr<genomeFile> p(new genomeFile());

	std::ifstream gfile(gfilename.c_str(), std::ios::binary);
	caos_assert(gfile.is_open());
	gfile >> std::noskipws;

	gfile >> *(p.get());

	agent->slots[slot] = p;
	std::string d = world.history.newMoniker(p);
	world.history.getMoniker(d).addEvent(2, "", genefile);
	world.history.getMoniker(d).moveToAgent(agent);
}

/**
 GENE MOVE (command) dest_agent (agent) dest_slot (integer) src_agent (agent) src_slot (integer)
 %status stub

 Move a genome to another slot.
*/
void caosVM::c_GENE_MOVE() {
	VM_PARAM_INTEGER(src_slot)
	VM_PARAM_VALIDAGENT(src_agent)
	VM_PARAM_INTEGER(dest_slot)
	VM_PARAM_VALIDAGENT(dest_agent)

	// TODO
}

/**
 GTOS (string) slot (integer)
 %status maybe
 
 Return the moniker stored in the given gene slot of the target agent.
*/
void caosVM::v_GTOS() {
	VM_PARAM_INTEGER(slot)

	valid_agent(targ);
	caos_assert(targ->slots.find(slot) != targ->slots.end());
	shared_ptr<class genomeFile> g = targ->slots[slot];
	result.setString(world.history.findMoniker(g));
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

	caos_assert(world.history.hasMoniker(moniker));
	Agent *a = world.history.getMoniker(moniker).owner;
	result.setAgent(dynamic_cast<Creature *>(a));
}

/* vim: set noet: */
