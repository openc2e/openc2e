/*
 *  caosValue.cpp
 *  openc2e
 *
 *  Created by Bryan Donlan on Thu Mar 10 2005.
 *  Copyright (c) 2005-2006 Alyssa Milburn and Bryan Donlan. All rights reserved.
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

#define IN_CAOSVAR_CPP

#include "caosValue.h"
#include "Engine.h" // version
#include "World.h" // unid

#include <fmt/core.h>

#ifdef DONT_INLINE_CAOSVAR_ACCESSORS
#error meh, copy the stuff out of caosValue.h and drop it here
#endif

std::string caosValue::dump() const {
	switch(getType()) {
		case CAOSSTR:
			return fmt::format("String \"{}\" ", getString());
			break;
		case CAOSINT:
			return fmt::format("Int {} ", getInt());
			break;
		case CAOSFLOAT:
			return fmt::format("Float {} ", getFloat());
			break;
		case CAOSAGENT:
			return fmt::format("Agent {} ", (void *)getAgent().get());
			break;
		case CAOSVEC:
			return fmt::format("Vector ({}, {})", getVector().x, getVector().y);
			break;
		default:
			return "[bad caosValue!] ";
			break;
	};
}

bool caosValue::operator == (const caosValue &v) const {
	// todo: should be able to compare int and float, apparently
	if (this->hasInt() && v.hasInt()) {
		return this->getInt() == v.getInt();
	} else if (this->hasDecimal() && v.hasDecimal()) {
		return this->getFloat() == v.getFloat();
	} else if (this->hasString() && v.hasString()) {
		return this->getString() == v.getString();
	} else if (this->hasAgent() && v.hasAgent()) {
		return this->getAgent() == v.getAgent();
	} else if (this->hasVector() && v.hasVector()) {
		return this->getVector() == v.getVector();
	} else if (engine.version < 3) {
		// C1/C2 allow you to compare agents to an integer (unid), since agents are integers..
		// TODO: do this for >/< too?

		if (this->hasInt() && v.hasAgent()) {
			return world.lookupUNID(this->getInt()) == v.getAgent();
		} else if (v.hasInt() && this->hasAgent()) {
			return world.lookupUNID(v.getInt()) == this->getAgent();
		}
	}

	throw caosException(std::string("caosValue operator == couldn't compare ") + this->dump() + "and " + v.dump());
}

bool caosValue::operator > (const caosValue &v) const {
	if (this->hasDecimal() && v.hasDecimal()) {
		return this->getFloat() > v.getFloat();
	} else if (this->hasString() && v.hasString()) {
		return this->getString() > v.getString();
	} else if (this->hasVector() && v.hasVector()) {
		// XXX this is totally arbitrary
		const Vector<float> &v1 = this->getVector();
		const Vector<float> &v2 = v.getVector();
		if (v1.x > v2.x)
			return true;
		else if (v1.x < v2.x)
			return false;
		else if (v1.y > v2.y)
			return true;
		else return false;
	}
	
	throw caosException(std::string("caosValue operator > couldn't compare ") + this->dump() + "and " + v.dump());
}

bool caosValue::operator < (const caosValue &v) const {
	if (this->hasDecimal() && v.hasDecimal()) {
		return this->getFloat() < v.getFloat();
	} else if (this->hasString() && v.hasString()) {
		return this->getString() < v.getString();
	} else if (this->hasVector() && v.hasVector()) {
		return (*this != v) && !(*this > v);
	}
	
	throw caosException(std::string("caosValue operator < couldn't compare ") + this->dump() + "and " + v.dump());
}

AgentRef nullagentref;

// TODO: muh
const AgentRef &caosValue::agentVisit::operator()(int i) const {
	if (engine.version == 2) {
		if (i == 0) {
			return nullagentref;
		}

		// TODO: unid magic?
	}

	throw wrongCaosValueTypeException("Wrong caosValue type: Expected agent, got int");
}

/* vim: set noet: */
