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

/*
 using sometruth is BROKEN, we need to have a stack

 todo: if we encounter else in jumpToNextIfBlock, we can just skip to the line past it
*/

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
	std::cout << "couldn't find matching block for IF blocks, stopping script\n";
}

void caosVM::c_DOIF() {
	VM_VERIFY_SIZE(0)
	truthstack.push_back(false);
	if (!truth) jumpToNextIfBlock();
	else { truthstack.pop_back(); truthstack.push_back(true); }
}

void caosVM::c_ELIF() {
	VM_VERIFY_SIZE(0)
	if (!truth || truthstack.back()) jumpToNextIfBlock();
	else { truthstack.pop_back(); truthstack.push_back(true); }
}

void caosVM::c_ELSE() {
	VM_VERIFY_SIZE(0)
	assert(truthstack.back());
	jumpToNextIfBlock();
}

void caosVM::c_ENDI() {
	VM_VERIFY_SIZE(0)
	truthstack.pop_back();
}

void caosVM::c_REPS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(reps)
}

void caosVM::c_REPE() {
	VM_VERIFY_SIZE(0)
}
