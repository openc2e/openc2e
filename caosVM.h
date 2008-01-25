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

#include "openc2e.h"
#include "caosScript.h"
#include <map>
#include <istream>
#include <ostream>
#include "AgentRef.h"

#include <boost/weak_ptr.hpp>
using boost::weak_ptr;

//#define CAOSDEBUG
//#define CAOSDEBUGDETAIL

// caosVM_agent.cpp:
unsigned int calculateScriptId(unsigned int message_id);

#define LVAL 1
#define RVAL 2
#define BYTESTR 4
					
class badParamException : public caosException {
	public:
		badParamException() : caosException("parameter type mismatch") {}
};

class vmStackItem {
	protected:
		struct visit_dump : public boost::static_visitor<std::string> {
			std::string operator()(const caosVar &i) const {
				return i.dump();
			}

			std::string operator()(caosVar *i) const {
				return std::string("ptr ") + i->dump();
			}

			std::string operator()(const bytestring_t &bs) const {
				std::ostringstream oss;
				oss << "[ ";
				for (bytestring_t::const_iterator i = bs.begin(); i != bs.end(); i++) {
					oss << (int)*i << " ";
				}
				oss << "]";
				return oss.str();
			}
		};

		struct visit_lval : public boost::static_visitor<const caosVar &> {
			
			const caosVar &operator()(const caosVar &i) const {
				return i;
			}

			const caosVar &operator()(caosVar *i) const {
				return *i;
			}

			const caosVar &operator()(const bytestring_t &) const {
				throw badParamException();
			}
				
		};

		struct visit_bs : public boost::static_visitor<bytestring_t> {
			bytestring_t operator()(const bytestring_t &i) const {
				return i;
			}
			bytestring_t operator()(caosVar *i) const {
				throw badParamException();
			}
			bytestring_t operator()(const caosVar &i) const {
				throw badParamException();
			}
		};
				
		boost::variant<caosVar, bytestring_t> value;

	public:

		vmStackItem(const caosVar &v) {
			value = v;
		}

		vmStackItem(bytestring_t bs) {
			value = bs;
		}

		vmStackItem(const vmStackItem &orig) {
			value = orig.value;
		}

		const caosVar &getRVal() const {
			try {
				return boost::apply_visitor(visit_lval(), value);
			} catch (boost::bad_visit &e) {
				throw badParamException();
			}
		}

		bytestring_t getByteStr() const {
			try {
				return boost::apply_visitor(visit_bs(), value);
			} catch (boost::bad_visit &e) {
				throw badParamException();
			}
		}

		std::string dump() const {
			try {
				return boost::apply_visitor(visit_dump(), value);
			} catch (boost::bad_visit &e) {
				return std::string("ERR::bad_visit");
			}
		}
};

struct callStackItem {
	std::vector<vmStackItem> valueStack;
	int nip;
};

typedef class caosVM *caosVM_p;

class blockCond {
	// XXX NOT SERIALIZABLE FIXME
	public:
		virtual bool operator()() = 0;
		virtual ~blockCond() {}
};

class caosVM {
public:	
	int trace;

	blockCond *blocking;

	void startBlocking(blockCond *whileWhat);
	bool isBlocking();
	
	// nb, ptr is immutable, class is mutable
	// This is so the stack manipulation macros work in the op classes as well
	const caosVM_p vm; // == this
	
	// script state...
	shared_ptr<script> currentscript;
	int nip, cip, runops;
	
	bool inst, lock, stop_loop;
	int timeslice;

	std::vector<vmStackItem> valueStack;
	std::vector<vmStackItem> auxStack;
	std::vector<callStackItem> callStack;
	
	std::istream *inputstream;
	std::ostream *outputstream;

	// ...which includes variables accessible to script
	caosVar var[100]; // might want to make this a map, for memory efficiency
	caosVar _p_[2]; // might want to add this onto the end of above map, if done
	AgentRef targ, owner, _it_;
	caosVar from;
	int part;
	weak_ptr<class Camera> camera;
	class Camera *getCamera();
	
