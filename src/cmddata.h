/*
 *  cmddata.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Thu 11 Aug 2005.
 *  Copyright (c) 2005-2006 Bryan Donlan. All rights reserved.
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

#pragma once

#include "caosVM.h"
#include "is_complete.h"

enum ci_type {
	CI_OTHER = -1,
	CI_END = 0,
	CI_COMMAND,
	CI_NUMERIC,
	CI_STRING,
	CI_AGENT,
	CI_VARIABLE,
	CI_BYTESTR,
	CI_VECTOR,
	CI_BAREWORD,
	CI_SUBCOMMAND,
	CI_ANYVALUE
};

struct cmdinfo {
	// Microsoft Visual C++ can use different representations of member function
	// pointers, which causes issues when we take pointers before the class is
	// fully defined. So, MAKE ABSOLUTELY SURE that caosVM is defined if we're
	// delaring/taking pointers to caos functions.
	// See https://social.msdn.microsoft.com/Forums/en-US/3c065ad7-a6cc-460a-8114-25b5ee01c76f/stack-corruption-when-passing-pointer-to-member-function-of-a-forward-referenced-class
	// for more information.
	static_assert(is_complete<caosVM>, "caosVM must be defined before member function pointers are declared");
	void (caosVM::*handler)();
	void (caosVM::*savehandler)();

	const char *lookup_key;
	const char *key;
	const char *name;
	const char *fullname;
	const char *docs;
	int argc;
	int stackdelta;
	const enum ci_type *argtypes;
	enum ci_type rettype;
	int evalcost;
};

/* vim: set noet: */
