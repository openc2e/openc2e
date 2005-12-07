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

/**
 PRT: BANG (command) strength (integer)
 %status stub
*/
void caosVM::c_PRT_BANG() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(strength)
}

/**
 PRT: FRMA (agent) inputport (integer)
 %status stub
*/
void caosVM::v_PRT_FRMA() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(inputport)
}

/**
 PRT: FROM (integer) inputport (integer)
 %status stub
*/
void caosVM::v_PRT_FROM() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(inputport)
}

/**
 PRT: INEW (command) id (integer) name (string) desc (string) x (integer) y (integer) msgnum (integer)
 %status stub
*/
void caosVM::c_PRT_INEW() {
	VM_VERIFY_SIZE(6)
	VM_PARAM_INTEGER(msgnum)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_STRING(desc)
	VM_PARAM_STRING(name)
	VM_PARAM_INTEGER(id)
}

/**
 PRT: ITOT (integer)
 %status stub
*/
void caosVM::v_PRT_ITOT() {
	VM_VERIFY_SIZE(0)
}

/**
 PRT: IZAP (command) id (integer)
 %status stub
*/
void caosVM::c_PRT_IZAP() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(id)
}

/**
 PRT: JOIN (command) source (agent) outputport (integer) dest (agent) inputport (integer)
 %status stub
*/
void caosVM::c_PRT_JOIN() {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(inputport)
	VM_PARAM_AGENT(dest)
	VM_PARAM_INTEGER(outputport)
	VM_PARAM_AGENT(source)
}

/**
 PRT: KRAK (command) agent (agent) in_or_out (integer) port (integer)
 %status stub
*/
void caosVM::c_PRT_KRAK() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(port)
	VM_PARAM_INTEGER(in_or_out)
	VM_PARAM_AGENT(agent)
}

/**
 PRT: NAME (string) agent (agent) in_or_out (integer) port (integer)
 %status stub
*/
void caosVM::v_PRT_NAME() {
	VM_VERIFY_SIZE(0)
	VM_PARAM_INTEGER(port)
	VM_PARAM_INTEGER(in_or_out)
	VM_PARAM_AGENT(agent)
}

/**
 PRT: ONEW (command) id (integer) name (string) desc (string) x (integer) y (integer)
 %status stub
*/
void caosVM::c_PRT_ONEW() {
	VM_VERIFY_SIZE(5)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_STRING(desc)
	VM_PARAM_STRING(name)
	VM_PARAM_INTEGER(id)
}

/**
 PRT: OTOT (integer)
 %status stub
*/
void caosVM::v_PRT_OTOT() {
	VM_VERIFY_SIZE(0)
}

/**
 PRT: OZAP (command) id (integer)
 %status stub
*/
void caosVM::c_PRT_OZAP() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(id)
}

/**
 PRT: SEND (command) id (integer) data (anything)
 %status stub
*/
void caosVM::c_PRT_SEND() { // TODO
	VM_VERIFY_SIZE(2)
	VM_PARAM_VALUE(data)
	VM_PARAM_INTEGER(id)
}

/* vim: set noet: */
