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

#include <string>
#include <vector>
#include <list>
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

class caosScript {
public:
	std::vector<std::list<token> > lines;
	std::vector<std::string> rawlines;
	
	caosScript(std::istream &);

	std::string dump();
};

class caosVM {
protected:
	caosVar var[100];
	caosVar _p_[2];
	caosVar targ;
	
	std::vector<caosVar> params;
	caosVar result;
	bool truth; // for comparisons
	
	Agent *owner, *_it_;

	int currentline;
	//	std::stack<int> stack;
		
public:
	caosScript *script; // todo: THIS SHOULD BE PRIVATE!

	// map
	void v_ADDM();
	void c_BRMI();
	void c_MAPD();
	void c_MAPK();
	void v_ADDR();
	void c_RTYP();
	void c_DOOR();
	void c_RATE();
	
	// camera
	void c_CMRT();
	void c_META();
	
	// core
	void v_GAME();
	void c_SETV();
	void null();
	void c_OUTS();
	void c_OUTV();

	// variables
	void v_RAND();
	void c_REAF();
	void v_VAxx();
	void v_OVxx();
	void c_MODV();
	void c_SUBV();
	void c_NEGV();
	void c_MULV();
	void c_ADDV();

	// flow
	void c_DOIF();
	void c_ENDI();
	void c_REPS();
	void c_REPE();
	void c_ELSE();
	
	// debug (currently in core)
	void c_DBG_OUTS();
	void c_DBG_OUTV();

	// agent
	void c_NEW_COMP();
	void c_NEW_SIMP();
	void v_NULL();
	void c_POSE();
	void c_RTAR();
	void v_TARG();
	void c_ATTR();
	void c_TICK();
	void c_BHVR();
	void c_TARG();
	void v_FROM();
	void v_POSE();
	void c_KILL();
	void c_NEXT();
	void c_SCRX();

	// motion
	void c_ELAS();
	void c_MVTO();

	// scripts
	void c_INST();
	void c_SLOW();
	void c_LOCK();
	void c_UNLK();
	void c_WAIT();

	// compound
	void c_PART();

	// creatures
	void c_STIM_WRIT();

	// sounds
	void c_MMSC();
	void v_MMSC();
	void c_RMSC();
	void v_RMSC();

	void runCurrentLine();
	void pushLineOntoStack(int);
	int popStackLine();
	caosVar internalRun(std::list<token> &tokens, bool first);
	void runEntirely();

	caosVM(Agent *o);

	friend void setupCommandPointers();
};

typedef void (caosVM::*caosVMmethod)();

struct cmdinfo {
	std::string name;
	unsigned int notokens;
	bool twotokens;
	bool needscondition;
	/*
	 * 0 = two-part command
	 * otherwise, pointer to the method
	 */
	caosVMmethod method;
	
	std::string dump();
	cmdinfo() { method = 0; twotokens = needscondition = false; }
	cmdinfo(std::string n, unsigned int o, bool t, caosVMmethod m) :
					name(n), notokens(o), twotokens(t), method(m) { needscondition = false; }
};

cmdinfo *getCmdInfo(std::string cmd, bool command);
cmdinfo *getSecondCmd(cmdinfo *first, std::string src, bool command);

class notEnoughParamsException { };
class badParamException { };
class tokeniseFailure { };

#define VM_VERIFY_SIZE(n) if (params.size() != n) { throw notEnoughParamsException(); }
#define VM_PARAM_STRING(name) std::string name; { caosVar __x = params.back(); \
	if (!__x.hasString()) { throw badParamException(); } \
	name = __x.stringValue; } params.pop_back();
#define VM_PARAM_INTEGER(name) int name; { caosVar __x = params.back(); \
	if (!__x.hasInt()) { throw badParamException(); } \
	else name = __x.intValue; } params.pop_back();
#define VM_PARAM_FLOAT(name) float name; { caosVar __x = params.back(); \
	if (!__x.hasFloat()) { throw badParamException(); } \
	name = __x.floatValue; } params.pop_back();
#define VM_PARAM_AGENT(name) Agent *name; { caosVar __x = params.back(); \
	if (!__x.hasAgent()) { throw badParamException(); } \
	name = __x.agentValue; } params.pop_back();
#define VM_PARAM_VARIABLE(name) caosVar *name; { caosVar __x = params.back(); \
	if (!__x.hasVariable()) { throw badParamException(); } \
	name = __x.variableValue; } params.pop_back();
#define VM_PARAM_DECIMAL(name) caosVar name = params.back(); \
	if ((!name.hasFloat()) && (!name.hasInt())) { throw badParamException(); } \
	params.pop_back();
