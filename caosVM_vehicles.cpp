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

/**
 CABN (command) left (integer) top (integer) right (integer) bottom (integer)
*/
void caosVM::c_CABN() {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(bottom)
	VM_PARAM_INTEGER(right)
	VM_PARAM_INTEGER(top)
	VM_PARAM_INTEGER(left)

	assert(targ); // TODO: check it's a Vehicle
	((Vehicle *)targ)->setCabinRect(left, top, right, bottom);
}

/**
 CABW (command) cap (integer)
*/
void caosVM::c_CABW() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(cap)

	assert(targ); // TODO: check it's a Vehicle
	((Vehicle *)targ)->setCapacity(cap);
}

