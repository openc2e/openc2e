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

#include <fmt/core.h>

using namespace mpark;

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

variableType caosValue::getType() const {
	if (mpark::holds_alternative<int>(value)) {
		return CAOSINT;
	} else if (mpark::holds_alternative<float>(value)) {
		return CAOSFLOAT;
	} else if (mpark::holds_alternative<std::string>(value)) {
		return CAOSSTR;
	} else if (mpark::holds_alternative<AgentRef>(value)) {
		return CAOSAGENT;
	} else if (mpark::holds_alternative<nulltype_tag>(value)) {
		return CAOSNULL;
	} else if (mpark::holds_alternative<bytestring_t>(value)) {
		return CAOSBYTESTRING;
	} else if (mpark::holds_alternative<FaceValue>(value)) {
		return CAOSFACEVALUE;
	} else if (mpark::holds_alternative<Vector<float>>(value)) {
		return CAOSVEC;
	} else {
		throw wrongCaosValueTypeException(fmt::format("getType not implemented for: {}", dump()));
	}
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
	if (auto* i = mpark::get_if<int>(&value)) {
		return *i;
	} else if (auto* f = mpark::get_if<float>(&value)) {
		// horror necessary for rounding without C99
		int x = *f;
		float diff = *f - x;
		if (*f >= 0.0f) {
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
	} else if (auto* fv = mpark::get_if<FaceValue>(&value)) {
		return fv->pose;
	} else if (auto* v = mpark::get_if<Vector<float>>(&value)) {
		return v->getMagnitude();
	} else {
		throw wrongCaosValueTypeException(fmt::format("Wrong caosValue type: Expected integer, got {}", dump()));
	}
}

float caosValue::getFloat() const {
	if (auto* i = mpark::get_if<int>(&value)) {
		return *i;
	} else if (auto* f = mpark::get_if<float>(&value)) {
		return *f;
	} else if (auto* v = mpark::get_if<Vector<float>>(&value)) {
		return v->getMagnitude();
	} else if (auto* fv = mpark::get_if<FaceValue>(&value)) {
		return fv->pose;
	} else {
		throw wrongCaosValueTypeException(fmt::format("Wrong caosValue type: Expected integer, got {}", dump()));
	}
}

void caosValue::getString(std::string& s) const {
	s = getString();
}

const std::string& caosValue::getString() const {
	if (auto* s = mpark::get_if<std::string>(&value)) {
		return *s;
	} else if (auto* fv = mpark::get_if<FaceValue>(&value)) {
		return fv->sprite_filename;
	} else {
		throw wrongCaosValueTypeException(fmt::format("Wrong caosValue type: Expected string, got {}", dump()));
	}
}

std::shared_ptr<Agent> caosValue::getAgent() const {
	return getAgentRef().lock();
}

static AgentRef nullagentref;

const AgentRef& caosValue::getAgentRef() const {
	if (auto* a = mpark::get_if<AgentRef>(&value)) {
		return *a;
	} else if (auto* i = mpark::get_if<int>(&value)) {
		// TODO: muh
		if (engine.version == 2) {
			if (i == 0) {
				return nullagentref;
			}

			// TODO: unid magic?
		}
	}
	throw wrongCaosValueTypeException(fmt::format("Wrong caosValue type: Expected agent, got {}", dump()));
}

const bytestring_t& caosValue::getByteStr() const {
	if (auto* bs = mpark::get_if<bytestring_t>(&value)) {
		return *bs;
	} else {
		throw wrongCaosValueTypeException(fmt::format("Wrong caosValue type: Expected bytestring, got {}", dump()));
	}
}

const Vector<float>& caosValue::getVector() const {
	if (auto* v = mpark::get_if<Vector<float>>(&value)) {
		return *v;
	} else {
		throw wrongCaosValueTypeException(fmt::format("Wrong caosValue type: Expected vector, got {}", dump()));
	}
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

/* vim: set noet: */
