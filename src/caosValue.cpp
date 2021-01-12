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

#include "caosValue.h"

#include "Engine.h" // version
#include "World.h" // unid
#include "utils/overload.h"

#include <fmt/core.h>

const char* variableTypeToString(variableType type) {
	switch (type) {
		case CAOSNULL:
			return "null";
		case CAOSAGENT:
			return "agent";
		case CAOSINT:
			return "integer";
		case CAOSFLOAT:
			return "float";
		case CAOSSTR:
			return "string";
		case CAOSBYTESTRING:
			return "bytestring";
		case CAOSFACEVALUE:
			return "facevalue";
		case CAOSVEC:
			return "vector";
	}
}

struct typeVisit {
	variableType operator()(int) const { return CAOSINT; }
	variableType operator()(float) const { return CAOSFLOAT; }
	variableType operator()(const std::string&) const { return CAOSSTR; }
	variableType operator()(const AgentRef&) const { return CAOSAGENT; }
	variableType operator()(nulltype_tag) const { return CAOSNULL; }
	variableType operator()(const bytestring_t&) const { return CAOSBYTESTRING; }
	variableType operator()(const FaceValue&) const { return CAOSFACEVALUE; }
	variableType operator()(const Vector<float>&) const { return CAOSVEC; }
};

#define BAD_TYPE(et, gt) \
	std::conditional<std::is_fundamental<et>::value, et, std::add_lvalue_reference<std::add_const<et>::type>::type>::type \
	operator()(const gt&) const { \
		throw wrongCaosValueTypeException( \
			"Wrong caosValue type: Expected " #et ", got " #gt); \
	}

struct intVisit {
	int operator()(int i) const { return i; }
	int operator()(float f) const {
		// horror necessary for rounding without C99
		int x = (int)f;
		float diff = f - x;
		if (f >= 0.0f) {
			if (diff >= 0.5f)
				return ++x;
			else
				return x;
		} else {
			if (diff <= -0.5f)
				return --x;
			else
				return x;
		}
	}
	int operator()(const FaceValue& fv) const { return fv.pose; }
	int operator()(const Vector<float>& v) const {
		return (int)v.getMagnitude();
	}
	BAD_TYPE(int, std::string);
	BAD_TYPE(int, AgentRef);
	BAD_TYPE(int, bytestring_t);
	BAD_TYPE(int, nulltype_tag);
};

struct floatVisit {
	float operator()(int i) const { return (float)i; }
	float operator()(float f) const { return f; }
	float operator()(const Vector<float>& v) const { return v.getMagnitude(); }
	float operator()(const FaceValue& fv) const { return fv.pose; }
	BAD_TYPE(float, std::string);
	BAD_TYPE(float, AgentRef);
	BAD_TYPE(float, bytestring_t);
	BAD_TYPE(float, nulltype_tag);
};

struct stringVisit {
	const std::string& operator()(const std::string& s) const {
		return s;
	}
	const std::string& operator()(const FaceValue& fv) const {
		return fv.sprite_filename;
	}
	BAD_TYPE(std::string, AgentRef);
	BAD_TYPE(std::string, nulltype_tag);
	BAD_TYPE(std::string, int);
	BAD_TYPE(std::string, float);
	BAD_TYPE(std::string, bytestring_t);
	BAD_TYPE(std::string, Vector<float>);
};

struct agentVisit {
	const AgentRef& operator()(const AgentRef& a) const {
		return a;
	}
	BAD_TYPE(AgentRef, std::string);
	BAD_TYPE(AgentRef, nulltype_tag);
	const AgentRef& operator()(int i) const;
	BAD_TYPE(AgentRef, float);
	BAD_TYPE(AgentRef, bytestring_t);
	BAD_TYPE(AgentRef, FaceValue);
	BAD_TYPE(AgentRef, Vector<float>);
};

struct vectorVisit {
	const Vector<float>& operator()(const Vector<float>& v) const {
		return v;
	}
	BAD_TYPE(Vector<float>, std::string);
	BAD_TYPE(Vector<float>, nulltype_tag);
	BAD_TYPE(Vector<float>, int);
	BAD_TYPE(Vector<float>, float);
	BAD_TYPE(Vector<float>, bytestring_t);
	BAD_TYPE(Vector<float>, FaceValue);
	BAD_TYPE(Vector<float>, AgentRef);
};


#undef BAD_TYPE

variableType caosValue::getType() const {
	return visit(typeVisit(), value);
}

void caosValue::reset() {
	value = nulltype_tag();
}

bool caosValue::isNull() const {
	return getType() == CAOSNULL;
}

caosValue::caosValue() {
}

caosValue::~caosValue() {
}

caosValue& caosValue::operator=(const caosValue& copyFrom) {
	value = copyFrom.value;
	return *this;
}

caosValue::caosValue(const caosValue& copyFrom)
	: value(copyFrom.value) {
}

caosValue::caosValue(int v) {
	setInt(v);
}
caosValue::caosValue(float v) {
	setFloat(v);
}
caosValue::caosValue(Agent* v) {
	setAgent(v);
}
caosValue::caosValue(const AgentRef& v) {
	setAgent(v);
}
caosValue::caosValue(const std::string& v) {
	setString(v);
}
caosValue::caosValue(const bytestring_t& v) {
	setByteStr(v);
}
caosValue::caosValue(const FaceValue& v)
	: value(v) {
}
caosValue::caosValue(const Vector<float>& v) {
	setVector(v);
}

bool caosValue::isEmpty() const {
	return getType() == CAOSNULL;
}
bool caosValue::hasInt() const {
	return getType() == CAOSINT || getType() == CAOSFACEVALUE;
}
bool caosValue::hasFloat() const {
	return getType() == CAOSFLOAT;
}
bool caosValue::hasAgent() const {
	return getType() == CAOSAGENT;
}
bool caosValue::hasString() const {
	return getType() == CAOSSTR || getType() == CAOSFACEVALUE;
}
bool caosValue::hasDecimal() const {
	return getType() == CAOSINT || getType() == CAOSFLOAT || getType() == CAOSFACEVALUE || getType() == CAOSVEC;
}
bool caosValue::hasNumber() const {
	return hasDecimal();
}
bool caosValue::hasByteStr() const {
	return getType() == CAOSBYTESTRING;
}
bool caosValue::hasVector() const {
	return getType() == CAOSVEC;
}

void caosValue::setInt(int i) {
	value = i;
}
void caosValue::setFloat(float i) {
	value = i;
}
void caosValue::setAgent(Agent* i) {
	value = AgentRef(i);
}
void caosValue::setAgent(const AgentRef& r) {
	value = r;
}
void caosValue::setString(const std::string& i) {
	value = i;
}
void caosValue::setByteStr(const bytestring_t& bs) {
	value = bs;
}
void caosValue::setVector(const Vector<float>& v) {
	value = v;
}

int caosValue::getInt() const {
	return visit(intVisit(), value);
}

float caosValue::getFloat() const {
	return visit(floatVisit(), value);
}

void caosValue::getString(std::string& s) const {
	s = getString();
}

const std::string& caosValue::getString() const {
	return visit(stringVisit(), value);
}

std::shared_ptr<Agent> caosValue::getAgent() const {
	return getAgentRef().lock();
}

const AgentRef& caosValue::getAgentRef() const {
	return visit(agentVisit(), value);
}

const bytestring_t& caosValue::getByteStr() const {
	return visit(overload(
					 [](const bytestring_t& bs) -> const bytestring_t& { return bs; },
					 [](const auto&) -> const bytestring_t& {
						 throw wrongCaosValueTypeException("Wrong caosValue type: Expected bytestring");
					 }),
		value);
}

const Vector<float>& caosValue::getVector() const {
	return visit(vectorVisit(), value);
}

std::string caosValue::dump() const {
	switch (getType()) {
		case CAOSNULL:
			return "Null";
		case CAOSSTR:
			return fmt::format("String \"{}\" ", getString());
		case CAOSINT:
			return fmt::format("Int {} ", getInt());
		case CAOSFLOAT:
			return fmt::format("Float {} ", getFloat());
		case CAOSAGENT:
			return fmt::format("Agent {} ", (void*)getAgent().get());
		case CAOSBYTESTRING: {
			std::string buf = "Bytestring [";
			for (auto c : getByteStr()) {
				buf += std::to_string((int)c);
				buf += " ";
			}
			buf += "]";
			return buf;
		}
		case CAOSFACEVALUE:
			return fmt::format("FaceValue ({}, \"{}\")", getInt(), getString());
		case CAOSVEC:
			return fmt::format("Vector ({}, {})", getVector().x, getVector().y);
	};
}

bool caosValue::operator==(const caosValue& v) const {
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

bool caosValue::operator>(const caosValue& v) const {
	if (this->hasDecimal() && v.hasDecimal()) {
		return this->getFloat() > v.getFloat();
	} else if (this->hasString() && v.hasString()) {
		return this->getString() > v.getString();
	} else if (this->hasVector() && v.hasVector()) {
		// XXX this is totally arbitrary
		const Vector<float>& v1 = this->getVector();
		const Vector<float>& v2 = v.getVector();
		if (v1.x > v2.x)
			return true;
		else if (v1.x < v2.x)
			return false;
		else if (v1.y > v2.y)
			return true;
		else
			return false;
	}

	throw caosException(std::string("caosValue operator > couldn't compare ") + this->dump() + "and " + v.dump());
}

bool caosValue::operator<(const caosValue& v) const {
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
const AgentRef& agentVisit::operator()(int i) const {
	if (engine.version == 2) {
		if (i == 0) {
			return nullagentref;
		}

		// TODO: unid magic?
	}

	throw wrongCaosValueTypeException("Wrong caosValue type: Expected agent, got int");
}

/* vim: set noet: */
