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
#include "openc2e.h"
#include <iostream>

using std::cout;
using std::cerr;

/**
 OUTS (command) val (string)
*/
void caosVM::c_OUTS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(val)

	// TODO
	for (unsigned int i = 0; i < val.size(); i++) {
		if (val[i] == '\\') {
			assert((i + 1) < val.size());
			i++;
			if (val[i] == 'n') {
				cout << '\n';
			} else {
				cout << val[i];
			}
		} else {
			cout << val[i];
		}
	}
}

/**
 OUTV (command) val (decimal)
*/
void caosVM::c_OUTV() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_DECIMAL(val)

	if (val.hasFloat()) {
		// TODO: DS spits things like '4.000000' and '-5.000000', we don't
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
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(name)
	// todo: ;-)
	static caosVar gamehack;
	result = gamehack;
	result.setVariable(&gamehack);
}

/**
 SCRP (command) family (integer) genus (integer) species (integer) event (integer)
*/
void caosVM::c_SCRP() {
	VM_VERIFY_SIZE(4)
	// TODO: throw something more intimidating
	cerr << "hit SCRP. shouldn't ever happen.\n";
}

/**
 RSCR (command)
*/
void caosVM::c_RSCR() {
	VM_VERIFY_SIZE(0)
	cerr << "hit RSCR. shouldn't ever happen.\n";
}

/**
 ENDM (command)
*/
void caosVM::c_ENDM() {
	VM_VERIFY_SIZE(0)
	c_STOP();
}
