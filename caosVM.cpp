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
#include <iostream>

caosVM::caosVM(Agent *o) {
	owner = o;
	// todo: if owner is Creature, set _it_
	// todo: http://www.gamewaredevelopment.co.uk/cdn/cdn_more.php?CDN_article_id=37 discusses
	// how targ is usually set to owner in event scripts, think about that
	targ = owner;
	resetScriptState();
}

bool caosVar::operator == (caosVar &v) {
	// todo: should be able to compare int and float, apparently
	if (this->hasInt() && v.hasInt()) {
		return (this->intValue == v.intValue);
	} else if (this->hasFloat() && v.hasFloat()) {
		return (this->floatValue == v.floatValue);
	} else if (this->hasString() && v.hasString()) {
		return (this->stringValue == v.stringValue);
	} else if (this->hasAgent() && v.hasAgent()) {
		return (this->agentValue == v.agentValue);
	}
	std::cerr << "caosVar operator == couldn't compare " << this->dump() << " and " << v.dump() << "\n";
	return false;
}

bool caosVar::operator > (caosVar &v) {
	// todo: should be able to compare int and float, apparently
	// todo: should be able to compare strings, apparently
	if (this->hasInt() && v.hasInt()) {
		return (this->intValue > v.intValue);
	} else if (this->hasFloat() && v.hasFloat()) {
		return (this->floatValue > v.floatValue);
	}
	std::cerr << "caosVar operator > couldn't compare " << this->dump() << " and " << v.dump() << "\n";
	return false;
}

bool caosVar::operator < (caosVar &v) {
	// todo: should be able to compare int and float, apparently
	// todo: should be able to compare strings, apparently
	if (this->hasInt() && v.hasInt()) {
		return (this->intValue < v.intValue);
	} else if (this->hasFloat() && v.hasFloat()) {
		return (this->floatValue < v.floatValue);
	}
	std::cerr << "caosVar operator < couldn't compare " << this->dump() << " and " << v.dump() << "\n";
	return false;
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
	if (maintoken.isvar) {
#ifdef CAOSDEBUG
		std::cerr << "internalRun got a var as the first token (" << maintoken.dump() << ")\n";
#endif
		throw badParamException();
	}
	cmdinfo *us = (first ? maintoken.cmd : maintoken.func);
	if (!us) {
#ifdef CAOSDEBUG
		std::cerr << "internalRun couldn't find cmdinfo for the first token (" << maintoken.dump() << ")\n";
#endif
		throw badParamException();
	}
#ifdef CAOSDEBUGDETAIL
	std::cerr << "internalRun called for " << us->dump() << "\n";
#endif
	// if the command/function needs parameters, we suck them up
	for (unsigned int i = 0; i < us->notokens; i++) {
		if (tokens.empty()) throw notEnoughParamsException();
		token atoken = tokens.front();
		if (!atoken.isvar) {
			// if the token used for this parameter is a command/function
			// then we execute it, passing it a reference to our tokens
			// so it can suck them up (obviously we want them gone)
			ourparams.push_back(internalRun(tokens, false));
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
			if (tokens.front().isvar) {
				first = tokens.front().var;
				tokens.pop_front();
			} else {
				first = internalRun(tokens, false);
			}

			// we die if this isn't valid in handleException
			comparisonType comparison = tokens.front().comparison; tokens.pop_front();

			if (tokens.front().isvar) {
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
	if (first && !tokens.empty()) {
		// assume there's another command on this line
		internalRun(tokens, true);
	}
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

	currentscript = 0;
	currentline = 0;
	locked = false;
	noschedule = false;
	blockingticks = 0;

	targ = 0;
	_it_ = 0;

	_p_[0].reset(); _p_[1].reset();
	for (unsigned int i = 0; i < 100; i++) { var[i].reset(); }
}

void caosVM::tick() {
	if (blockingticks) { blockingticks--; return; }
	if (!currentscript) return;
	while (currentline < currentscript->lines.size()) {
		runCurrentLine();
	}
	currentscript = 0;
}

void caosVM::runCurrentLine() {
	unsigned int i = currentline;
	std::list<token> b = currentscript->lines[currentline];
	try {
		if (!b.empty()) internalRun(b, true);
	} catch (badParamException e) {
		std::cerr << "caught badParamException running " << currentscript->rawlines[i] << "\n";
	} catch (notEnoughParamsException e) {
		std::cerr << "caught notEnoughParamsException running " << currentscript->rawlines[i] << "\n";
	}
	/* Generally, we want to proceed to the next line. Sometimes, opcodes will change the
		current line from under us, and in those instances, we should leave it alone. */
	if (currentline == i) currentline++;
}
