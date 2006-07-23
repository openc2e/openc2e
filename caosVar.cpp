/*
 *  caosVar.cpp
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

#include "caosVar.h"

#include <iostream>
#include <sstream>
#include <boost/format.hpp>

#ifdef DONT_INLINE_CAOSVAR_ACCESSORS
#error meh, copy the stuff out of caosVar.h and drop it here
#endif

using boost::str;

static inline std::string stringify(double x) {
	std::ostringstream o;
	if (!(o << x)) throw "stringify() failed";
	return o.str();
}

std::string caosVar::dump() const {
	switch(getType()) {
		case STRING:
			return str(boost::format("String \"%s\" ") % getString());
			break;
		case INTEGER:
			return str(boost::format("Int %d ") % getInt());
			break;
		case FLOAT:
			return str(boost::format("Float %f ") % getFloat());
			break;
		case AGENT:
			return str(boost::format("Agent %p ") % (Agent *)getAgent().get());
			break;
		default:
			return "[bad caosVar!] ";
			break;
	};
}

bool caosVar::operator == (const caosVar &v) const {
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
	}

	throw caosException(std::string("caosVar operator == couldn't compare ") + this->dump() + "and " + v.dump());
}

bool caosVar::operator > (const caosVar &v) const {
	if (this->hasDecimal() && v.hasDecimal()) {
		return this->getFloat() > v.getFloat();
	} else if (this->hasString() && v.hasString()) {
		return this->getString() > v.getString();
	} else if (this->hasVector() && v.hasVector()) {
		// XXX this is totally arbitrary
		const Vector &v1 = this->getVector();
		const Vector &v2 = v.getVector();
		if (v1.x > v2.x)
			return true;
		else if (v1.x < v2.x)
			return false;
		else if (v1.y > v2.y)
			return true;
		else return false;
	}
	
	throw caosException(std::string("caosVar operator > couldn't compare ") + this->dump() + "and " + v.dump());
}

bool caosVar::operator < (const caosVar &v) const {
	if (this->hasDecimal() && v.hasDecimal()) {
		return this->getFloat() < v.getFloat();
	} else if (this->hasString() && v.hasString()) {
		return this->getString() < v.getString();
	} else if (this->hasVector() && v.hasVector()) {
		return (*this != v) && !(*this > v);
	}
	
	throw caosException(std::string("caosVar operator < couldn't compare ") + this->dump() + "and " + v.dump());
}


/* vim: set noet: */
