/*
 *  caosVM_flow.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun May 30 2004.
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

#include "caosVM.h"
#include <iostream>
#include "openc2e.h"

void caosVM::jumpToNextIfBlock() {
	cmdinfo *doif = getCmdInfo("DOIF", true); assert(doif != 0);
	cmdinfo *elif = getCmdInfo("ELIF", true); assert(elif != 0);
	cmdinfo *els = getCmdInfo("ELSE", true); assert(els != 0);
	cmdinfo *endi = getCmdInfo("ENDI", true); assert(endi != 0);
	int stack = 0;
	for (unsigned int i = currentline + 1; i < currentscript->lines.size(); i++) {
		token front = currentscript->lines[i].front();
		if (front.cmd == doif) {
			stack++;
		} else if (front.cmd == endi) {
			if (stack) { stack--; continue; }
			currentline = i; return;
		} else if (stack) {
			continue;
		} else if ((front.cmd == elif) && !truthstack.back()) {
			currentline = i; return;
		} else if ((front.cmd == els) && !truthstack.back()) {
			currentline = i + 1; return; // we don't NEED to actually run 'else'
		}
	}
	currentline = currentscript->lines.size();
	std::cerr << "couldn't find matching block for IF blocks, stopping script\n";
}

/**
 DOIF (command) condition (condition)
 
 Part of a DOIF/ELIF/ELSE/ENDI block. Jump to the next part of the block if condition is false, otherwise continue executing.
*/
void caosVM::c_DOIF() {
	VM_VERIFY_SIZE(0)
	truthstack.push_back(false);
	if (!truth) jumpToNextIfBlock();
	else { truthstack.pop_back(); truthstack.push_back(true); }
}

/**
 ELIF (command) condition (condition)
 
 Part of a DOIF/ELIF/ELSE/ENDI block. If none of the previous DOIF/ELIF conditions have been true, and condition evaluates to true, then the code in the ELIF block is executed.
*/
void caosVM::c_ELIF() {
	VM_VERIFY_SIZE(0)
	assert(!truthstack.empty());
	if (!truth || truthstack.back()) jumpToNextIfBlock();
	else { truthstack.pop_back(); truthstack.push_back(true); }
}

/**
 ELSE (command)
 
 Part of a DOIF/ELIF/ELSE/ENDI block. If ELSE is present, it is jumped to when none of the previous DOIF/ELIF conditions are true.
*/
void caosVM::c_ELSE() {
	VM_VERIFY_SIZE(0)
	assert(!truthstack.empty());
	assert(truthstack.back());
	jumpToNextIfBlock();
}

/**
 ENDI (command)
 
 The end of a DOIF/ELIF/ELSE/ENDI block.
*/
void caosVM::c_ENDI() {
	VM_VERIFY_SIZE(0)
	assert(!truthstack.empty());
	truthstack.pop_back();
}

/**
 REPS (command) reps (integer)
*/
void caosVM::c_REPS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(reps)
	assert(reps > 0)
	repstack.push_back(reps);
	linestack.push_back(currentline + 1);
}

/**
 REPE (command)
*/
void caosVM::c_REPE() {
	VM_VERIFY_SIZE(0)
	assert(!linestack.empty());
	assert(!repstack.empty());
	int i = repstack.back() - 1;
	repstack.pop_back();
	if (i) {
		repstack.push_back(i);
		currentline = linestack.back();
	} else linestack.pop_back();
}

/**
 LOOP (command)
 
 The start of a LOOP..EVER or LOOP..UNTL loop.
*/
void caosVM::c_LOOP() {
	VM_VERIFY_SIZE(0)
	linestack.push_back(currentline + 1);
}

/**
 EVER (command)
 
 Jump back to the matching LOOP, no matter what.
*/
void caosVM::c_EVER() {
	VM_VERIFY_SIZE(0)
	assert(!linestack.empty());
	currentline = linestack.back();
}

/**
 UNTL (command) condition (condition)
 
 Jump back to the matching LOOP unless the condition evaluates to true.
*/
void caosVM::c_UNTL() {
	VM_VERIFY_SIZE(0)
	assert(!linestack.empty());
	if (!truth) currentline = linestack.back();
	else linestack.pop_back();
}

/**
 GSUB (command) label (label)
 
 Jump to a subroutine defined by SUBR with label 'label'.
*/
void caosVM::c_GSUB() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(label)
	assert(label.size());
	cmdinfo *subr = getCmdInfo("SUBR", true); assert(subr != 0);
	for (unsigned int i = currentline + 1; i < currentscript->lines.size(); i++) {
		std::list<token>::iterator j = currentscript->lines[i].begin();
		if (((*j).cmd == subr) && ((*++j).var.stringValue == label)) {
			linestack.push_back(currentline + 1);
			currentline = i + 1;
			return;
		}
	}
	std::cerr << "warning: GSUB didn't find matching SUBR for " << label << ", ignoring\n";
}

/**
 SUBR (command) label (label)
 
 Define the start of a subroute to be called with GSUB, with label 'label'.
 If the command is encountered during execution, it acts like a STOP.
*/
void caosVM::c_SUBR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(label)
	c_STOP();
}

/**
 RETN (command)
 
 Return from a subroutine called with GSUB.
*/
void caosVM::c_RETN() {
	VM_VERIFY_SIZE(0)
	assert(!linestack.empty());
	currentline = linestack.back();
	linestack.pop_back();
}
