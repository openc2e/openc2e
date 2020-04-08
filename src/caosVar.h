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

#include <boost/variant.hpp>
#include "openc2e.h"
#include <string>
#include <cassert>
#include "AgentRef.h"
#include <typeinfo>
#include "physics.h"
#include "alloc_count.h"

class Agent;

class wrongCaosVarTypeException : public caosException {
	public:
		wrongCaosVarTypeException() throw() : caosException("Wrong caos variable type") {}
		wrongCaosVarTypeException(const char *s) throw() : caosException(s) { }
		wrongCaosVarTypeException(const std::string &s) throw() : caosException(s) { }
};

struct nulltype_tag { };

enum variableType {
	CAOSNULL = 0, CAOSAGENT, CAOSINT, CAOSFLOAT, CAOSSTR, CAOSVEC
};

class caosVar {
	private:
		COUNT_ALLOC(caosVar)
		FRIEND_SERIALIZE(caosVar)
	protected:
		struct typeVisit : public boost::static_visitor<variableType> {
			variableType operator()(int) const { return CAOSINT; }
			variableType operator()(float) const { return CAOSFLOAT; }
			variableType operator()(const std::string &) const { return CAOSSTR; }
			variableType operator()(const AgentRef &) const { return CAOSAGENT; }
			variableType operator()(nulltype_tag) const { return CAOSNULL; }
			variableType operator()(const Vector<float> &) const { return CAOSVEC; }
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
			int operator()(const Vector<float> &v) const {
				return (int)v.getMagnitude();
			}
			BAD_TYPE(int, std::string);
			BAD_TYPE(int, AgentRef);
			BAD_TYPE(int, nulltype_tag);
		};

		struct floatVisit : public boost::static_visitor<float> {
			float operator()(int i) const { return (float)i; }
			float operator()(float f) const { return f; }
			float operator()(const Vector<float> &v) const { return v.getMagnitude(); }
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
			BAD_TYPE(std::string, Vector<float>);
		};
		
		struct agentVisit : public boost::static_visitor<const AgentRef &> {
			const AgentRef &operator()(const AgentRef &a) const {
				return a;
			}
			BAD_TYPE(AgentRef, std::string);
			BAD_TYPE(AgentRef, nulltype_tag);
			const AgentRef &operator()(int i) const;
			BAD_TYPE(AgentRef, float);
			BAD_TYPE(AgentRef, Vector<float>);
		};

		struct vectorVisit : public boost::static_visitor<const Vector<float> &> {
			const Vector<float> &operator()(const Vector<float> &v) const {
				return v;
			}
			BAD_TYPE(Vector<float>, std::string);
			BAD_TYPE(Vector<float>, nulltype_tag);
			BAD_TYPE(Vector<float>, int);
			BAD_TYPE(Vector<float>, float);
			BAD_TYPE(Vector<float>, AgentRef);
		};

			
#undef BAD_TYPE
		boost::variant<int, float, AgentRef, std::string, nulltype_tag, Vector<float> > value;

	public:
		variableType getType() const {
			return boost::apply_visitor(typeVisit(), value);
		}
		
		void reset() {
			value = nulltype_tag();
		}

		bool isNull() {
			return getType() == CAOSNULL;
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
		caosVar(const std::string &v) { setString(v); } 
		caosVar(const Vector<float> &v) { setVector(v); }
		
		bool isEmpty() const { return getType() == CAOSNULL; }
		bool hasInt() const { return getType() == CAOSINT; }
		bool hasFloat() const { return getType() == CAOSFLOAT; }
		bool hasAgent() const { return getType() == CAOSAGENT; }
		bool hasString() const { return getType() == CAOSSTR; }
		bool hasDecimal() const { return getType() == CAOSINT || getType() == CAOSFLOAT || getType() == CAOSVEC; }
		bool hasNumber() const { return hasDecimal(); }
		bool hasVector() const { return getType() == CAOSVEC; }
		
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
		void setVector(const Vector<float> &v) {
			value = v;
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

		std::shared_ptr<Agent> getAgent() const {
			return getAgentRef().lock();
		}

		const AgentRef &getAgentRef() const {
			return boost::apply_visitor(agentVisit(), value);
		}

		const Vector<float> &getVector() const {
			return boost::apply_visitor(vectorVisit(), value);
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
