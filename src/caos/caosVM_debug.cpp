/*
 *  caosVM_debug.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Oct 24 2004.
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

#include "caos_assert.h"
#include "caosVM.h"
#include "Agent.h"
#include "World.h"
#include <iostream>
#include <memory>
#include "cmddata.h"
#include <cctype>
#include "dialect.h"
#include <algorithm>
#include "caosScript.h"
#include "Scriptorium.h"

// #include "malloc.h" <- unportable horror!
#include <sstream>
#include <fmt/core.h>

/**
 DBG: OUTS (command) val (string)
 %status maybe
 %pragma variants all

 Outputs a string to the debug log.
*/
/**
 DBGM (command) val (string)
 %status maybe
 %pragma variants c1 c2
*/
void caosVM::c_DBG_OUTS() {
	VM_PARAM_STRING(val)
	
	fmt::print("{}\n", val);
}

/**
 DBG: OUTV (command) val (decimal)
 %status maybe
 %pragma variants all
 
 Outputs a decimal value to the debug log.
*/
/**
 DBGV (command) val (integer)
 %status maybe
 %pragma variants c1 c2
*/
void caosVM::c_DBG_OUTV() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VALUE(val)

	if (val.hasFloat()) {
		fmt::print("{:0.06f}", val.getFloat());
	} else if (val.hasInt()) {
		fmt::print("{}", val.getInt());
	} else if (val.hasVector()) {
		const Vector<float> &v = val.getVector();
		fmt::print("({:0.6f}, {:0.6f})", v.x, v.y);
	} else throw badParamException();
	fmt::print("\n");
}

/**
 DBUG (command) val (integer)
 %status maybe
 %pragma variants c1 c2
*/
void caosVM::c_DBUG() {
	inst = true;
	c_DBG_OUTV();
}

/**
 UNID (integer)
 %status maybe
 %pragma variants c3 cv sm

 Returns the unique ID of the target agent.
 This is currently no good for persisting.

XXX: when serialization support works, this might well become good for
	 persisting :)
*/
void caosVM::v_UNID() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);
	result.setInt(targ->getUNID());
}

/**
 UNID (agent)
 %status maybe
 %pragma variants c2

 Returns the unique ID of the target agent.
 This is currently no good for persisting.
*/
void caosVM::v_UNID_c2() {
	VM_VERIFY_SIZE(0)
	valid_agent(targ);
	result.setAgent(targ);
}

/**
 AGNT (agent) id (integer)
 %status maybe

 Returns the agent with the given UNID, or NULL if agent has been deleted.
*/
void caosVM::v_AGNT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(id)

	result.setAgent(world.lookupUNID(id));
}

/**
 DBG: MALLOC (command)
 %status stub

 Dumps some random memory stats to stderr.
*/
void caosVM::c_DBG_MALLOC() {
	VM_VERIFY_SIZE(0)
	
	// more unportable horror!
/*	struct mallinfo mi = mallinfo();
#define MPRINT(name) \
	fprintf(stderr, "%10s = %d\n", #name, mi. name)
	MPRINT(arena);
	MPRINT(ordblks);
	MPRINT(smblks);
	MPRINT(hblks);
	MPRINT(hblkhd);
	MPRINT(usmblks);
	MPRINT(fsmblks);
	MPRINT(uordblks);
	MPRINT(fordblks);
	MPRINT(keepcost);
	malloc_stats(); */
	
	/*std::cerr << "caosSlab free=" << caosValueSlab.free_elements() <<
				 " used=" << caosValueSlab.used_elements() <<
				 " total=" << caosValueSlab.total_elements() <<
				 std::endl;*/
}
	
/**
 DBG: DUMP (command)
 %status ok
 %pragma variants all

 Dumps the current script's bytecode to stderr.
*/
void caosVM::c_DBG_DUMP() {
	std::cerr << vm->currentscript->dump();
}	

/**
 DBG: TRACE (command) level (integer)
 %status ok
 %pragma variants all

 Sets opcode trace level. Zero disables.
*/
void caosVM::c_DBG_TRACE() {
	VM_PARAM_INTEGER(en)

	std::cerr << "trace: " << en << std::endl;
	vm->trace = en;
	if (vm->trace < 0)
		vm->trace = 0;
}

/**
 MANN (command) cmd (string)
 %status stub
 
 Looks up documentation on the given command and spits it on the current output stream.
*/
void caosVM::c_MANN() {
	VM_PARAM_STRING(cmd)

	caos_assert(outputstream);

	std::transform(cmd.begin(), cmd.end(), cmd.begin(), toupper);
	const cmdinfo *i = currentscript->dialect->cmdbase();
	
	bool found = false;
	while (i->lookup_key) {
		// TODO: this doesn't work for FACE at the moment due to hack elsewhere
		if (cmd == i->fullname) {
			found = true;
			std::string d = i->docs;
			// TODO: docs should always include name/parameters/etc, so should never be empty
			if (d.size())
				*outputstream << std::string(i->docs) << std::endl;
			else
				*outputstream << "no documentation for " << cmd << std::endl << std::endl;
		}
	
		i++;
	}

	if (!found) {
		*outputstream << "didn't find " << cmd << std::endl;
		return;
	}
}

