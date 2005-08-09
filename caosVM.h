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

#ifndef _CAOSVM_H
#define _CAOSVM_H

#include "caosScript.h"
#include <map>
#include <ostream>
#include "AgentRef.h"

//#define CAOSDEBUG
//#define CAOSDEBUGDETAIL

// caosVM_agent.cpp:
unsigned int calculateScriptId(unsigned int message_id);

class caosVM {
protected:
	// script state...
	script *currentscript;
	unsigned int currentline;
	bool blocking;
	std::vector<bool> truthstack;
	std::vector<unsigned int> linestack;
	std::vector<unsigned int> repstack;
	std::vector<std::vector<AgentRef> > enumstack;
	bool locked, noschedule;
	unsigned int blockingticks;
	std::ostream *outputstream;

	// ...which includes variables accessible to script
	caosVar var[100]; // might want to make this a map, for memory efficiency
	caosVar _p_[2]; // might want to add this onto the end of above map, if done
	AgentRef targ, owner, _it_;
	unsigned int part;
	
	void resetScriptState(); // resets everything except OWNR
	
	// variables to pass data to/from opcodes
	std::vector<caosVar> params;
	caosVar result;
	bool truth;
	signed char varnumber; // VAxx/OVxx hack

	void jumpToNextIfBlock();
	void jumpToEquivalentNext();

public:
	void setTarg(const AgentRef &a) { targ = a; part = 0; }
	void setVariables(caosVar &one, caosVar &two) { _p_[0] = one; _p_[1] = two; }
	void setOwner(Agent *a) { owner = a; }
	void setOutputStream(std::ostream &o) { outputstream = &o; }

	// map
	void v_ADDM();
	void c_BRMI();
	void c_MAPD();
	void c_MAPK();
	void v_ADDR();
	void c_RTYP();
	void v_RTYP();
	void c_DOOR();
	void c_RATE();
	void v_ROOM();
	void v_LEFT();
	void v_RGHT();
	void v_UP();
	void v_DOWN();
	void c_PROP();
	void v_PROP();
	void c_PERM();
	void v_PERM();
	void v_GRAP();
	void v_GMAP();
	void c_LINK();
	void v_GRID();
	void c_EMIT();
	void v_WALL();
	void c_ALTR();
	
	// camera
	void c_CMRT();
	void c_META();
	void c_CMRA();
	void c_CMRP();
	void v_CMRX();
	void v_CMRY();
	void v_WNDW();
	void v_WNDH();
	void v_WNDB();
	void v_WNDL();
	void v_WNDR();
	void v_WNDT();
	
	// core
	void v_GAME();
	void c_OUTS();
	void c_OUTV();
	void c_SCRP(); // dummy
	void c_RSCR(); // dummy
	void c_ISCR(); // dummy
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
	void v_SQRT();
	void v_P1();
	void v_P2();
	void c_ANDV();
	void c_ORRV();
	void c_ADDS();
	void v_VTOS();
	void v_AVAR();
	void v_CHAR();

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
	void c_ENUM();
	void c_ESEE();
	void c_ETCH();
	void c_EPAS();
	void c_NEXT();
	
	// debug
	void c_TEST_PASS();
	void c_TEST_FAIL();
	void c_TEST_INIT();
	void c_TEST_CHEK();
	void c_TEST_STRT();
	void c_TEST_FINI();
	void c_DBG_OUTS();
	void c_DBG_OUTV();
	void v_UNID();
	void v_AGNT();

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
	void v_PNTR();
	void v_OWNR();
	void c_MESG_WRIT();
	void c_MESG_WRT();
	void v_TOTL();
	void c_SHOW();
	void v_POSX();
	void v_POSY();
	void c_FRAT();
	void c_OVER();
	void c_PUHL();
	void v_VISI();
	void v_POSL();
	void v_POST();
	void v_POSB();
	void v_POSR();
	void v_PLNE();
	void c_PLNE();
	void v_WDTH();
	void c_TINT();
	void c_RNGE();
	void v_RNGE();
	void v_TRAN();
	void c_TRAN();
	void v_HGHT();
	void c_HAND();
	void v_HAND();
	void v_TOUC();
	void v_TICK();
	void c_PUPT();
	
	// motion
	void c_ELAS();
	void c_MVTO();
	void v_VELX();
	void v_VELY();
	void v_OBST();
	void v_TMVT();
	void v_TMVF();
	void v_RELX();
	void v_RELY();
	void c_VELO();
	void c_ACCG();
	void v_ACCG();
	void c_AERO();
	void v_AERO();
	void c_MVSF();
	void c_FRIC();
	void v_FRIC();
	void v_FALL();
	
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
	void c_FCUS();

	// creatures
	void c_STIM_SHOU();
	void c_STIM_SIGN();
	void c_STIM_TACT();
	void c_STIM_WRIT();
	void c_SWAY_SHOU();
	void c_ASLP();
	void c_ZOMB();
	void c_DIRN();
	void c_NOHH();
	void v_HHLD();
	void c_MVFT();
	void c_CHEM();
	void v_CREA();
	void c_VOCB();
	
	// sounds
	void c_SNDE();
	void c_SNDC();
	void c_MMSC();
	void v_MMSC();
	void c_RMSC();
	void v_RMSC();
	void c_SNDL();
	void c_FADE();

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
	void c_CLIK();

	// vehicles
	void c_CABN();
	void c_CABW();
	void c_SPAS();
	void c_GPAS();
	void c_DPAS();
	
	// ports
	void c_PRT_BANG();
	void v_PRT_FRMA();
	void v_PRT_FROM();
	void c_PRT_INEW();
	void v_PRT_ITOT();
	void c_PRT_IZAP();
	void c_PRT_JOIN();
	void c_PRT_KRAK();
	void v_PRT_NAME();
	void c_PRT_ONEW();
	void v_PRT_OTOT();
	void c_PRT_OZAP();
	void c_PRT_SEND();

	void runCurrentLine();
	caosVar internalRun(std::list<token> &tokens, bool first); // run a command, as represented by tokens
	void runEntirely(script &s);
	void tick();
	void stop();
	bool fireScript(script &s, bool nointerrupt);

	caosVM(const AgentRef &o);

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

#define VM_VERIFY_SIZE(n) if (params.size() != n) { throw notEnoughParamsException(); }
#define VM_PARAM_STRING(name) std::string name; { caosVar __x = params.back(); \
	if (!__x.hasString()) { throw badParamException(); } \
	name = __x.getString(); } params.pop_back();
#define VM_PARAM_INTEGER(name) int name; { caosVar __x = params.back(); \
	if (!__x.hasDecimal()) { throw badParamException(); } \
	else name = __x.getInt(); } params.pop_back();
#define VM_PARAM_FLOAT(name) float name; { caosVar __x = params.back(); \
	if (!__x.hasDecimal()) { throw badParamException(); } \
	name = __x.getFloat(); } params.pop_back();
#define VM_PARAM_AGENT(name) Agent *name; { caosVar __x = params.back(); \
	if (!__x.hasAgent()) { throw badParamException(); } \
	name = __x.getAgent(); } params.pop_back();
#define VM_PARAM_VARIABLE(name) caosVar *name; { caosVar __x = params.back(); \
	if (!__x.hasVariable()) { throw badParamException(); } \
	name = __x.getVariable(); } params.pop_back();
#define VM_PARAM_DECIMAL(name) caosVar name = params.back(); \
	if (!name.hasDecimal()) { throw badParamException(); } \
	params.pop_back();

#endif