	void resetScriptState(); // resets everything except OWNR

private:
	void resetCore();
public:

	caosVar result;
	
public:
	void setTarg(const AgentRef &a) { targ = a; }
	void setVariables(caosVar &one, caosVar &two) { _p_[0] = one; _p_[1] = two; }
	void setOwner(Agent *a) { owner = a; }
	void setOutputStream(std::ostream &o) { outputstream = &o; }

	class Creature *getTargCreature();
	class SpritePart *getCurrentSpritePart();

	void dummy_cmd();

	// map
	void v_ADDM();
	void c_ADDB();
	void c_BRMI();
	void c_MAPD();
	void c_MAPK();
	void v_ADDR();
	void c_RTYP();
	void v_RTYP();
	void v_RTYP_c2();
	void c_SETV_RTYP();
	void c_DOOR();
	void v_DOOR();
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
	void v_LINK();
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
	void v_HIRP();
	void v_LORP();
	void v_TORX();
	void v_TORY();
	void c_CACL();
	void v_WIND();
	void v_TEMP();
	void s_TEMP();
	void v_LITE();
	void s_LITE();
	void v_RADN();
	void s_RADN();
	void v_ONTR();
	void s_ONTR();
	void v_INTR();
	void s_INTR();
	void v_PRES();
	void s_PRES();
	void v_HSRC();
	void s_HSRC();
	void v_LSRC();
	void s_LSRC();
	void v_RSRC();
	void s_RSRC();
	void v_PSRC();
	void s_PSRC();
	void v_WNDX();
	void v_WNDY();
	void c_DOCA();
	void c_SETV_DOOR();
	void v_FLOR();
	void c_SYS_DMAP();
	void v_GNDW();
	void v_GRND();
	void c_ROOM();
	void v_ROOM_c1();
	
	// camera
	void v_VISI();
	void v_ONTV();
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
	void v_TRCK();
	void c_LINE();
	void v_SNAX();
	void c_SCAM();
	void c_ZOOM();
	void c_SNAP();
	void v_LOFT();
	void c_BKGD();
	void v_BKGD();
	void c_FRSH();
	void c_SYS_CMRP();
	void c_SYS_CMRA();
	void c_SYS_CAMT();
	void c_SYS_WTOP();

	// world
	void c_LOAD();
	void c_SAVE();
	void c_QUIT();
	void v_WNAM();
	void v_WUID();
	void c_WTNT();
	void v_NWLD();
	void c_WRLD();
	void v_WRLD();
	void c_PSWD();
	void v_PSWD();
	void v_WNTI();
	void c_DELW();

	// core
	void v_GAME();
	void s_GAME();
	void v_EAME();
	void s_EAME();
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
	void v_WOLF();
	void v_LANG();
	void v_TOKN();
	void v_GAME_c2();
	void s_GAME_c2();
	void c_VRSN();
	void v_VRSN();
	void v_OC2E_DDIR();
	void c_SYS_CMND();

	// variables
	void c_SETV();
	void v_RAND();
	void c_REAF();
	void v_VAxx();
	void v_OVxx();
	void v_MVxx();
	void s_VAxx();
	void s_OVxx();
	void s_MVxx();
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
	void s_P1();
	void v_P2();
	void s_P2();
	void c_ANDV();
	void c_ORRV();
	void c_ADDS();
	void v_VTOS();
	void v_AVAR();
	void s_AVAR();
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
	void c_CATO();
	void v_WILD();
	void v_NAME();
	void s_NAME();
	void v_MAME();
	void s_MAME();
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
	void c_POWV();
	void c_RNDV();
	void v_EGGL();
	void v_HATL();

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
	void c_DBG_ASRT();
	void c_DBG_OUTS();
	void c_DBG_OUTV();
	void c_DBUG();
	void c_DBG_MALLOC();
	void c_DBG_DUMP();
	void c_DBG_TRACE();
	void c_MANN();
	void c_DBG_DISA();
	void v_UNID();
	void v_UNID_c2();
	void v_AGNT();
	void v_DBG_IDNT();
	void c_DBG_PROF();
	void c_DBG_CPRO();
	void v_DBG_STOK();
	void c_DBG_TSLC();
	void v_DBG_TSLC();

