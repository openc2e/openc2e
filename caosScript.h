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

class Agent;

struct caosVar {
	unsigned char flags;
	int intValue;
	float floatValue;
	std::string stringValue;
	Agent *agentValue;
	caosVar *variableValue;

	void reset() { flags = 0; }
	caosVar() { reset(); }
	//virtual ~caosVar() { } // we don't truly need this.

	bool empty() { return (flags == 0); }
	bool hasInt() { return (flags & 1); }
	void setInt(int i) { flags = flags | 1; intValue = i; }
	bool hasFloat() { return (flags & 2); }
	void setFloat(float i) { flags = flags | 2; floatValue = i; }
	bool hasString() { return (flags & 4); }
	void setString(std::string i) { flags = flags | 4; stringValue = i; }
	bool hasAgent() { return (flags & 8); }
	void setAgent(Agent *i) { flags = flags | 8; agentValue = i; }
	bool hasVariable() { return (flags & 16); }
	void setVariable(caosVar *i) { flags = flags | 16; variableValue = i; }
	//virtual void notifyChanged() { }

	bool operator == (caosVar &v);
	bool operator != (caosVar &v) { return !(*this == v); }
	bool operator > (caosVar &v);
	bool operator < (caosVar &v);

	std::string dump();
};

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
};

struct script {
	std::vector<std::list<token> > lines;
	std::vector<std::string> rawlines;

	std::string dump();
};

class caosScript {
public:
	script installer, removal;
	std::vector<script> scripts;

	caosScript(std::istream &);

	std::string dump();
};

#endif
