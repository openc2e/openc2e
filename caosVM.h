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

#include "caosScript.h"

#define CAOSDEBUG
//#define CAOSDEBUGDETAIL

class caosVM {
protected:
	caosVar var[100];
	caosVar _p_[2];
	
	std::vector<caosVar> params;
	caosVar result;
	bool truth;
	
	Agent *targ, *owner, *_it_;

	unsigned int currentline;
	std::vector<bool> truthstack;
	std::vector<unsigned int> linestack;
	std::vector<int> repstack;

	script *currentscript;

	signed char varnumber; // VAxx/OVxx hack

public:
	// map
	void v_ADDM();
	void c_BRMI();
	void c_MAPD();
	void c_MAPK();
	void v_ADDR();
	void c_RTYP();
	void c_DOOR();
	void c_RATE();
	void v_ROOM();
	
	// camera
	void c_CMRT();
	void c_META();
	
	// core
	void v_GAME();
	void c_OUTS();
	void c_OUTV();
	void c_SCRP(); // dummy
	void c_RSCR(); // dummy
	void c_ENDM();

	// variables
	void c_SETV();
	void v_RAND();
	void c_REAF();
	void v_VAxx();
	void v_OVxx();
	void c_MODV();
	void c_SUBV();
	void c_NEGV();
	void c_MULV();
	void c_ADDV();
	void c_SETA();
	void c_DIVV();
	void c_SETS();
	void v_UFOS();
	void v_MODU();
	void v_GNAM();
	void c_ABSV();
	void v_ACOS();
	void v_ASIN();
	void v_ATAN();
	void v_COS_();
	void v_SIN_();
	void v_TAN_();

	// flow
	void c_DOIF();
	void c_ENDI();
	void c_REPS();
	void c_REPE();
	void c_ELSE();
	void c_ELIF();
	void c_LOOP();
	void c_EVER();
	void c_UNTL();
	void c_GSUB();
	void c_SUBR();
	void c_RETN();
	
	// debug
	void c_TEST_PASS();
	void c_TEST_FAIL();
	void c_TEST_INIT();
	void c_TEST_CHEK();
	void c_TEST_STRT();
	void c_TEST_FINI();
	void c_DBG_OUTS();
	void c_DBG_OUTV();

	// agent
	void c_NEW_COMP();
	void c_NEW_SIMP();
	void c_NEW_VHCL();
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
	void c_ANIM();
	void v_ATTR();
	void v_ABBA();
	void c_BASE();
	void v_BASE();
	void v_BHVR();
	void v_CARR();
	void v_FMLY();
	void v_GNUS();
	void v_SPCS();
	void v_PLNE();
	void v_PNTR();
	void v_OWNR();
	void c_MESG_WRIT();
	void c_MESG_WRT();
	void v_TOTL();
	void c_SHOW();

	// motion
	void c_ELAS();
	void c_MVTO();

	// scripts
	void c_INST();
	void c_SLOW();
	void c_LOCK();
	void c_UNLK();
	void c_WAIT();
	void c_STOP();

	// compound
	void c_PART();
	void c_PAT_DULL();
	void c_PAT_BUTT();
	void c_PAT_KILL();

	// creatures
	void c_STIM_WRIT();

	// sounds
	void c_SNDE();
	void c_MMSC();
	void v_MMSC();
	void c_RMSC();
	void v_RMSC();

	// time
	void v_PACE();

	// resources
	void v_PRAY_AGTI();
	void v_PRAY_AGTS();
	void v_PRAY_BACK();
	void v_PRAY_COUN();
	void v_PRAY_DEPS();
	void v_PRAY_EXPO();
	void v_PRAY_FILE();
	void v_PRAY_FORE();
	void c_PRAY_GARB();
	void v_PRAY_IMPO();
	void v_PRAY_INJT();
	void v_PRAY_KILL();
	void v_PRAY_MAKE();
	void v_PRAY_NEXT();
	void v_PRAY_PREV();
	void c_PRAY_REFR();
	void v_PRAY_TEST();
	
	// input
	void c_CLAC();

	void runCurrentLine();
	void jumpToNextIfBlock();
	caosVar internalRun(std::list<token> &tokens, bool first);
	void runEntirely(script &s);

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

extern signed char varnumber;
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
