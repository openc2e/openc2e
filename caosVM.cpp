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
#include <iostream>

caosVM::caosVM(const AgentRef &o) {
	setOwner(o);
	setTarg(owner);
	resetScriptState();
}

bool handleComparison(caosVar &one, caosVar &two, comparisonType compare) {
	switch (compare) {
		case EQ:
			return (one == two);
		case NE:
			return (one != two);
		case GE:
			if (one == two) return true;
		case GT:
			return (one > two);
		case LE:
			if (one == two) return true;
		case LT:
			return (one < two);
		default:
			throw badParamException();
	}
}

caosVar caosVM::internalRun(std::list<token> &tokens, bool first) {
	// v- this is the command/function we're executing
	std::vector<caosVar> ourparams;
	token maintoken = tokens.front();
	tokens.pop_front();
	assert(maintoken.type == token::FUNCTION);
	cmdinfo *us = maintoken.func;
#ifdef CAOSDEBUGDETAIL
	std::cerr << "internalRun called for " << us->dump() << "\n";
#endif
	// if the command/function needs parameters, we suck them up
	for (unsigned int i = 0; i < us->notokens; i++) {
		if (tokens.empty()) throw notEnoughParamsException();
		token &atoken = tokens.front();
		if (atoken.type == token::FUNCTION) {
			// if the token used for this parameter is a command/function
			// then we execute it, passing it a reference to our tokens
			// so it can suck them up (obviously we want them gone)
			ourparams.push_back(internalRun(tokens, false));
		} else if (atoken.type == token::LABEL) {
			// temporary hack for labels
			tokens.pop_front();
			caosVar v;
			v.setString(atoken.data);
			ourparams.push_back(v);
		} else {
			tokens.pop_front();
			ourparams.push_back(atoken.var);
		}
	}
	if (us->needscondition) {
		comparisonType last = NONE;
		// pass over the tokens multiple times, picking up the condition data
		while (!tokens.empty()) {
			if (tokens.size() < 3) throw badParamException(); // we need at least three tokens per pass

			// grab [token] [condition] [token]
			caosVar first, second;
			if (tokens.front().type == token::CAOSVAR) {
				first = tokens.front().var;
				tokens.pop_front();
			} else {
				first = internalRun(tokens, false);
			}

			// we die if this isn't valid in handleException
			comparisonType comparison = tokens.front().comparison; tokens.pop_front();

			if (tokens.front().type == token::CAOSVAR) {
				second = tokens.front().var;
				tokens.pop_front();
			} else {
				second = internalRun(tokens, false);
			}

			// now handle the comparison
			bool ourtruth = handleComparison(first, second, comparison);
			if (last == NONE) truth = ourtruth;
			else if (last == AND) truth = ourtruth && truth;
			else truth = ourtruth || truth;

			// and finally check for AND/OR joins, for the next pass
			if (!tokens.empty()) {
				last = tokens.front().comparison; tokens.pop_front();
				if ((last != AND) && (last != OR))
					throw badParamException();
			}
		}
	}
	params = ourparams;
	result.reset();
#ifdef CAOSDEBUGDETAIL
	std::cerr << "internalRun executing " << us->dump() << "\n";
#endif
	varnumber = maintoken.varnumber; // VAxx/OVxx hack
	(this->*(us->method))();
	if (!params.empty()) {
		std::cerr << "warning: CAOS function " << maintoken.dump() << " didn't pop all parameters!\n";
	}
	if (first)
		assert(tokens.empty());
	return result;
}

void caosVM::runEntirely(script &s) {
	resetScriptState();
	currentscript = &s;
	while (currentline < currentscript->lines.size()) {
		runCurrentLine();
	}
	currentscript = 0;
}

void caosVM::fireScript(script &s, bool nointerrupt) {
	if (locked) return; // can't interrupt scripts which called LOCK
	if (currentscript && nointerrupt) return; // don't interrupt scripts with a timer script
	resetScriptState();
	currentscript = &s;
}

void caosVM::resetScriptState() {
	truthstack.clear();
	linestack.clear();
	repstack.clear();
	enumstack.clear();

	currentscript = 0;
	currentline = 0;
	locked = false;
	noschedule = false;
	blockingticks = 0;
	blocking = false;

	setTarg(0);
	_it_ = 0;

	_p_[0].reset(); _p_[0].setInt(0); _p_[1].reset(); _p_[1].setInt(0);
	for (unsigned int i = 0; i < 100; i++) { var[i].reset(); var[i].setInt(0); }
}

void caosVM::tick() {
	assert(currentscript);
	if (blockingticks) { blockingticks--; return; }
	unsigned int n = 0;
	// run 5 lines per tick
	while ((currentline < currentscript->lines.size()) && (noschedule || n < 5)) {
		runCurrentLine();
		if (blocking) return; // todo: should we check for noschedule/etc?
		n++;
		if (!currentscript) return;
	}
	if (currentline == currentscript->lines.size()) {
		stop();
	}
}

void caosVM::runCurrentLine() {
	unsigned int i = currentline;
	std::list<token> b = currentscript->lines[currentline];
	try {
		if (!b.empty()) internalRun(b, true);
	} catch (badParamException e) {
		std::cerr << "caught badParamException while running '" << currentscript->dumpLine(currentline) << "' (line #" << i << ")" << std::endl;
	} catch (notEnoughParamsException e) {
		std::cerr << "caught notEnoughParamsException - this is an openc2e bug!\n";
	} catch (assertFailure e) {
		std::cerr.flush();
		std::cerr << "caught assert failure '" << e.what() << "' while running '" << currentscript->dumpLine(currentline) << "' (line #" << i << ")" << std::endl;
		currentline = currentscript->lines.size();
	}
	/* Generally, we want to proceed to the next line. Sometimes, opcodes will change the
		current line from under us, and in those instances, we should leave it alone. */
	if ((currentline == i) && (!blocking)) currentline++;
}

void caosVM::stop() {
	currentscript = 0;
	if (owner)
		owner->vm = 0;
	world.freeVM(this);
	locked = false;
}

