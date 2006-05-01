/*
 *  caosVar.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Thu Mar 10 2005.
 *  Copyright (c) 2004 Alyssa Milburn and Bryan Donlan. All rights reserved.
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

#ifndef CAOSVAR_H
#define CAOSVAR_H 1

#include "boost/variant.hpp"
#include "openc2e.h"
#include <string>
#include <cassert>
#include "AgentRef.h"
#include "slaballoc.h"
#include <typeinfo>

#include "serialization.h"

class Agent;

class wrongCaosVarTypeException : public caosException {
	public:
		wrongCaosVarTypeException() throw() : caosException("Wrong caos variable type") {}
		wrongCaosVarTypeException(const char *s) throw() : caosException(s) { }
		wrongCaosVarTypeException(const std::string &s) throw() : caosException(s) { }
};

struct nulltype_tag { };

enum variableType {
	NULLTYPE = 0, AGENT, INTEGER, FLOAT, STRING
};

class caosVar {
	private:
		FRIEND_SERIALIZE(caosVar)
	protected:
		struct typeVisit : public boost::static_visitor<variableType> {
			variableType operator()(int) const { return INTEGER; }
			variableType operator()(float) const { return FLOAT; }
			variableType operator()(const std::string &) const { return STRING; }
			variableType operator()(const AgentRef &) const { return AGENT; }
			variableType operator()(nulltype_tag) const { return NULLTYPE; }
		};

#define BAD_TYPE(et, gt) \
		const et &operator()(const gt &) const { \
			throw wrongCaosVarTypeException( \
					"Wrong caosVar type: Expected " #et ", got " #gt \
					); \
		}
		
		struct intVisit : public boost::static_visitor<int> {
			int operator()(int i) const { return i; }
			int operator()(float f) const {
				// horror necessary for rounding without C99
				int x = (int)f; float diff = f - x;
				if (f >= 0.0f) {
					if (diff >= 0.5f) return ++x; else return x;
				} else {
					if (diff <= -0.5f) return --x; else return x;
				}
			}
			BAD_TYPE(int, std::string);
			BAD_TYPE(int, AgentRef);
			BAD_TYPE(int, nulltype_tag);
		};

		struct floatVisit : public boost::static_visitor<float> {
			float operator()(int i) const { return (float)i; }
			float operator()(float f) const { return f; }
			BAD_TYPE(float, std::string);
			BAD_TYPE(float, AgentRef);
			BAD_TYPE(float, nulltype_tag);
		};

		struct stringVisit : public boost::static_visitor<const std::string &> {
			const std::string &operator()(const std::string &s) const {
				return s;
			}
			BAD_TYPE(std::string, AgentRef);
			BAD_TYPE(std::string, nulltype_tag);
			BAD_TYPE(std::string, int);
			BAD_TYPE(std::string, float);
		};
		
		struct agentVisit : public boost::static_visitor<const AgentRef &> {
			const AgentRef &operator()(const AgentRef &a) const {
				return a;
			}
			BAD_TYPE(AgentRef, std::string);
			BAD_TYPE(AgentRef, nulltype_tag);
			BAD_TYPE(AgentRef, int);
			BAD_TYPE(AgentRef, float);
		};
			
#undef BAD_TYPE
		boost::variant<int, float, AgentRef, std::string, nulltype_tag> value;

	public:
		variableType getType() const {
			return boost::apply_visitor(typeVisit(), value);
		}
		
		void reset() {
			value = nulltype_tag();
		}

		bool isNull() {
			return getType() == NULLTYPE;
		}

		caosVar() {
		}

		~caosVar() {
		}

		caosVar &operator=(const caosVar &copyFrom) {
			value = copyFrom.value;
			return *this;
		}

		caosVar(const caosVar &copyFrom) : value(copyFrom.value) { }
		
		caosVar(int v) { setInt(v); }
		caosVar(float v) { setFloat(v); }
		caosVar(Agent *v) { setAgent(v); }
		caosVar(const AgentRef &v) { setAgent(v); }
		caosVar(std::string &v) { setString(v); } 
		
		bool isEmpty() const { return getType() == NULLTYPE; }
		bool hasInt() const { return getType() == INTEGER; }
		bool hasFloat() const { return getType() == FLOAT; }
		bool hasAgent() const { return getType() == AGENT; }
		bool hasString() const { return getType() == STRING; }
		bool hasDecimal() const { return getType() == INTEGER || getType() == FLOAT; }
		bool hasNumber() const { return hasDecimal(); }
		
		void setInt(int i) { value = i; }
		void setFloat(float i) { value = i; }
		void setAgent(Agent *i) {
			value = AgentRef(i);
		}
		void setAgent(const AgentRef &r) {
			value = r;
		}
		void setString(const std::string &i) {
			value = i;
		}

		int getInt() const {
			return boost::apply_visitor(intVisit(), value);
		}

		float getFloat() const {
			return boost::apply_visitor(floatVisit(), value);
		}

		void getString(std::string &s) const {
			s = getString();
		}

		const std::string &getString() const {
			return boost::apply_visitor(stringVisit(), value);
		}

		boost::shared_ptr<Agent> getAgent() const {
			return getAgentRef().lock();
		}

		const AgentRef &getAgentRef() const {
			return boost::apply_visitor(agentVisit(), value);
		}

		bool operator == (const caosVar &v) const;
		bool operator != (const caosVar &v) const { return !(*this == v); }
		bool operator > (const caosVar &v) const;
		bool operator < (const caosVar &v) const;

		std::string dump() const;
};

struct caosVarCompare {
	bool operator()(const caosVar &v1, const caosVar &v2) const {
		if (v1.getType() == v2.getType())
			return v1 < v2;
		else
			return v1.getType() < v2.getType();
	}
};

#endif
/* vim: set noet: */
