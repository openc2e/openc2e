/*
 *  caosVM.cpp
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

#include "caosVM.h"
#include "openc2e.h"
#include "World.h"
#include "bytecode.h"
#include <iostream>

using std::cout;
using std::cerr;
using std::endl;

class caosAbort : public caosOp {
	public:
		void execute(caosVM *vm) {
			cerr << "Bad! Some opcode forgot to set its successor" << endl;
			caos_assert(false);
		}
} abortop;

caosVM::caosVM(const AgentRef &o)
	: vm(this)
{
	owner = o;
	currentscript.reset();
	cip = nip = 0;
	blocking = NULL;
	inputstream = 0; outputstream = 0;
	resetCore();
	trace = false;
}

bool caosVM::isBlocking() {
	if (!blocking) return false;
	bool bl = (*blocking)();
	if (!bl) {
		delete blocking;
		blocking = NULL;
	}
	return bl;
}

void caosVM::startBlocking(blockCond *whileWhat) {
	if (!owner)
		// TODO: should this just fail to block?
		throw creaturesException("trying to block in a non-blockable script");
	inst = false;
	if (blocking)
		throw creaturesException("trying to block with a block condition in-place");
	blocking = whileWhat;
}

inline void caosVM::runOp() {
	cip = nip;
	nip++;
	shared_ptr<script> scr = currentscript;
	caosOp *op = currentscript->getOp(cip);
	if (!op) {
		stop();
		return;
	}
	result.reset(); // xxx this belongs in opcode maybe
	try {
		if (trace) {
			fprintf(stderr, "Trace: %08d ", cip);
			std::cerr << op->dump() << std::endl;
		}
		op->execute(this);
	} catch (creaturesException &e) {
		//std::cerr << "script stopped due to exception " << e.what() << endl;
		stop();
		throw;
	} catch (caosException &e) {
		e.trace(scr->filename.c_str(), op->getlineno(), scr, op);
		//std::cerr << "script stopped due to exception " << e.what() << endl;
		stop();
		throw;
	}
	if (!result.isNull())
		valueStack.push_back(result);
}

void caosVM::stop() {
	cip = nip = 0;
	lock = false;
	currentscript.reset();
}

void caosVM::runEntirely(shared_ptr<script> s) {
	currentscript = s;
	cip = nip = 0;
	while (true) {
		runOp();
		if (!currentscript) break;
		if (blocking) {
			delete blocking;
			blocking = NULL;
			throw creaturesException("blocking in an installation script");
		}
	}
	stop(); // probably redundant, but eh
}

bool caosVM::fireScript(shared_ptr<script> s, bool nointerrupt, Agent *frm) {
	assert(owner);
	assert(s);
	if (lock) return false; // can't interrupt scripts which called LOCK
	if (currentscript && nointerrupt) return false; // don't interrupt scripts with a timer script

	resetScriptState();
	currentscript = s;
	cip = nip = 0;
	targ = owner;
	from.set(frm);
	timeslice = 1;
	return true;
}

void caosVM::resetScriptState() {
	stop();
	resetCore();
}

void caosVM::resetCore() {
	if (blocking)
		delete blocking;
	blocking = NULL;
	
	valueStack.clear();
	callStack.clear();

	inst = lock = 0;
	timeslice = 0;

	c_FILE_ICLO(); // make sure input stream is freed
	inputstream = 0; // .. possibly not strictly necessary, when all is bugfree
	c_FILE_OCLO(); // make sure output stream is freed
	outputstream = &cout;

	_it_ = NULL;
	from = NULL;
	setTarg(owner);

	_p_[0].reset(); _p_[0].setInt(0); _p_[1].reset(); _p_[1].setInt(0);
	for (unsigned int i = 0; i < 100; i++) { var[i].reset(); var[i].setInt(0); }

	camera.reset();
}

void caosVM::tick() {
	stop_loop = false;
	while (currentscript && !stop_loop && (timeslice > 0 || inst)) {
		if (isBlocking()) return;
		runOp();
	}
}

/* vim: set noet: */
