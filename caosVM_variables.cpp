/*
 *  caosVM_variables.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon May 31 2004.
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
#include <stdlib.h> // rand()

/*
 VAxx (variable)

 script-local variables, 00 to 99 (todo: is this right?)
 */
void caosVM::v_VAxx() {
	VM_VERIFY_SIZE(0)
	result = var[0];
	result.setVariable(&var[0]); // todo: not just zero
}

/*
 SETV (command) var (variable) value (decimal)

 sets given variable to given number [int/float]
 */
void caosVM::c_SETV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(value)
	VM_PARAM_VARIABLE(var)
	var->reset();
	if (value.hasFloat()) {
		var->setFloat(value.floatValue);
	} else { // VM_PARAM_DECIMAL guarantees us float || int
		var->setInt(value.intValue);
	}
	var->notifyChanged(); // do we need this? if so, stick it everywhere else
}

/*
 OVxx (variable)

 agent-local variables from TARG, 00 to 99
 */
void caosVM::v_OVxx() {
	VM_VERIFY_SIZE(0)
	// COUGH COUGH BROKEN (var != OVxx)
	result = var[0];
	result.setVariable(&var[0]); // todo: not just zero
}

void caosVM::c_MODV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(mod)
	VM_PARAM_VARIABLE(v) // integer
	if (!v->hasInt()) throw badParamException();
	v->setInt(v->intValue % mod);
}

void caosVM::c_ADDV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(add)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat())
		v->setFloat(v->floatValue + (add.hasFloat() ? add.floatValue : add.intValue));
	else if (v->hasInt())
	  v->setInt((int)(v->intValue + (add.hasFloat() ? add.floatValue : add.intValue)));
	else
		throw badParamException();
}

void caosVM::c_SUBV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(sub)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat())
		v->setFloat(v->floatValue - (sub.hasFloat() ? sub.floatValue : sub.intValue));
	else if (v->hasInt())
	  v->setInt((int)(v->intValue - (sub.hasFloat() ? sub.floatValue : sub.intValue)));
	else
		throw badParamException();
}

void caosVM::c_NEGV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat())
		v->setFloat(-v->floatValue);
	else if (v->hasInt())
		v->setInt(-v->intValue);
	else
		throw badParamException();
}

/*
  MULV (command) var (variable) mul (decimal)
*/
void caosVM::c_MULV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(mul)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat())
		v->setFloat(v->floatValue * (mul.hasFloat() ? mul.floatValue : mul.intValue));
	else if (v->hasInt())
	  v->setInt((int)(v->intValue * (mul.hasFloat() ? mul.floatValue : mul.intValue)));
	else
		throw badParamException();
}

/*
 RAND (integer) value1 (integer) value2 (integer)

 return a random integer between value1 and value2 inclusive
 */
void caosVM::v_RAND() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(value2)
	VM_PARAM_INTEGER(value1)

	int diff = value2 - value1 + 1;
	double r = (rand() / (double)((unsigned int)RAND_MAX + 1));

	result.setInt((int)((r * diff) + value1));

	// todo: we should call srand at the start of our code somewhere
}

/*
 REAF (command)

 reread catalogue files
 */
void caosVM::c_REAF() {
	VM_VERIFY_SIZE(0)
	// todo
}
