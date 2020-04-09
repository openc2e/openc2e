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
#include "caosScript.h"
#include <cassert>
#include <iostream>
#include <climits>
#include <memory>

#include <fmt/printf.h>

using std::cout;
using std::cerr;
using std::endl;

void dumpStack(caosVM *vm) {
	std::cerr << "\tvalueStack: ";
	int i, c = 0;
	for (i = vm->valueStack.size() - 1; i >= 0 && c++ < 5; i--)
		std::cerr << vm->valueStack[i].dump() << " | ";
	if (i >= 0)
		std::cerr << "...";
	else
		std::cerr << "END";
	std::cerr << std::endl;
}

caosVM::caosVM(const AgentRef &o)
	: vm(this)
{
	owner = o;
	currentscript.reset();
	std::fill(var.begin(), var.end(), 0);
	cip = nip = 0;
	blocking = NULL;
	inputstream = 0; outputstream = 0;
	resetCore();
	trace = false;
}

caosVM::~caosVM() {
	resetCore(); // delete blocking, close streams
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

inline void caosVM::safeJMP(int dest) {
//	std::cerr << "jmp from " << cip << " to " << dest << "(old nip = " << nip << ") in script of length " << currentscript->scriptLength() << std::endl;
	if (dest < 0) {
		std::cerr << currentscript->dump();
		throw caosException(fmt::sprintf("Internal error: Unrelocated jump at %08x", cip));
	}
	if (dest >= currentscript->scriptLength()) {
		std::cerr << currentscript->dump();
		throw creaturesException(fmt::sprintf("Internal error: Jump out of bounds at %08x", cip));
	}
	nip = dest;
}

inline void caosVM::invoke_cmd(script *s, bool is_saver, int opidx) {
	const cmdinfo *ci = s->dialect->getcmd(opidx);
	// We subtract two here to account for a) the missing return, and b)
	// consuming the new value.
	int stackdelta = ci->stackdelta - (is_saver ? 2 : 0);
	unsigned int stackstart = valueStack.size();
	assert(result.isNull());
#ifndef VCPP_BROKENNESS
	if (is_saver)
		(this->*(ci->savehandler))();
	else
		(this->*(ci->handler))();
#else
	if (is_saver)
		dispatchCAOS(this, ci->savehandler_idx);
	else
		dispatchCAOS(this, ci->handler_idx);
#endif
	if (!is_saver && !result.isNull()) {
		valueStack.push_back(result);
		result.reset();
	} else {
		assert(result.isNull());
	}
	if (stackdelta < INT_MAX - 1) {
		if ((int)stackstart + stackdelta != (int)valueStack.size()) {
			dumpStack(this);
			throw caosException(fmt::sprintf(
				"Internal error: Stack imbalance detected: expected to be %d after start of %d, but stack size is now %d",
				stackdelta, (int)stackstart, (int)valueStack.size()
			));
		}
	}
}

inline void caosVM::runOpCore(script *s, caosOp op) {
	switch (op.opcode) {
		case CAOS_NOP: break;
		case CAOS_DIE:
			{
				int idx = op.argument;
				std::string err = "aborted";
				caosVar constVal = s->getConstant(idx);
				if (constVal.hasString())
					err = constVal.getString();
				throw creaturesException(err);
			}
		case CAOS_STOP:
			{
				stop();
				break;
			}
		case CAOS_SAVE_CMD:
			{
				invoke_cmd(s, true, op.argument);
				break;
			}
		case CAOS_CMD:
			{
				invoke_cmd(s, false, op.argument);
				break;
			}
		case CAOS_YIELD:
			{
#ifndef NDEBUG
				// This condition can arise as a result of bad save data,
				// so an assert is not appropriate... or is it?
				//
				// In any case, it is mostly harmless but should never occur,
				// as it indicates a bug in the CAOS compiler.
				caos_assert(auxStack.size() == 0);
#endif
				if (!inst)
					timeslice -= op.argument;
				break;
			}
		case CAOS_COND:
			{
				VM_PARAM_VALUE(v2);
				VM_PARAM_VALUE(v1);
				VM_PARAM_INTEGER(condaccum);
				assert(!v1.isEmpty());
				assert(!v2.isEmpty());
				int condition = op.argument;
				if (condition & CAND) condition -= CAND;
				if (condition & COR) condition -= COR;
				int result = 0;
				if (condition == CEQ)
					result = (v1 == v2);
				if (condition == CNE)
					result = !(v1 == v2);
				
				if (condition == CLT)
					result = (v1 < v2);
				if (condition == CGE)
					result = !(v1 < v2);
				if (condition == CGT)
					result = (v1 > v2);
				if (condition == CLE)
					result = !(v1 > v2);

				if (condition == CBT) {
					caos_assert(v1.hasInt() && v2.hasInt());
					result = (v2.getInt() == (v1.getInt() & v2.getInt()));
				}
				if (condition == CBF) {
					caos_assert(v1.hasInt() && v2.hasInt());
					result = (0           == (v1.getInt() & v2.getInt()));
				}
				if (op.argument & CAND)
					result = (condaccum && result);
				else
					result = (condaccum || result);
				valueStack.push_back(caosVar(result));
				break;
			}
		case CAOS_CONST:
			{
				valueStack.push_back(vmStackItem(s->getConstant(op.argument)));
				break;
			}
		case CAOS_CONSTINT:
			{
				valueStack.push_back(vmStackItem(caosVar(op.argument)));
				break;
			}
		case CAOS_BYTESTR:
			{
				valueStack.push_back(vmStackItem(s->getBytestr(op.argument)));
				break;
			}
		case CAOS_PUSH_AUX:
			{
				caos_assert(op.argument >= 0);
				caos_assert(op.argument < (int)valueStack.size());
				auxStack.push_back(valueStack[valueStack.size() - op.argument - 1]);
				break;
			}
		case CAOS_RESTORE_AUX:
			{
				caos_assert(op.argument >= 0);
				caos_assert(op.argument <= (int)auxStack.size());
				for (int i = 0; i < op.argument; i++) {
					valueStack.push_back(auxStack.back());
					auxStack.pop_back();
				}	
				break;				
			}
		case CAOS_STACK_ROT:
			{
				caos_assert(op.argument >= 0);
				caos_assert(op.argument < (int)valueStack.size());
				for (int i = 0; i < op.argument; i++) {
					int top = valueStack.size() - 1;
					std::swap(valueStack[top - i], valueStack[top - i - 1]);
				}
				break;
			}
		case CAOS_CJMP:
			{
				VM_PARAM_VALUE(v);
				if (v.getInt() != 0)
					safeJMP(op.argument);
				break;
			}
		case CAOS_JMP:
			{
				safeJMP(op.argument);
				break;
			}
		case CAOS_DECJNZ:
			{
				VM_PARAM_INTEGER(counter);
				counter--;
				if (counter) {
					safeJMP(op.argument);
					valueStack.push_back(caosVar(counter));
				}
				break;
			}
		case CAOS_GSUB:
			{
				callStack.push_back(callStackItem());
				callStack.back().nip = nip;
				callStack.back().valueStack.swap(valueStack);
				safeJMP(op.argument);
				break;				
			}
		case CAOS_ENUMPOP:
			{
				VM_PARAM_VALUE(v);
				if (v.isEmpty())
					break;
				if (!v.hasAgent()) {
					dumpStack(this);
					throw caosException(std::string("Stack item type mismatch: ") + v.dump());
				}
				targ = v.getAgentRef();
				safeJMP(op.argument);
				break;
			}
		default:
			throw creaturesException(fmt::sprintf("Illegal opcode %d", (int)op.opcode));
	}
}

inline void caosVM::runOp() {
	cip = nip;
	nip++;
	
	runops++;
	if (runops > 1000000) throw creaturesException("script exceeded 1m ops");

	shared_ptr<script> scr = currentscript;
	caosOp op = currentscript->getOp(cip);
	
	try {
		if (trace) {
			std::cerr
				<< fmt::sprintf(
					"optrace(%s): INST=%d TS=%d %p @%08d top=%s depth=%d ",
					scr->filename.c_str(), (int)inst, (int)timeslice,
					(void *)this, cip,
					(valueStack.empty() ? std::string("(empty)") : valueStack.back().dump()),
					valueStack.size()
				)
				<< dumpOp(currentscript->dialect, op)
				<< std::endl;
			if (trace >= 2) {
				dumpStack(this);
			}
		}
		runOpCore(scr.get(), op);
	} catch (caosException &e) {
		e.trace(currentscript, op.traceindex);
		stop();
		throw;
	} catch (creaturesException &e) {
		stop();
		throw;
	}
	
}

void caosVM::stop() {
	lock = false;
	currentscript.reset();
	std::fill(var.begin(), var.end(), 0);
}

void caosVM::runEntirely(shared_ptr<script> s) {
	// caller is responsible for resetting/setting *all* state!
	cip = nip = runops = 0;
	currentscript = s;

	while (true) {
		runOp();
		if (!currentscript) break;
		if (blocking) {
			delete blocking;
			blocking = NULL;
			throw creaturesException("blocking in an installation script");
		}
	}
}

bool caosVM::fireScript(shared_ptr<script> s, bool nointerrupt, Agent *frm) {
	assert(owner);
	assert(s);
	if (lock) return false; // can't interrupt scripts which called LOCK
	if (currentscript && nointerrupt) return false; // don't interrupt scripts with a timer script

	resetScriptState();
	currentscript = s;
	targ = owner;
	from.setAgent(frm);
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
	result.reset();
	
	valueStack.clear();
	auxStack.clear();
	callStack.clear();

	inst = lock = 0;
	timeslice = 0;

	c_FILE_ICLO(); // make sure input stream is freed
	inputstream = 0; // .. possibly not strictly necessary, when all is bugfree
	c_FILE_OCLO(); // make sure output stream is freed
	outputstream = 0;

	_it_ = NULL;
	from.setAgent(NULL);
	setTarg(owner);
	part = 0;

	_p_[0].reset(); _p_[0].setInt(0); _p_[1].reset(); _p_[1].setInt(0);
	std::fill(var.begin(), var.end(), 0);

	camera.reset();

	trace = 0;
	cip = nip = runops = 0;
}

void caosVM::tick() {
	stop_loop = false;
	runops = 0;
	while (currentscript && !stop_loop && (timeslice > 0 || inst)) {
		if (isBlocking()) return;
		runOp();
	}
}

void caosVM::dummy_cmd() {
	// no-op
}
/* vim: set noet: */
