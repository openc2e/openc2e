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
#include <istream>
#include <ostream>
#include "AgentRef.h"

//#define CAOSDEBUG
//#define CAOSDEBUGDETAIL

// caosVM_agent.cpp:
unsigned int calculateScriptId(unsigned int message_id);

#define LVAL 1
#define RVAL 2
#define BYTESTR 4
					
struct vmStackItem {
	public:
		int type;
		caosVar i_val;
		caosVar *p_val;
		std::vector<unsigned int> bytestring;

		vmStackItem(const caosVar &v) {
			i_val = v;
			p_val = &i_val;
			type = LVAL;
		}

		vmStackItem(caosVar *p) {
			p_val = p;
			type = LVAL | RVAL;
		}

		vmStackItem(std::vector<unsigned int> bs) {
			bytestring = bs;
			type = BYTESTR;
		}

		vmStackItem(const vmStackItem &orig) {
			type = orig.type;
			i_val = orig.i_val;
			if (type & RVAL) {
				p_val = orig.p_val;
			}
			else p_val = &i_val;
			bytestring = orig.bytestring;
		}
};

struct callStackItem {
	std::vector<vmStackItem> valueStack;
	int nip;
};

typedef class caosVM *caosVM_p;

class blockCond {
	public:
		virtual bool operator()() = 0;
		virtual ~blockCond() {}
};

class caosVM {
public:	
	bool trace;

	blockCond *blocking;

	void startBlocking(blockCond *whileWhat);
	bool isBlocking();
	
	// nb, ptr is immutable, class is mutable
	// This is so the stack manipulation macros work in the op classes as well
	const caosVM_p vm; // == this
	
	// script state...
	shared_ptr<script> currentscript;
	int nip, cip;
	
	bool inst, lock, stop_loop;
	int timeslice;

	std::vector<vmStackItem> valueStack;
	std::vector<callStackItem> callStack;
	
	std::istream *inputstream;
	std::ostream *outputstream;

	// ...which includes variables accessible to script
	caosVar var[100]; // might want to make this a map, for memory efficiency
	caosVar _p_[2]; // might want to add this onto the end of above map, if done
	AgentRef targ, owner, _it_, from;
	unsigned int part;
	
	void resetScriptState(); // resets everything except OWNR

protected:
	inline void returnVariable(caosVar &cv) {
		valueStack.push_back(&cv);
	}
private:
	void resetCore();
public:

	caosVar result;
	
public:
	void setTarg(const AgentRef &a) { targ = a; part = 0; }
	void setVariables(caosVar &one, caosVar &two) { _p_[0] = one; _p_[1] = two; }
	void setOwner(Agent *a) { owner = a; }
	void setOutputStream(std::ostream &o) { outputstream = &o; }

	class Creature *getTargCreature();
	class SpritePart *getCurrentSpritePart();

	// map
	void v_ADDM();
	void c_ADDB();
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
	void v_MAPW();
	void v_MAPH();
	void v_BKDS();
	void v_RLOC();
	void c_DMAP();
	void v_ERID();
	void c_DELR();
	void c_DELM();
	void v_MLOC();
	
	// camera
	void v_VISI();
	void c_CMRT();
	void c_META();
	void v_META();
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
	void c_WDOW();
	void v_WDOW();
	void c_TRCK();
	void c_LINE();
	void v_SNAX();
	void c_SCAM();
	void c_ZOOM();

	// world
	void c_LOAD();
	void c_SAVE();
	void c_QUIT();
	void v_WNAM();

	// core
	void v_GAME();
	void v_EAME();
	void c_DELG();
	void c_OUTX();
	void c_OUTS();
	void c_OUTV();
	void c_SCRP(); // dummy
	void c_RSCR(); // dummy
	void c_ISCR(); // dummy
	void c_ENDM();
	void v_VMNR();
	void v_VMJR();

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
	void c_CHAR();
	void v_TYPE();
	void v_ITOF();
	void v_FTOI();
	void v_STRL();
	void v_READ();
	void v_CATI();
	void v_CATA();
	void v_CATX();
	void v_WILD();
	void v_NAME();
	void v_MAME();
	void v_SUBS();
	void v_STOI();
	void v_STOF();
	void v_LOWA();
	void v_UPPA();
	void v_SINS();
	void v_REAQ();
	void c_DELN();
	void v_REAN();
	void c_NAMN();
	void v_GAMN();

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
	void c_ECON();
	void c_NEXT();
	void c_CALL();
	void v_CAOS();
	
