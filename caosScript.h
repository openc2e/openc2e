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
	enum variableType {
		NULLTYPE = 0, AGENT, INTEGER, FLOAT, STRING, VARREF
	};

	variableType type;

	/* XXX: we need both the variable reference and value. this 
	 * really ought not to exist.
	 */
	caosVar *variableValue;
	
	/* This is not in the union so its constructor and destructors get called
	 * at the appropriate times.
	 */
	std::string stringValue;

	union {
		int intValue;
		float floatValue;
		Agent *agentValue;
	};

	void reset() { type = NULLTYPE; stringValue.clear(); variableValue = NULL; }
	caosVar() { reset(); setInt(0); }
	//virtual ~caosVar() { } // we don't truly need this.

	//bool empty() { return (flags == 0); }
	bool hasInt() { return type == INTEGER; }
	void setInt(int i) { type = INTEGER; intValue = i; }
	bool hasFloat() { return type == FLOAT; }
	void setFloat(float i) { type = FLOAT; floatValue = i; }
	bool hasString() { return type == STRING; }
	void setString(const std::string &i) { type = STRING; stringValue = i; }
	bool hasAgent() { return type == AGENT; }
	void setAgent(Agent *i) { type = AGENT; agentValue = i; }
	bool hasVariable() { return variableValue != NULL; }
	void setVariable(caosVar *i) { variableValue = i; }
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
