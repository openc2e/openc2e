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
#include <iostream>
using std::cerr;
using std::cout;

/**
 DBG: OUTS (command) val (string)

 output a string to the debug log
*/
void caosVM::c_DBG_OUTS() {
	c_OUTS();
	cout << "\n";
}

/**
 DBG: OUTV (command) val (decimal)
 
 output a decimal value to the debug log
*/
void caosVM::c_DBG_OUTV() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_DECIMAL(val)

	if (val.hasFloat()) {
		cout << val.floatValue;
	} else {
		cout << val.intValue;
	}
}

/**
 TEST PASS (command)

 openc2e-only
 signal a single test pass
*/
void caosVM::c_TEST_PASS() {
	VM_VERIFY_SIZE(0)
}

/**
 TEST FAIL (command)

 openc2e-only
 signal a single test fail
*/
void caosVM::c_TEST_FAIL() {
	VM_VERIFY_SIZE(0)
}

/**
 TEST INIT (command) notests (integer)

 openc2e-only
 initialise the testing system, there will be notests tests
*/
void caosVM::c_TEST_INIT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(notests)
}

/**
 TEST CHEK (command) comparison (comparison)

 openc2e-only
 signal a test pass if comparison is true, or a fail otherwise
*/
void caosVM::c_TEST_CHEK() {
	VM_VERIFY_SIZE(0)
}

/**
 TEST STRT (command)

 openc2e-only
 start a single test, continuing until TEST PASS/FAIL/FINI
*/
void caosVM::c_TEST_STRT() {
	VM_VERIFY_SIZE(0)
}

/**
 TEST FINI (command)

 openc2e-only
 signal a single test fail if there has been no TEST/FAIL since last TEST STRT
*/
void caosVM::c_TEST_FINI() {
	VM_VERIFY_SIZE(0)
}

/**
 UNID (integer)

 return unique id of target agent
 this is NO GOOD for persisting!
*/
void caosVM::v_UNID() {
	VM_VERIFY_SIZE(0)
	assert(targ);
	result.setInt((int)targ); // TODO: pointers are no good, AGNT needs to be able to detect deletion!
}

/**
 AGNT (agent) id (integer)

 return agent, given input from UNID, or NULL if agent has been deleted
*/
void caosVM::v_AGNT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(id)

	result.setAgent((Agent *)id); // TODO: see UNID
}