	// debug
	void c_TEST_PASS();
	void c_TEST_FAIL();
	void c_TEST_INIT();
	void c_TEST_CHEK();
	void c_TEST_STRT();
	void c_TEST_FINI();
	void c_DBG_OUTS();
	void c_DBG_OUTV();
	void c_DBG_MALLOC();
	void c_DBG_TRACE();
	void c_MANN();
	void c_DBG_DISA();
	void v_UNID();
	void v_AGNT();

	// agent
	void c_NEW_COMP();
	void c_NEW_SIMP();
	void c_NEW_VHCL();
	void v_NULL();
	void c_POSE();
	void c_RTAR();
	void c_TTAR();
	void c_STAR();
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
	void c_ANMS();
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
	void v_PUHL();
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
	void c_STPT();
	void c_DCOR();
	void c_MIRA();
	void v_MIRA();
	void v_DISQ();
	void c_ALPH();
	void v_HELD();
	void c_GALL();
	void v_GALL();
	void v_SEEE();
	void v_TINT();
	void c_TINO();
	void c_DROP();
	
	// motion
	void c_ELAS();
	void v_ELAS();
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
	void v_MOVS();
	void c_MVBY();
	void c_FLTO();
	void c_FREL();
	void v_FLTX();
	void v_FLTY();
	
	// scripts
	void c_INST();
	void c_SLOW();
	void c_LOCK();
	void c_UNLK();
	void c_WAIT();
	void c_STOP();
	void v_CODE();
	void v_CODF();
	void v_CODG();
	void v_CODS();

	void c_RGAM();
	void v_MOWS();

	// compound
	void c_PART();
	void v_PART();
	void c_PAT_DULL();
	void c_PAT_BUTT();
	void c_PAT_FIXD();
	void c_PAT_TEXT();
	void c_PAT_CMRA();
	void c_PAT_KILL();
	void c_PAT_MOVE();
	void c_PAT_GRPH();
	void c_FCUS();
	void c_FRMT();
	void c_PTXT();
	void v_PTXT();
	void v_PNXT();
	void c_PAGE();
	void v_PAGE();
	void v_NPGS();
	void c_GRPV();
	void c_GRPL();

	// creatures
	void c_STIM_SHOU();
	void c_STIM_SIGN();
	void c_STIM_TACT();
	void c_STIM_WRIT();
	void c_SWAY_SHOU();
	void c_ZOMB();
	void c_DIRN();
	void c_NOHH();
	void v_HHLD();
	void c_MVFT();
	void v_CREA();
	void c_VOCB();
	void c_DEAD();
	void c_NORN();
	void v_NORN();
	void v_ZOMB();
	void v_DEAD();
	void c_URGE_SIGN();
	void c_URGE_WRIT();
	void c_DRIV();
	void v_DRIV();
	void c_CHEM();
	void v_CHEM();
	void c_ASLP();
	void v_ASLP();
	void c_APPR();
	void c_UNCS();
	void v_UNCS();
	void v_FACE();
	void c_LIKE();
	void v_LIMB();
	void c_ORDR_SHOU();
	void c_DREA();
	void v_DREA();
	void c_BORN();
	void v_CAGE();
	void v_BYIT();
	void v_IT();
	void v_DFTX();
	void v_DFTY();
	void c_NEWC();
	void c_NEW_CREA();
	void c_LTCY();
	void c_MATE();
	void v_DRV();
	void v_IITT();
	void c_AGES();
	void c_LOCI();
	void v_LOCI();
	void v_TAGE();
	
	// sounds
	void c_SNDE();
	void c_SNDC();
	void c_MMSC();
	void v_MMSC();
	void c_RMSC();
	void v_RMSC();
	void c_SNDL();
	void c_FADE();
	void c_STPC();
	void c_STRK();
	void c_VOLM();
	void v_VOLM();
	void v_MUTE();
	void c_SEZZ();

	// time
	void v_PACE();
	void c_BUZZ();
	void v_BUZZ();
	void v_DATE();
	void v_SEAN();
	void v_TIME();
	void v_YEAR();
	void v_MSEC();
	void c_PAUS();
	void v_PAUS();
	void c_WPAU();
	void v_WPAU();
	void v_RTIF();

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
	void c_IMSK();
	void v_IMSK();
	void v_KEYD();
	void v_HOTS();
	void v_HOTP();
	void c_PURE();
	void v_PURE();
	void v_MOPX();
	void v_MOPY();

