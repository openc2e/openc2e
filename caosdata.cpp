/*
 *  caosdata.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Jun 01 2004.
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

#include "caosVM_cmdinfo.h"

void setupCommandPointers() {
	cmds = new cmdinfo[247];
	CMDDEF(SETV, 2)
	CMDDEF(MAPK, 0)
	CMDDEF(BRMI, 2)
	CMDDEF(MAPD, 2)
	CMDDEF(META, 4)
	CMDDEF(RTAR, 3)
	CMDDEF(CMRT, 1)
	CMDDEF(DOIF, 0)
	cmds[phash_cmd(*(int *)"DOIF")].needscondition = true;
	CMDDEF(ELIF, 0)
	cmds[phash_cmd(*(int *)"ELIF")].needscondition = true;
	CMDDEF(ELSE, 0)
	CMDDEF(ENDI, 0)
	CMDDEF(REAF, 0)
	CMDDEF(MODV, 2)
	CMDDEF(SUBV, 2)
	CMDDEF(MULV, 2)
	CMDDEF(NEGV, 1)
	CMDDEF(POSE, 1)
	CMDDEF(PART, 1)
	CMDDEF(INST, 0)
	CMDDEF(SLOW, 0)
	CMDDEF(LOCK, 0)
	CMDDEF(UNLK, 0)
	CMDDEF(WAIT, 0)
	CMDDEF(REPS, 1)
	CMDDEF(REPE, 0)
	CMDDEF(ATTR, 1)
	CMDDEF(TICK, 1)
	CMDDEF(ELAS, 1)
	CMDDEF(BHVR, 1)
	CMDDEF(MVTO, 2)
	CMDDEF(TARG, 1)
	CMDDEF(KILL, 1)
	CMDDEF(SCRX, 4)
	CMDDEF(ADDV, 2)
	CMDDEF(NEXT, 0)
	CMDDEF(RTYP, 2)
	CMDDEF(DOOR, 3)
	CMDDEF(RATE, 5)
	CMDDEF(OUTS, 1)
	CMDDEF(OUTV, 1)
	CMDDEF(MMSC, 3)
	CMDDEF(RMSC, 3)
	CMDDEF(ANIM, 1)
	CMDDEF(SETA, 2)

	NULLDEF(SCRP)
	NULLDEF(ENDM)
	NULLDEF(RSCR)

	DBLCMDDEF("DBG:")
	DBLCMDDEF("NEW:")
	DBLCMDDEF("STIM")

	funcs = new cmdinfo[512];
	FUNCDEF(VAxx, 0)
	FUNCDEF(OVxx, 0)
	FUNCDEF(ADDM, 5)
	FUNCDEF(GAME, 1)
	FUNCDEF(TARG, 0)
	FUNCDEF(NULL, 0)
	FUNCDEF(RAND, 2)
	FUNCDEF(FROM, 0)
	FUNCDEF(POSE, 0)
	FUNCDEF(ADDR, 7)
	FUNCDEF(MMSC, 2)
	FUNCDEF(RMSC, 2)
	FUNCDEF(PACE, 0)
	FUNCDEF(ATTR, 0)

	cmdinfo n1(std::string("DBG: OUTS"), 1, false, &caosVM::c_DBG_OUTS);
	doublecmds.push_back(n1);
	cmdinfo n3(std::string("DBG: OUTV"), 1, false, &caosVM::c_DBG_OUTV);
	doublecmds.push_back(n3);
	cmdinfo n2(std::string("NEW: SIMP"), 7, false, &caosVM::c_NEW_SIMP);
	doublecmds.push_back(n2);
	cmdinfo n4(std::string("NEW: COMP"), 7, false, &caosVM::c_NEW_COMP);
	doublecmds.push_back(n4);
	cmdinfo n5(std::string("STIM WRIT"), 3, false, &caosVM::c_STIM_WRIT);
	doublecmds.push_back(n5);
}