	// agent
	void c_NEW_COMP();
	void c_NEW_COMP_c1();
	void c_NEW_SIMP();
	void c_NEW_SIMP_c2();
	void c_NEW_VHCL();
	void c_NEW_VHCL_c1();
	void c_NEW_BKBD();
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
	void v_FROM_ds();
	void s_FROM();
	void s_FROM_ds();
	void v_POSE();
	void c_KILL();
	void c_SCRX();
	void c_ANIM();
	void c_ANIM_c2();
	void c_ANMS();
	void v_ATTR();
	void s_ATTR();
	void v_ABBA();
	void c_BASE();
	void v_BASE();
	void v_BHVR();
	void v_CARR();
	void v_CARR_c1();
	void v_FMLY();
	void v_GNUS();
	void v_SPCS();
	void v_PNTR();
	void v_OWNR();
	void c_MESG_WRIT();
	void c_MESG_WRT();
	void v_TOTL();
	void c_SHOW();
	void v_SHOW();
	void v_POSX();
	void v_POSY();
	void c_FRAT();
	void c_OVER();
	void c_PUHL();
	void c_SETV_PUHL();
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
	void v_RNGE_c2();
	void s_RNGE();
	void s_RNGE_c2();
	void v_TRAN();
	void c_TRAN();
	void v_HGHT();
	void c_HAND();
	void v_HAND();
	void v_TOUC();
	void v_TICK();
	void c_PUPT();
	void c_SETV_PUPT();
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
	void v_NCLS();
	void v_PCLS();
	void v_TCOR();
	void c_CORE();
	void v_TWIN();
	void v_ACTV();
	void s_ACTV();
	void v_THRT();
	void s_THRT();
	void v_SIZE();
	void s_SIZE();
	void v_GRAV();
	void s_GRAV();
	void c_SETV_CLS2();
	void c_SETV_CLAS();
	void c_SLIM();
	void c_BHVR_c2();
	void v_LIML();
	void v_LIMT();
	void v_LIMR();
	void v_LIMB_c1();
	void v_OBJP();
	void s_OBJP();
	void v_XIST();
	void c_SCLE();
	void c_IMGE();
	void c_TNTW();
	void c_PRNT();
	void v_TCAR();
	void c_EDIT();
	void v_FRZN();
	void s_FRZN();
	
	// motion
	void c_ELAS();
	void v_ELAS();
	void c_MVTO();
	void v_VELX();
	void v_VELY();
	void s_VELX();
	void s_VELY();
	void v_OBST();
	void v_OBST_c2();
	void v_TMVB();
	void v_TMVT();
	void v_TMVF();
	void v_RELX();
	void v_RELY();
	void v_RELX_c2();
	void v_RELY_c2();
	void c_VELO();
	void c_ACCG();
	void v_ACCG();
	void s_ACCG();
	void v_ACCG_c2();
	void s_ACCG_c2();
	void c_AERO();
	void v_AERO();
	void v_AERO_c2();
	void s_AERO();
	void s_AERO_c2();
	void c_MVSF();
	void c_FRIC();
	void v_FRIC();
	void v_FALL();
	void v_MOVS();
	void s_MOVS();
	void c_MVBY();
	void c_FLTO();
	void c_FREL();
	void v_FLTX();
	void v_FLTY();
	void c_MCRT();
	void v_REST();
	void s_REST();
	
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
	void c_JECT();
	void v_SORQ();
	
	void c_RGAM();
	void v_MOWS();

	// compound
	void c_PART();
	void v_PART();
	void c_NEW_PART();
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
	void c_BBD_WORD();
	void c_BBD_SHOW();
	void c_BBD_EMIT();
	void c_BBD_EDIT();
	void c_BBD_VOCB();
	void c_NEW_BBTX();
	void c_BBTX();
	void c_SPOT();
	void c_KNOB();
	void c_KMSG();
	void c_BBLE();