	// vehicles
	void c_CABN();
	void c_CABW();
	void c_SPAS();
	void c_GPAS();
	void c_DPAS();
	void c_CABP();
	void c_RPAS();
	void v_RPAS();
	void c_CABV();
	void v_CABV();
	
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

	// files
	void c_FILE_GLOB();
	void c_FILE_ICLO();
	void c_FILE_IOPE();
	void c_FILE_JDEL();
	void c_FILE_OCLO();
	void c_FILE_OFLU();
	void c_FILE_OOPE();
	void v_FVWM();
	void v_INNF();
	void v_INNI();
	void v_INNL();
	void v_INOK();
	void c_WEBB();

	// net
	void c_NET_PASS();
	void v_NET_PASS();
	void c_NET_LINE();
	void v_NET_LINE();
	void v_NET_ERRA();

	// genetics
	void c_GENE_CLON();
	void c_GENE_CROS();
	void c_GENE_KILL();
	void c_GENE_LOAD();
	void c_GENE_MOVE();
	void v_GTOS();
	void v_MTOA();
	void v_MTOC();
	
	// history
	void v_HIST_NAME();
	void c_HIST_EVNT();
	void v_OOWW();

	void runOp();
	void runEntirely(shared_ptr<script> s);

	void tick();
	void stop();
	bool fireScript(shared_ptr<script> s, bool nointerrupt, Agent *frm = 0);

	caosVM(const AgentRef &o);

	bool stopped() { return !currentscript; }

	friend void setupCommandPointers();
};

typedef void (caosVM::*caosVMmethod)();

class notEnoughParamsException : caosException {
	public:
		notEnoughParamsException() : caosException("Not enough parameters") {}
};
class badParamException : public caosException {
	public:
		badParamException() : caosException("parameter type mismatch") {}
};

#define VM_VERIFY_SIZE(n) // no-op, we assert in the pops. orig: if (params.size() != n) { throw notEnoughParamsException(); }
static inline void VM_STACK_CHECK(const caosVM *vm) {
	if (!vm->valueStack.size())
		throw notEnoughParamsException();
}
#define VM_PARAM_VALUE(name) caosVar name; { VM_STACK_CHECK(vm); \
	vmStackItem __x = vm->valueStack.back(); \
	if (!(__x.type & LVAL)) { throw badParamException(); } \
	name = *__x.p_val; } vm->valueStack.pop_back();
#define VM_PARAM_STRING(name) std::string name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	if (!(__x.type & LVAL)) { throw badParamException(); } \
	if (!__x.p_val->hasString()) { throw badParamException(); } \
	name = __x.p_val->getString(); } vm->valueStack.pop_back();
#define VM_PARAM_INTEGER(name) int name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	if (!(__x.type & LVAL)) { throw badParamException(); } \
	if (!__x.p_val->hasNumber()) { throw badParamException(); } \
	name = __x.p_val->getInt(); } vm->valueStack.pop_back();
#define VM_PARAM_FLOAT(name) float name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	if (!(__x.type & LVAL)) { throw badParamException(); } \
	if (!__x.p_val->hasNumber()) { throw badParamException(); } \
	name = __x.p_val->getFloat(); } vm->valueStack.pop_back();
#define VM_PARAM_AGENT(name) Agent *name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	if (!(__x.type & LVAL)) { throw badParamException(); } \
	if (!__x.p_val->hasAgent()) { throw badParamException(); } \
	name = __x.p_val->getAgent(); } vm->valueStack.pop_back();
#define VM_PARAM_VALIDAGENT(name) VM_PARAM_AGENT(name) caos_assert(name);
#define VM_PARAM_VARIABLE(name) caosVar *name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	if (!(__x.type & RVAL)) { throw badParamException(); } \
	name = __x.p_val; } vm->valueStack.pop_back();
#define VM_PARAM_DECIMAL(name) caosVar name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	if (!(__x.type & LVAL)) { throw badParamException(); } \
	if (!__x.p_val->hasDecimal()) { throw badParamException(); } \
	name = *__x.p_val; } vm->valueStack.pop_back();
#define VM_PARAM_BYTESTR(name) std::vector<unsigned int> name; { \
	VM_STACK_CHECK(vm); \
	vmStackItem __x = vm->valueStack.back(); \
	if (!(__x.type & BYTESTR)) { throw badParamException(); } \
	name = __x.bytestring; } vm->valueStack.pop_back();

#define STUB throw caosException("stub in " __FILE__)

#endif
/* vim: set noet: */
