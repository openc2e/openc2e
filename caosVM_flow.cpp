/*
 *  caosVM_flow.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun May 30 2004.
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
#include <iostream>

void caosVM::c_DOIF() {
	VM_VERIFY_SIZE(0)
	// todo ;-) .. remember 'truth' holds the comparison result
	std::cerr << "if we handled DOIF, we " << (truth ? "would" : "wouldn't") << " execute the code here\n";
}

void caosVM::c_ELSE() {
	VM_VERIFY_SIZE(0)
}

void caosVM::c_ENDI() {
	VM_VERIFY_SIZE(0)
	std::cerr << "we hit endif\n";
}

void caosVM::c_REPS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(reps)
}

void caosVM::c_REPE() {
	VM_VERIFY_SIZE(0)
}