/**
 DBG: DISA (command) family (integer) genus (integer) species (integer) event (integer)
 %pragma variants all
 %status ok

 Dumps the "bytecode" of the indicated script to the current output channel.
 Note that this isn't really bytecode yet (though that's a possible future
 improvement).

 If the script is not found no output will be generated.
 */
void caosVM::c_DBG_DISA() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	
	caos_assert(outputstream);

	std::shared_ptr<script> s = world.scriptorium->getScript(family, genus, species, event);
	if (s) {
		if (s->fmly != family || s->gnus != genus || s->spcs != species) {
			*outputstream << "warning: search resulted in script from " << s->fmly << ", " << s->gnus << ", " << s->spcs << " script" << std::endl;
		}
		*outputstream << s->dump();
	} else
		*outputstream << "no such script" << std::endl;
}

/**
 DBG: ASRT (command) condition (condition)
 %pragma parser new AssertParser()
 %pragma variants all
 %status maybe

 Blows up unless the given condition is true.
*/
void caosVM::c_DBG_ASRT() {
	throw caosException("DBG: ASRT condition failed");
}

/**
 DBG: ASRF (command) condition (condition)
 %pragma variants all
 %status maybe

 (openc2e-only)
 Blows up unless the given condition is false.
*/
void caosVM::c_DBG_ASRF() {
	throw caosException("DBG ASRF condition succeeded");
}

/**
 DBG: FAIL (command)
 %pragma variants all
 %status maybe

 (openc2e-only)
 Blows up.
*/
void caosVM::c_DBG_FAIL() {
	throw caosException("DBG: FAIL reached");
}

/**
 DBG: IDNT (string) agent (agent)
 %status ok
 %pragma variants all

 (openc2e-only)
 Return a nicely-formatted string identifying the classifier of the agent,
 using the catalogue to find the name if possible.
*/
void caosVM::v_DBG_IDNT() {
	VM_PARAM_AGENT(a)
	
	if (!a)
		result.setString("(null)");
	else
		result.setString(a->identify());
}

/**
 DBG: PROF (command)
 %status stub

 Dumps the current agent profiling information to the output stream, in CSV format.
*/
void caosVM::c_DBG_PROF() {
	// TODO
}

/**
 DBG: CPRO (command)
 %status stub

 Clears the current agent profiling information.
*/
void caosVM::c_DBG_CPRO() {
	// TODO
}

/**
 DBG: STOK (string) bareword (bareword)
 %status ok
 %pragma variants all

 Returns the bare token in 'bareword' as a string.
*/
void caosVM::v_DBG_STOK() {
	VM_PARAM_STRING(bareword)
	
	result.setString(bareword);
}

/**
 DBG: TSLC (command) timeslice (integer)
 %status ok
 %pragma variants all
 %cost 0

 Sets the currently executing script's remaining timeslice value. This command
 affects only the current timeslice; future slices use the normal amount for
 the dialect in question.
*/
void caosVM::c_DBG_TSLC() {
	VM_PARAM_INTEGER(tslc);
	timeslice = tslc;
}

/**
 DBG: TSLC (integer)
 %status ok
 %pragma variants all
 
 Returns the number of ticks left in the current script's remaining timeslice.
*/
void caosVM::v_DBG_TSLC() {
	result.setInt(timeslice);
}

/**
DBG: SIZO (string)
 %status ok
 %pragma variants all

 Returns a human-readable profile of the sizes and allocation counts of
 various internal data structures
 */
void caosVM::v_DBG_SIZO() {
	std::ostringstream oss;
#define SIZEOF_OUT(t) do { oss << "sizeof(" #t ") = " << sizeof(t) << std::endl; } while(0)
	SIZEOF_OUT(caosVM);
	SIZEOF_OUT(caosValue);
	SIZEOF_OUT(Agent);
	SIZEOF_OUT(std::string);
	SIZEOF_OUT(AgentRef);
	SIZEOF_OUT(Vector<float>);
#ifdef PROFILE_ALLOCATION_COUNT
	AllocationCounter::walk(oss);
#else
	oss << "This build of openc2e does not have allocation profiling enabled." << std::endl;
#endif
	oss << "caosVMs in pool: " << world.vmpool_size() << std::endl;
#undef SIZEOF_OUT

	result.setString(oss.str());
}

/* vim: set noet: */
