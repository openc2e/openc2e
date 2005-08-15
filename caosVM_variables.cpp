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
#include "Agent.h"
#include <stdlib.h> // rand()
#include <iostream>
#include <math.h> // abs()/fabs()
#include "openc2e.h"

/**
 VAxx (variable)
 %pragma noparse

 script-local variables, 00 to 99
 */

/**
 MVxx (variable)
 %pragma noparse
 %status maybe

 Like OVxx, only for OWNR, not TARG.
 */

/**
 ADDS (command) var (variable) value (string)

 var = var + value (ie, concaternate stings)
*/
void caosVM::c_ADDS() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_STRING(value)
	VM_PARAM_VARIABLE(variable)

	assert(variable->hasString());
	variable->setString(variable->stringValue + value);
}

/**
 SETS (command) var (variable) value (string)

 sets given variable to given string
*/
void caosVM::c_SETS() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_STRING(value)
	VM_PARAM_VARIABLE(var)
	var->reset();
	var->setString(value);
}

/**
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
}

/**
 SETA (command) var (variable) value (agent)

 sets given variable to given agent
*/
void caosVM::c_SETA() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_AGENT(value)
	VM_PARAM_VARIABLE(var)
	var->reset();
	var->setAgent(value);
}

/**
 OVxx (variable)
 %pragma noparse

 agent-local variables from TARG, 00 to 99
 */
// TODO: OVxx parser

/**
 TYPE (integer) value (anything)
*/
void caosVM::v_TYPE() {
	VM_PARAM_VALUE(value)

	caos_assert(!value.isEmpty());

	if (value.hasInt())
		result.setInt(0);
	else if (value.hasFloat())
		result.setInt(1);
	else if (value.hasString())
		result.setInt(2);
	else if (value.hasAgent()) {
		// TODO
		result.setInt(3);
	}
}

/**
 MODV (command) var (variable) mod (integer)

 returns var % mod
*/
void caosVM::c_MODV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(mod)
	VM_PARAM_VARIABLE(v) // integer
	if (!v->hasInt()) throw badParamException();
	v->setInt(v->intValue % mod);
}

/**
 ANDV (command) var (variable) and (integer)

 returns var & and
*/
void caosVM::c_ANDV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(andv)
	VM_PARAM_VARIABLE(v)
	if (!v->hasInt()) throw badParamException();
	v->setInt(v->intValue & andv);
}

/**
 ORRV (command) var (variable) or (integer)

 returns var | or
*/
void caosVM::c_ORRV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(orv)
	VM_PARAM_VARIABLE(v)
	if (!v->hasInt()) throw badParamException();
	v->setInt(v->intValue | orv);
}


/**
 ADDV (command) var (variable) add (integer)

 returns var + add
*/
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

/**
 SUBV (command) var (variable) sub (integer)

 returns var - sub
*/
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

/**
 NEGV (command) var (variable)

 returns a negated variable (eg 1 to -1, -3 to 3)
*/
void caosVM::c_NEGV() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat())
		v->setFloat(-v->floatValue);
	else if (v->hasInt())
		v->setInt(-v->intValue);
	else
		throw badParamException();
}

/**
 DIVV (command) var (variable) div (decimal)
 
 returns var / mul
*/
void caosVM::c_DIVV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(div)
	VM_PARAM_VARIABLE(v)
	if (v->hasInt() && div.hasInt()) {
		// integer division
		v->setInt(v->intValue / div.intValue);
	} else if (v->hasInt() || v->hasFloat()) {
		// floating point division
		v->setFloat((v->hasFloat() ? v->floatValue : v->intValue) /
					(div.hasFloat() ? div.floatValue : v->intValue));
	} else
		throw badParamException();
}

/**
  MULV (command) var (variable) mul (decimal)

  returns var * mul
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

/**
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

/**
 REAF (command)

 reread catalogue files
 */
void caosVM::c_REAF() {
	VM_VERIFY_SIZE(0)
	// todo
}

