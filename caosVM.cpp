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
	// if owner is Creature, set _it_
	targ.reset();
	// todo: http://www.gamewaredevelopment.co.uk/cdn/cdn_more.php?CDN_article_id=37 discusses
	// how targ is usually set to owner in event scripts, think about that
	targ.setAgent(0);
}

bool caosVar::operator == (caosVar &v) {
	if (this->hasInt() && v.hasInt()) {
		return (this->intValue == v.intValue);
	} else if (this->hasFloat() && v.hasFloat()) {
		return (this->floatValue == v.floatValue);
	} else if (this->hasString() && v.hasString()) {
		return (this->stringValue == v.stringValue);
	} else if (this->hasAgent() && v.hasAgent()) {
		return (this->agentValue == v.agentValue);
	}
	std::cerr << "caosVar operator == couldn't compare\n";
	return false;
}

bool caosVar::operator > (caosVar &v) {
	if (this->hasInt() && v.hasInt()) {
		return (this->intValue > v.intValue);
	} else if (this->hasFloat() && v.hasFloat()) {
		return (this->floatValue > v.floatValue);
	}
	std::cerr << "caosVar operator > couldn't compare\n";
	return false;
}

bool caosVar::operator < (caosVar &v) {
	if (this->hasInt() && v.hasInt()) {
		return (this->intValue < v.intValue);
	} else if (this->hasFloat() && v.hasFloat()) {
		return (this->floatValue < v.floatValue);
	}
	std::cerr << "caosVar operator < couldn't compare\n";
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
	if (maintoken.isvar) throw badParamException();
	cmdinfo *us = (first ? maintoken.cmd : maintoken.func);
	if (!us) throw badParamException();
	std::cout << "internalRun called for " << us->dump() << "\n";
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
	std::cout << "internalRun executing " << us->dump() << "\n";
	(this->*(us->method))();
	if (first && !tokens.empty()) {
		// assume there's another command on this line
		internalRun(tokens, true);
	}
	return result;
}

void caosVM::runEntirely() {
  for (std::vector<std::list<token> >::iterator i = script->lines.begin();
			 i != script->lines.end(); i++) {
		std::list<token> b = *i;
		try {
			if (!b.empty()) internalRun(b, true);
		} catch (badParamException e) {
			std::cerr << "caught badParamException\n";
		} catch (notEnoughParamsException e) {
			std::cerr << "caught notEnoughParamsException\n";
		}
	}
}

void caosVM::runCurrentLine() {
}

void caosVM::pushLineOntoStack(int line) {
}

int caosVM::popStackLine() {
	return -1;
}
