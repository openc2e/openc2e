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
	virtual ~caosVar() { } // we don't truly need this.

	bool hasInt() { return (flags & 0x1); }
	void setInt(int i) { flags = flags | 0x1; intValue = i; }
	bool hasFloat() { return (flags & 0x2); }
	void setFloat(float i) { flags = flags | 0x2; floatValue = i; }
	bool hasString() { return (flags & 0x4); }
	void setString(std::string i) { flags = flags | 0x4; stringValue = i; }
	bool hasAgent() { return (flags & 0x8); }
	void setAgent(Agent *i) { flags = flags | 0x8; agentValue = i; }
	bool hasVariable() { return (flags & 0x16); }
	void setVariable(caosVar *i) { flags = flags | 0x16; variableValue = i; }
	virtual void notifyChanged() { }

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
	caosVar var;
	struct cmdinfo *cmd, *func;
	bool isvar;
	comparisonType comparison;

	std::string dump();

	token() { isvar = true; cmd = 0; func = 0; comparison = NONE; }
};

struct script {
	std::vector<std::list<token> > lines;
	std::vector<std::string> rawlines;
};

class caosScript {
public:
	script installer, removal;
	std::vector<script> scripts;

	caosScript(std::istream &);

	std::string dump();
};
