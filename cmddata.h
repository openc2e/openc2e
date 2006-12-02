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
#ifndef CMDDATA_H
#define CMDDATA_H 1

class caosVM;

enum ci_type {
	CI_OTHER = -1,
	CI_END = 0,
	CI_NUMERIC,
	CI_STRING,
	CI_AGENT,
	CI_VARIABLE,
	CI_BYTESTR,
	CI_VECTOR,
	CI_BAREWORD
};

struct cmdinfo {
//	void (caosVM::*handler)();
	int disp_id;
	const char *key;
	const char *name;
	const char *fullname;
	const char *docs;
	int argc;
	int retc;
	const enum ci_type *argtypes;
	int evalcost;
};

void registerAutoDelegates();
void dispatchCAOS(class caosVM *vm, int idx);

#endif


/* vim: set noet: */
