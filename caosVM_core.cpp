/*
 *  caosVM_core.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
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

using std::cout;
using std::cerr;

/**
 OUTS (command) val (string)
*/
void caosVM::c_OUTS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(val)

	cout << val;
}

/**
 OUTV (command) val (decimal)
*/
void caosVM::c_OUTV() {
  VM_VERIFY_SIZE(1)
	VM_PARAM_DECIMAL(val)

	if (val.hasFloat()) {
		cout << val.floatValue;
	} else {
		cout << val.intValue;
	}
}

/**
	GAME (variable) name (string)

	returns game variable with name given (unchecked)
*/
void caosVM::v_GAME() {
	// todo: ;-)
	static caosVar gamehack;
	result = gamehack;
	result.setVariable(&gamehack);
}

/**
 SCRP (command)
*/
void caosVM::c_SCRP() {
	cerr << "hit SCRP. shouldn't ever happen.\n";
}

/**
 RSCR (command)
*/
void caosVM::c_RSCR() {
	cerr << "hit RSCR. shouldn't ever happen.\n";
}

/**
 ENDM (command)
*/
void caosVM::c_ENDM() {
	c_STOP();
}
