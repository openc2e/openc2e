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

#include "Agent.h"
#include "Scriptorium.h"
#include "World.h"
#include "caosScript.h"
#include "caosVM.h"
#include "cmddata.h"
#include "common/io/StringWriter.h"
#include "common/io/Writer.h"
#include "common/io/WriterFmt.h"
#include "common/throw_ifnot.h"
#include "dialect.h"

#include <algorithm>
#include <cctype>
#include <memory>

// #include "malloc.h" <- unportable horror!
#include <fmt/core.h>

/**
 DBG: OUTS (command) val (string)
 %status maybe
 %variants c3 cv sm openc2e

 Outputs a string to the debug log.
*/
/**
 DBGM (command) val (string)
 %status maybe
 %variants c1 c2
*/
void c_DBG_OUTS(caosVM* vm) {
	VM_PARAM_STRING(val)

	fmt::print("{}\n", val);
}

/**
 DBG: OUTV (command) val (decimal)
 %status maybe
 %variants c3 cv sm openc2e
 
 Outputs a decimal value to the debug log.
*/
/**
 DBGV (command) val (integer)
 %status maybe
 %variants c1 c2
*/
void c_DBG_OUTV(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VALUE(val)

	if (val.hasFloat()) {
		fmt::print("{:0.06f}", val.getFloat());
	} else if (val.hasInt()) {
		fmt::print("{}", val.getInt());
	} else if (val.hasVector()) {
		const Vector<float>& v = val.getVector();
		fmt::print("({:0.6f}, {:0.6f})", v.x, v.y);
	} else
		throw badParamException();
	fmt::print("\n");
}

/**
 DBUG (command) val (integer)
 %status maybe
 %variants c1 c2
*/
void c_DBUG(caosVM* vm) {
	vm->inst = true;
	c_DBG_OUTV(vm);
}

/**
 UNID (integer)
 %status maybe
 %variants c3 cv sm

 Returns the unique ID of the target agent.
 This is currently no good for persisting.

XXX: when serialization support works, this might well become good for
	 persisting :)
*/
void v_UNID(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->getUNID());
}

/**
 UNID (agent)
 %status maybe
 %variants c2

 Returns the unique ID of the target agent.
 This is currently no good for persisting.
*/
void v_UNID_c2(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	valid_agent(vm->targ);
	vm->result.setAgent(vm->targ);
}

/**
 AGNT (agent) id (integer)
 %status maybe

 Returns the agent with the given UNID, or NULL if agent has been deleted.
*/
void v_AGNT(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(id)

	vm->result.setAgent(world.lookupUNID(id));
}

/**
 DBG: MALLOC (command)
 %status stub
 %variants openc2e

 Dumps some random memory stats to stderr.
*/
void c_DBG_MALLOC(caosVM*) {
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
}

/**
 DBG: DUMP (command)
 %status ok
 %variants openc2e

 Dumps the current script's bytecode to stderr.
*/
void c_DBG_DUMP(caosVM* vm) {
	fmt::print(stderr, "{}", vm->currentscript->dump());
}

/**
 DBG: TRACE (command) level (integer)
 %status ok
 %variants openc2e

 Sets opcode trace level. Zero disables.
*/
void c_DBG_TRACE(caosVM* vm) {
	VM_PARAM_INTEGER(en)

	fmt::print(stderr, "trace: {}\n", en);
	vm->trace = en;
	if (vm->trace < 0)
		vm->trace = 0;
}

/**
 MANN (command) cmd (string)
 %status stub
 
 Looks up documentation on the given command and spits it on the current output stream.
*/
void c_MANN(caosVM* vm) {
	VM_PARAM_STRING(cmd)

	THROW_IFNOT(vm->outputstream);

	std::transform(cmd.begin(), cmd.end(), cmd.begin(), toupper);
	const cmdinfo* i = vm->currentscript->dialect->cmdbase();

	bool found = false;
	while (i->lookup_key) {
		if (cmd == i->fullname) {
			found = true;
			std::string d = i->docs;
			// TODO: docs should always include name/parameters/etc, so should never be empty
			if (d.size())
				fmt::print(*vm->outputstream, "{}\n", i->docs);
			else
				fmt::print(*vm->outputstream, "no documentation for {}\n\n", cmd);
		}
		i++;
	}

	if (!found) {
		fmt::print(*vm->outputstream, "didn't find {}\n", cmd);
		return;
	}
}