	// creatures
	void c_STM_SHOU();
	void c_STM_SIGN();
	void c_STM_TACT();
	void c_STM_WRIT();
	void c_STIM_SHOU();
	void c_STIM_SIGN();
	void c_STIM_TACT();
	void c_STIM_WRIT();
	void c_STIM_FROM_c1();
	void c_STIM_SHOU_c2();
	void c_STIM_SIGN_c2();
	void c_STIM_TACT_c2();
	void c_STIM_WRIT_c2();
	void c_SWAY_SHOU();
	void c_SWAY_SIGN();
	void c_SWAY_TACT();
	void c_SWAY_WRIT();
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
	void s_NORN();
	void v_ZOMB();
	void v_DEAD();
	void c_URGE_SHOU();
	void c_URGE_SIGN();
	void c_URGE_TACT();
	void c_URGE_WRIT();
	void c_DRIV();
	void v_DRIV();
	void c_CHEM();
	void c_CHEM_c1();
	void v_CHEM();
	void v_CHEM_c1();
	void c_ASLP();
	void v_ASLP();
	void c_APPR();
	void c_UNCS();
	void v_UNCS();
	void v_FACE_STRING();
	void v_FACE_INT();
	void c_LIKE();
	void v_LIMB();
	void c_ORDR_SHOU();
	void c_ORDR_SIGN();
	void c_ORDR_TACT();
	void c_ORDR_WRIT();
	void c_DREA();
	void v_DREA();
	void c_BORN();
	void v_CAGE();
	void v_BYIT();
	void v_IT();
	void c_NEWC();
	void c_NEW_CREA();
	void c_NEW_CREA_c1();
	void c_NEW_CREA_c2();
	void c_LTCY();
	void c_MATE();
	void v_DRV();
	void v_IITT();
	void c_AGES();
	void c_LOCI();
	void v_LOCI();
	void v_TAGE();
	void v_ORGN();
	void v_ORGF();
	void v_ORGI();
	void c_SOUL();
	void v_SOUL();
	void v_DECN();
	void v_ATTN();
	void v_DIRN();
	void c_TOUC();
	void c_FORF();
	void c_WALK();
	void c_FACE();
	void c_DONE();
	void c_SAYN();
	void c_IMPT();
	void c_AIM();
	void v_BABY();
	void s_BABY();
	void c_SNEZ();
	void v_DRIV_c1();
	void c_DREA_c1();
	void c_FK();
	void v_BRED();
	void v_BVAR();
	void c_EXPR();
	void v_EXPR();
	void c_TNTC();
	void c_INJR();
	void c_SAY();
	void c_TRIG();
	void v_MONK();
	// (clothes)
	void c_BODY();
	void v_BODY();
	void c_DYED();
	void c_HAIR();
	void c_NUDE();
	void c_RSET();
	void c_STRE();
	void c_SWAP();
	void c_WEAR();
	void v_WEAR();
	void c_TNTO();
	// (attachment locations)
	void v_DFTX();
	void v_DFTY();
	void v_UFTX();
	void v_UFTY();
	void v_HEDX();
	void v_HEDY();
	void v_MTHX();
	void v_MTHY();
	
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
	void c_VOIS();
	void c_MIDI();
	void c_PLDS();

	// time
	void v_PACE();
	void c_BUZZ();
	void v_BUZZ();
	void v_HIST_DATE();
	void v_DATE();
	void v_HIST_SEAN();
	void v_SEAN();
	void v_HIST_TIME();
	void v_TIME();
	void v_HIST_YEAR();
	void v_YEAR();
	void v_MSEC();
	void c_PAUS();
	void v_PAUS();
	void c_WPAU();
	void v_WPAU();
	void v_RTIF();
	void v_RTIM();
	void v_WTIK();
	void v_RACE();
	void v_ETIK();
	void v_SEAV();
	void c_ASEA();
	void v_TMOD();
	void v_DAYT();
	void v_MONT();

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
	void v_NET_MAKE();
	
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
	void v_SCOL();
	void v_CLAC();
	void v_CLIK();
	void c_SCRL();
	void c_MOUS();

