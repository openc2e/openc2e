/*
 *  caosVM.h
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

#ifndef CAOSSCRIPT_H
#define CAOSSCRIPT_H

#include <vector>
#include <list>
#include <string>
#include <istream>
#include "caosVar.h"

class Agent;

enum comparisonType {
	NONE, EQ, NE, GT, GE, LT, LE, AND, OR
};

struct token {
	enum { CAOSVAR, FUNCTION, BYTESTRING, COMPARISON, POSSIBLEFUNC, LABEL } type;

	// CAOSVAR
	caosVar var;

	// FUNCTION
	struct cmdinfo *func;
	signed char varnumber;

	// BYTESTRING
	unsigned int len;
	unsigned char *bytes;

	// COMPARISON
	comparisonType comparison;

	// POSSIBLEFUNC/LABEL
	std::string data;

	std::string dump();

	token() { }
	token(const token &t) {
		var = t.var;
		func = t.func;
		varnumber = t.varnumber;
		len = t.len;
		bytes = t.bytes;
		comparison = t.comparison;
		data = t.data;
		type = t.type;
	}
};

struct script {
	std::vector<std::list<token> > lines;

	std::string dump();
	std::string dumpLine(unsigned int);
};

class caosScript {
public:
	script installer, removal;
	std::vector<script> scripts;

	caosScript(std::istream &);

	std::string dump();
};

#endif
