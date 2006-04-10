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

#include "caosVM.h"
#include "openc2e.h"
#include "Agent.h"
#include "World.h"
#include <iostream>
#include "cmddata.h"
#include <cctype>
#include "dialect.h"
#include <algorithm>

// #include "malloc.h" <- unportable horror!
#include <sstream>
#include <boost/format.hpp>

using std::cerr;
using std::cout;

/**
 DBG: OUTS (command) val (string)
 %status maybe

 Outputs a string to the debug log.
*/
void caosVM::c_DBG_OUTS() {
	c_OUTS();
	cout << "\n";
}

/**
 DBG: OUTV (command) val (decimal)
 %status maybe
 
 Outputs a decimal value to the debug log.
*/
void caosVM::c_DBG_OUTV() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_DECIMAL(val)

	if (val.hasFloat()) {
		cout << boost::format("%0.06f") % val.getFloat();
	} else {
		cout << val.getInt();
	}

	cout << std::endl;
}

/**
 TEST PASS (command)
 %status stub

 (openc2e-only)
 Signals a single test pass.
*/
void caosVM::c_TEST_PASS() {
	VM_VERIFY_SIZE(0)
}

/**
 TEST FAIL (command)
 %status stub

 (openc2e-only)
 Signals a single test fail.
*/
void caosVM::c_TEST_FAIL() {
	VM_VERIFY_SIZE(0)
}

/**
 TEST INIT (command) notests (integer)
 %status stub

 (openc2e-only)
 Initialises the testing system.  There will be 'notests' tests run.
*/
void caosVM::c_TEST_INIT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(notests)
}

/**
 TEST CHEK (command) comparison (comparison)
 %status stub

 (openc2e-only)
 Signals a test pass if comparison is true, or a fail otherwise.
*/
void caosVM::c_TEST_CHEK() {
	VM_VERIFY_SIZE(0)
}

/**
 TEST STRT (command)
 %status stub

 (openc2e-only)
 Starts a single test, continuing until TEST PASS/FAIL/FINI.
*/
void caosVM::c_TEST_STRT() {
	VM_VERIFY_SIZE(0)
}

/**
 TEST FINI (command)
 %status stub

 (openc2e-only)
 Signals a single test fail if there has been no TEST/FAIL since last TEST STRT.
*/
void caosVM::c_TEST_FINI() {
	VM_VERIFY_SIZE(0)
}

/**
 UNID (integer)
 %status maybe

 Returns the unique ID of the target agent.
 This is currently no good for persisting.

XXX: when serialization support works, this might well become good for
	 persisting :)
*/
void caosVM::v_UNID() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setInt(targ->getUNID());
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
	
	/*std::cerr << "caosSlab free=" << caosVarSlab.free_elements() <<
				 " used=" << caosVarSlab.used_elements() <<
				 " total=" << caosVarSlab.total_elements() <<
				 std::endl;*/
}
	
/**
 DBG: TRACE (command) enable (integer)
 %status ok

 Enables/disables opcode tracing to cerr.
*/
void caosVM::c_DBG_TRACE() {
	VM_VERIFY_SIZE(0)
	VM_PARAM_INTEGER(en)

	vm->trace = en;
}

/**
 MANN (command) cmd (string)
 %status ok
 
 Looks up documentation on the given command and spits it on the current output socket.
*/
void caosVM::c_MANN() {
	VM_PARAM_STRING(cmd)
	transform(cmd.begin(), cmd.end(), cmd.begin(), (int(*)(int))toupper);

	const cmdinfo *cmds = NULL;
	if (currentscript) {
		const Variant *v = currentscript->getVariant();
		if (v)
			cmds = v->cmds;
	}
	if (!cmds)
		cmds = variants["c3"]->cmds;
	// we're not too worried about the performance of this...
	for (int i = 0; cmds[i].name; i++) {
		if (strcmp(cmds[i].fullname, cmd.c_str()))
			continue;
		*outputstream << cmds[i].docs;
		return;
	}
}

/**
 DBG: DISA (command) family (integer) genus (integer) species (integer) event (integer)
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

	shared_ptr<script> s = world.scriptorium.getScript(family, genus, species, event);
	if (!s) return;

	*outputstream << s->dump();
}

/**
 DBG: ASRT (command) condition (condition)
 %pragma parser new AssertParser()
 %status maybe

 Blows up unless the given condition is true.
*/

/* vim: set noet: */