	// vehicles
	void c_CABN();
	void c_CABW();
	void c_SPAS();
	void c_GPAS();
	void c_GPAS_c2();
	void c_DPAS();
	void c_DPAS_c2();
	void c_CABP();
	void c_RPAS();
	void c_CABV();
	void v_CABV();
	void v_XVEC();
	void s_XVEC();
	void v_YVEC();
	void s_YVEC();
	void v_BUMP();
	void c_TELE();
	void c_DPS2();
	
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
	void v_NET_ERRA();	
	void v_NET_EXPO();
	void v_NET_FROM();
	void c_NET_HEAD();
	void c_NET_HEAR();
	void v_NET_HOST();
	void c_NET_LINE();
	void v_NET_LINE();
	void c_NET_PASS();
	void v_NET_PASS();
	void v_NET_RAWE();
	void c_NET_RUSO();
	void c_NET_STAT();
	void v_NET_ULIN();
	void c_NET_UNIK();
	void v_NET_USER();
	void v_NET_WHAT();
	void c_NET_WHOD();
	void c_NET_WHOF();
	void c_NET_WHON();
	void c_NET_WHOZ();
	void c_NET_WRIT();

	// genetics
	void c_GENE_CLON();
	void c_GENE_CROS();
	void c_GENE_KILL();
	void c_GENE_LOAD();
	void c_GENE_MOVE();
	void v_GTOS();
	void v_MTOA();
	void v_MTOC();
	void c_NEW_GENE();
	
	// history
	void v_HIST_CAGE();
	void v_HIST_COUN();
	void v_HIST_CROS();
	void c_HIST_EVNT();
	void v_HIST_FIND();
	void v_HIST_FINR();
	void v_HIST_FOTO();
	void c_HIST_FOTO();
	void v_HIST_GEND();
	void v_HIST_GNUS();
	void v_HIST_MON1();
	void v_HIST_MON2();
	void v_HIST_MUTE();
	void v_HIST_NAME();
	void c_HIST_NAME();
	void v_HIST_NETU();
	void v_HIST_NEXT();
	void v_HIST_PREV();
	void v_HIST_RTIM();
	void v_HIST_TAGE();
	void v_HIST_TYPE();
	void c_HIST_UTXT();
	void v_HIST_UTXT();
	void v_HIST_VARI();
	void c_HIST_WIPE();
	void v_HIST_WNAM();
	void v_HIST_WTIK();
	void v_HIST_WUID();
	void v_HIST_WVET();
	void v_OOWW();
	void c_EVNT();
	void c_RMEV();
	void c_DDE_NEGG();
	void c_DDE_DIED();
	void c_DDE_LIVE();
	
	// Vector ops
	void v_VEC_MAKE();
	void c_VEC_GETC();
	void v_VEC_ANGL();
	void c_VEC_SUBV();
	void c_VEC_ADDV();
	void c_VEC_MULV();
	void v_VEC_UNIT();
	void v_VEC_NULL();
	void v_VEC_MAGN();
	void c_VEC_SETV();


	// serialization test functions
	void c_SERS_MAPP();
	void c_SERL_MAPP();
	void c_SERS_SCRP();
	void c_SERL_SCRP();

	void safeJMP(int nip);
	void invoke_cmd(script *s, bool is_saver, int opidx);
	void runOpCore(script *s, caosOp op);
	void runOp();
	void runEntirely(shared_ptr<script> s);

	void tick();
	void stop();
	bool fireScript(shared_ptr<script> s, bool nointerrupt, Agent *frm = 0);

	caosVM(const AgentRef &o);
	~caosVM();

	bool stopped() { return !currentscript; }

	friend void setupCommandPointers();
};

typedef void (caosVM::*caosVMmethod)();

class notEnoughParamsException : public caosException {
	public:
		notEnoughParamsException() : caosException("Not enough parameters") {}
};

