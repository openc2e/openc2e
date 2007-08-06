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

#include <boost/format.hpp>

using std::cout;
using std::cerr;
using std::endl;

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

inline void caosVM::safeJMP(int dest) {
//	std::cerr << "jmp from " << cip << " to " << dest << "(old nip = " << nip << ") in script of length " << currentscript->scriptLength() << std::endl;
	if (dest < 0) {
		std::cerr << currentscript->dump();
		throw caosException(boost::str(boost::format("Internal error: Unrelocated jump at %08x") % cip));
	}
	if (dest >= currentscript->scriptLength()) {
		std::cerr << currentscript->dump();
		throw creaturesException(boost::str(boost::format("Internal error: Jump out of bounds at %08x") % cip));
	}
	nip = dest;
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
		case CAOS_CMD:
			{
				const cmdinfo *ci = s->dialect->getcmd(op.argument);
				if (!inst)
					timeslice -= ci->evalcost;
#ifndef VCPP_BROKENNESS
				(this->*(ci->handler))();
#else
				dispatchCAOS(this, ci->handler_idx);
#endif
				break;
			}
		case CAOS_COND:
			{
				VM_PARAM_VALUE(v2);
				VM_PARAM_VALUE(v1);
				VM_PARAM_INTEGER(condaccum);
				int condition = op.argument;
				int result;
#define COMPARE(type, get) do {\
	type v1a, v2a; \
	v1a = v1.get(); \
	v2a = v2.get(); \
	if (v1a > v2a) \
		result = CGT; \
	else if (v1a < v2a) \
		result = CLT; \
	else \
		result = CEQ; \
} while (0)
				if (v1.getType() == INTEGER && v2.getType() == INTEGER) {
					COMPARE(int, getInt);
					if (v2.getInt() == (v1.getInt() & v2.getInt()))
						result |= CBT;
					if (0           == (v1.getInt() & v2.getInt()))
						result |= CBF;
				} else if (v1.hasNumber() && v2.hasNumber())
					COMPARE(float, getFloat);
				else if (v1.hasString() && v2.hasString())
					COMPARE(std::string, getString);
				else if (v1.hasAgent() && v2.hasAgent())
					COMPARE(AgentRef, getAgentRef().get);
				else if (v1.isEmpty() && v2.isEmpty())
					result = CEQ;
				else
					result = 0; // XXX?
				if (condition & CAND)
					this->result.setInt(condaccum && !!(result & condition & CMASK));
				else
					this->result.setInt(condaccum || !!(result & condition & CMASK));
				break;
#undef COMPARE
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
		case CAOS_VAXX:
			{
				valueStack.push_back(vmStackItem(&var[op.argument]));
				break;
			}
		case CAOS_OVXX:
			{
				valid_agent(targ);
				valueStack.push_back(vmStackItem(&targ->var[op.argument]));
				break;
			}
		case CAOS_MVXX:
			{
				valid_agent(owner);
				valueStack.push_back(vmStackItem(&owner->var[op.argument]));
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
					result.setInt(counter);
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
				if (!v.hasAgent())
					throw caosException("Stack item type mismatch");
				targ = v.getAgentRef();
				safeJMP(op.argument);
				break;
			}
		default:
			throw creaturesException(boost::str(boost::format("Illegal opcode %d") % (int)op.opcode));
	}
}

inline void caosVM::runOp() {
	cip = nip;
	nip++;
	shared_ptr<script> scr = currentscript;
	caosOp op = currentscript->getOp(cip);
	result.reset(); // xxx this belongs in opcode maybe
	try {
		if (trace) {
			std::cerr
				<< boost::str(boost::format("optrace(%s): INST=%d TS=%d %p @%08d top=%s ") % scr->filename.c_str() % (int)inst % (int)timeslice % (void *)this % cip % (valueStack.empty() ? std::string("(empty)") : valueStack.back().dump()))
				<< dumpOp(op) << std::endl;
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
	part = 0;

	_p_[0].reset(); _p_[0].setInt(0); _p_[1].reset(); _p_[1].setInt(0);
	for (unsigned int i = 0; i < 100; i++) { var[i].reset(); var[i].setInt(0); }

	camera.reset();

	trace = false;
}

void caosVM::tick() {
	stop_loop = false;
	while (currentscript && !stop_loop && (timeslice > 0 || inst)) {
		if (isBlocking()) return;
		runOp();
	}
}

void caosVM::dummy_cmd() {
	// no-op
}
/* vim: set noet: */
