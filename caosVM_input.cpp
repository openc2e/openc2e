/*
 *  caosVM_input.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on 28/07/2004.
 *  Copyright 2004 Alyssa Milburn. All rights reserved.
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
#include <iostream>
using std::cerr;

/**
 CLAC (command) message (integer)
 */
void caosVM::c_CLAC() {
  VM_VERIFY_SIZE(1)
  VM_PARAM_INTEGER(message)

  caos_assert(targ);

  targ->clac[0] = calculateScriptId(message);
  targ->clik = -1;
}

/**
 CLIK (command) msg1 (integer) msg2 (integer) msg3 (integer)
*/
void caosVM::c_CLIK() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(msg3)
	VM_PARAM_INTEGER(msg2)
	VM_PARAM_INTEGER(msg1)

	caos_assert(targ);
	
	targ->clac[0] = calculateScriptId(msg1);
	targ->clac[1] = calculateScriptId(msg2);
	targ->clac[2] = calculateScriptId(msg3);
	targ->clik = 0;
}