class invalidAgentException : public caosException {
	public:
		invalidAgentException() : caosException("Invalid agent handle") {}
		invalidAgentException(const char *s) : caosException(s) {}
		invalidAgentException(const std::string &s) : caosException(s) {}
};

#define VM_VERIFY_SIZE(n) // no-op, we assert in the pops. orig: if (params.size() != n) { throw notEnoughParamsException(); }
static inline void VM_STACK_CHECK(const caosVM *vm) {
	if (!vm->valueStack.size())
		throw notEnoughParamsException();
}

class caosVM__lval {
	protected:
		caosVM *owner;
	public:
		caosVar value;
		caosVM__lval(caosVM *vm) : owner(vm) {
			VM_STACK_CHECK(vm);
			value = owner->valueStack.back().getRVal();
			owner->valueStack.pop_back();
		}
		~caosVM__lval() {
			owner->valueStack.push_back(value);
		}
};

#define VM_PARAM_VALUE(name) caosVar name; { VM_STACK_CHECK(vm); \
	vmStackItem __x = vm->valueStack.back(); \
	name = __x.getRVal(); } vm->valueStack.pop_back();
#define VM_PARAM_STRING(name) std::string name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	name = __x.getRVal().getString(); } vm->valueStack.pop_back();
#define VM_PARAM_INTEGER(name) int name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	name = __x.getRVal().getInt(); } vm->valueStack.pop_back();
#define VM_PARAM_FLOAT(name) float name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	name = __x.getRVal().getFloat(); } vm->valueStack.pop_back();
#define VM_PARAM_VECTOR(name) Vector<float> name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	name = __x.getRVal().getVector(); } vm->valueStack.pop_back();
#define VM_PARAM_AGENT(name) boost::shared_ptr<Agent> name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	name = __x.getRVal().getAgent(); } vm->valueStack.pop_back();
// TODO: is usage of valid_agent correct here, or should we be caos_asserting?
#define VM_PARAM_VALIDAGENT(name) VM_PARAM_AGENT(name) valid_agent(name);
#define VM_PARAM_VARIABLE(name) caosVM__lval vm__lval_##name(this); caosVar * const name = &vm__lval_##name.value;
#define VM_PARAM_DECIMAL(name) caosVar name; { VM_STACK_CHECK(vm); vmStackItem __x = vm->valueStack.back(); \
	name = __x.getRVal(); } vm->valueStack.pop_back();
#define VM_PARAM_BYTESTR(name) bytestring_t name; { \
	VM_STACK_CHECK(vm); \
	vmStackItem __x = vm->valueStack.back(); \
	name = __x.getByteStr(); } vm->valueStack.pop_back();

#define CAOS_LVALUE(name, check, get, set) \
	void caosVM::v_##name() { \
		check; \
		valueStack.push_back((get)); \
	} \
	void caosVM::s_##name() { \
		check; \
		VM_PARAM_VALUE(newvalue) \
		set; \
	}

#define CAOS_LVALUE_WITH(name, agent, check, get, set) \
	CAOS_LVALUE(name, valid_agent(agent); check, get, set)

#define CAOS_LVALUE_TARG(name, check, get, set) \
	CAOS_LVALUE_WITH(name, targ, check, get, set)

#define CAOS_LVALUE_WITH_SIMPLE(name, agent, exp) \
	CAOS_LVALUE(name, valid_agent(agent), exp, exp = newvalue)

#define CAOS_LVALUE_SIMPLE(name, exp) \
	CAOS_LVALUE(name, (void)0, exp, exp = newvalue)

#define CAOS_LVALUE_TARG_SIMPLE(name, exp) \
	CAOS_LVALUE_TARG(name, (void)0, exp, exp = newvalue)
#define STUB throw caosException("stub in " __FILE__)

// FIXME: use do { ... } while (0)
#define valid_agent(x) do { if (!(x)) throw invalidAgentException(boost::str(boost::format("Invalid agent handle: %s thrown from %s:%d") % #x % __FILE__ % __LINE__)); } while(0)

#endif
/* vim: set noet: */