/**
 DBG: DISA (command) family (integer) genus (integer) species (integer) event (integer)
 %variants openc2e
 %status ok

 Dumps the "bytecode" of the indicated script to the current output channel.
 Note that this isn't really bytecode yet (though that's a possible future
 improvement).

 If the script is not found no output will be generated.
 */
void c_DBG_DISA(caosVM* vm) {
	VM_PARAM_INTEGER(event)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	THROW_IFNOT(vm->outputstream);

	std::shared_ptr<script> s = world.scriptorium->getScript(family, genus, species, event);
	if (s) {
		if (s->fmly != family || s->gnus != genus || s->spcs != species) {
			fmt::print(*vm->outputstream, "warning: search resulted in script from {}, {}, {} script\n", s->fmly, s->gnus, s->spcs);
		}
		fmt::print(*vm->outputstream, "{}", s->dump());
	} else
		fmt::print(*vm->outputstream, "no such script\n");
}

/**
 DBG: ASRT (command) condition (condition)
 %variants c3 cv sm openc2e
 %status ok

 Blows up unless the given condition is true.
*/
void c_DBG_ASRT(caosVM*) {
	throw caosException("DBG: ASRT condition failed");
}

/**
 DBG: ASRF (command) condition (condition)
 %variants openc2e
 %status ok

 (openc2e-only)
 Blows up unless the given condition is false.
*/
void c_DBG_ASRF(caosVM*) {
	throw caosException("DBG ASRF condition succeeded");
}

/**
 DBG: FAIL (command)
 %variants openc2e
 %status ok

 (openc2e-only)
 Blows up.
*/
void c_DBG_FAIL(caosVM*) {
	throw caosException("DBG: FAIL reached");
}

/**
 DBG: IDNT (string) agent (agent)
 %status ok
 %variants openc2e

 (openc2e-only)
 Return a nicely-formatted string identifying the classifier of the agent,
 using the catalogue to find the name if possible.
*/
void v_DBG_IDNT(caosVM* vm) {
	VM_PARAM_AGENT(a)

	if (!a)
		vm->result.setString("(null)");
	else
		vm->result.setString(a->identify());
}

/**
 DBG: PROF (command)
 %status stub

 Dumps the current agent profiling information to the output stream, in CSV format.
*/
void c_DBG_PROF(caosVM*) {
	// TODO
}

/**
 DBG: CPRO (command)
 %status stub

 Clears the current agent profiling information.
*/
void c_DBG_CPRO(caosVM*) {
	// TODO
}

/**
 DBG: STOK (string) bareword (bareword)
 %status ok
 %variants openc2e

 Returns the bare token in 'bareword' as a string.
*/
void v_DBG_STOK(caosVM* vm) {
	VM_PARAM_STRING(bareword)

	vm->result.setString(bareword);
}

/**
 DBG: TSLC (command) timeslice (integer)
 %status ok
 %variants openc2e

 Sets the currently executing script's remaining timeslice value. This command
 affects only the current timeslice; future slices use the normal amount for
 the dialect in question.
*/
void c_DBG_TSLC(caosVM* vm) {
	VM_PARAM_INTEGER(tslc);
	vm->timeslice = tslc;
}

/**
 DBG: TSLC (integer)
 %status ok
 %variants openc2e
 
 Returns the number of ticks left in the current script's remaining timeslice.
*/
void v_DBG_TSLC(caosVM* vm) {
	vm->result.setInt(vm->timeslice);
}

/**
DBG: SIZO (string)
 %status ok
 %variants openc2e

 Returns a human-readable profile of the sizes and allocation counts of
 various internal data structures
 */
void v_DBG_SIZO(caosVM* vm) {
	StringWriter oss;
#define SIZEOF_OUT(t) \
	do { \
		fmt::print(oss, "sizeof(" #t ") = {}\n", sizeof(t)); \
	} while (0)
	SIZEOF_OUT(caosVM);
	SIZEOF_OUT(caosValue);
	SIZEOF_OUT(Agent);
	SIZEOF_OUT(std::string);
	SIZEOF_OUT(AgentRef);
	SIZEOF_OUT(Vector<float>);
#ifdef PROFILE_ALLOCATION_COUNT
	AllocationCounter::walk(oss);
#else
	fmt::print(oss, "This build of openc2e does not have allocation profiling enabled.\n");
#endif
	fmt::print(oss, "caosVMs in pool: {}\n", world.vmpool_size());
#undef SIZEOF_OUT

	vm->result.setString(oss.string());
}

/* vim: set noet: */