/**
 UFOS (string)
 
 return 'uname -a' on platforms which support it, or OS details in another format otherwise
*/
void caosVM::v_UFOS() {
	VM_VERIFY_SIZE(0)
	result.setString("some random platform"); // TODO
}

/**
 MODU (string)
 
 returns Docking Station engine string for now, should return modules loaded and display engine type
*/
void caosVM::v_MODU() {
	VM_VERIFY_SIZE(0)
	result.setString("OriginalDisplay SDL (netbabel 148)");
}

/**
 GNAM (string)
 
 returns running game (eg "Docking Station")
*/
void caosVM::v_GNAM() {
	VM_VERIFY_SIZE(0)
	result.setString("Docking Station"); // todo
}

/**
 ABSV (command) var (decimal variable)
 
 modifies var if negative so that its value is positive
*/
void caosVM::c_ABSV() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VARIABLE(var)
	
	if (var->hasFloat()) var->setFloat(fabs(var->floatValue));
	else if (var->hasInt()) var->setInt(abs(var->intValue));
	else throw badParamException();
}

// TODO: all the below functions actually work in radians right now ;)

/**
 ACOS (float) x (float)
 
 returns arccosine of x in degrees <font color="#ff0000">(currently radians, which is incorrect)</font>
*/
void caosVM::v_ACOS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	result.setFloat(acos(x));
}

/**
 ASIN (float) x (float)
 
 returns arcsine of x in degrees <font color="#ff0000">(currently radians, which is incorrect)</font>
*/
void caosVM::v_ASIN() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	result.setFloat(asin(x));
}

/**
 ATAN (float) x (float)
 
 returns arctangent of x in degrees <font color="#ff0000">(currently radians, which is incorrect)</font>
*/
void caosVM::v_ATAN() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	result.setFloat(atan(x));
}

/**
 COS_ (float) x (float)
 
 returns cosine of x in degrees <font color="#ff0000">(currently radians, which is incorrect)</font>
*/
void caosVM::v_COS_() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	result.setFloat(cos(x));
}

/**
 SIN_ (float) x (float)
 
 returns sine of x in degrees <font color="#ff0000">(currently radians, which is incorrect)</font>
*/
void caosVM::v_SIN_() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	result.setFloat(sin(x));
}

/**
 TAN_ (float) x (float)
 
 returns tangent of x in degrees <font color="#ff0000">(currently radians, which is incorrect)</font>
*/
void caosVM::v_TAN_() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	result.setFloat(tan(x));
}

/**
 SQRT (float) v (float)
 
 returns square root of v
*/
void caosVM::v_SQRT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	result.setFloat(sqrt(x));
}

/**
 _P1_ (variable)
 %pragma implementation caosVM::v_P1
*/
void caosVM::v_P1() {
	VM_VERIFY_SIZE(0)

	vm->valueStack.push_back(&_p_[0]);
}

/**
 _P2_ (variable)
 %pragma implementation caosVM::v_P2
*/
void caosVM::v_P2() {
	VM_VERIFY_SIZE(0)
		
	vm->valueStack.push_back(&_p_[1]);
}

/**
 AVAR (variable) agent (agent) index (integer)
*/
void caosVM::v_AVAR() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(index)
	VM_PARAM_AGENT(agent)

	assert(index > -1);
	assert(index < 100);
	assert(agent);
	result = agent->var[index];
	returnVariable(agent->var[index]);
}

/**
 VTOS (string) value (decimal)
*/
void caosVM::v_VTOS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_DECIMAL(value)

	// TODO: this is hacky!
	char buffer[20];

	if (value.hasInt()) {
		sprintf(buffer, "%i", value.intValue);
	} else {
		caos_assert(value.hasFloat());
		// TODO: this format isn't right (see OUTS also)
		sprintf(buffer, "0%f", value.floatValue);
	}

	result.setString(buffer);
}

/**
 CHAR (integer) str (string) index (integer)
*/
void caosVM::v_CHAR() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(index)
	VM_PARAM_STRING(str)

	caos_assert(str.size() >= index);
	
	result.setInt(str[index - 1]);
}

/* vim: set noet: */
