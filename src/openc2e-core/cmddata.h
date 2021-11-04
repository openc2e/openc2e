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

#include <stdint.h>

class caosVM;

enum ci_type : uint8_t {
	CI_OTHER = (uint8_t)-1,
	CI_COMMAND = 1,
	CI_AGENT = 'a',
	CI_BYTESTR = 'b',
	CI_CONDITION = 'c',
	CI_NUMERIC = 'd',
	CI_ANYVALUE = 'm',
	CI_STRING = 's',
	CI_VARIABLE = 'v',
	CI_VECTOR = 'u',
	CI_BAREWORD = '#',
	CI_SUBCOMMAND = '*',
	CI_FACEVALUE = '%',
};

struct cmdinfo {
	void (*handler)(caosVM*);
	void (*savehandler)(caosVM*);

	const char* lookup_key;
	const char* name;
	const char* fullname;
	const char* docs;
	int argc;
	int stackdelta;
	const enum ci_type* argtypes;
	enum ci_type rettype;
};

/* vim: set noet: */
