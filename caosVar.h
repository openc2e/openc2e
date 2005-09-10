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

#include "openc2e.h"
#include <string>
#include <cassert>
#include "AgentRef.h"

class Agent;

class caosVar {
	protected:
		enum variableType {
			NULLTYPE = 0, AGENT, INTEGER, FLOAT, STRING
		};


		union {
			int intValue;
			float floatValue;
			AgentRef *refValue;
			std::string *stringValue;
		} values;

		variableType type;
	public:
		void reset() {
			switch (type) {
				case AGENT:
					delete values.refValue;
					break;
				case STRING:
					delete values.stringValue;
					break;
				default: break;
			}
			type = NULLTYPE;
		}

		bool isNull() {
			return type == NULLTYPE;
		}

		caosVar() {
			type = INTEGER;
			values.intValue = 0;
		}

		~caosVar() {
			switch (type) {
				case STRING:
					delete values.stringValue;
					break;
				case AGENT:
					delete values.refValue;
					break;
				default: break;
			}
		}

		caosVar &operator=(const caosVar &copyFrom) {
			type = copyFrom.type;
			switch (type) {
				case INTEGER:
				case FLOAT:
					values = copyFrom.values;
					break;
				case STRING:
					values.stringValue = new std::string(*copyFrom.values.stringValue);
					break;
				case AGENT:
					values.refValue = new AgentRef(*copyFrom.values.refValue);
					break;
				case NULLTYPE:
					break;
				default: assert(0);
			}
			return *this;
		}

		caosVar(const caosVar &copyFrom) : type(copyFrom.type) {
			type = NULLTYPE;
			*this = copyFrom;
		}

		bool isEmpty() const { return type == NULLTYPE; }
		bool hasInt() const { return type == INTEGER; }
		bool hasFloat() const { return type == FLOAT; }
		bool hasAgent() const { return type == AGENT; }
		bool hasString() const { return type == STRING; }
		bool hasDecimal() const { return type == INTEGER || type == FLOAT; }
		bool hasNumber() const { return hasDecimal(); }
		
		void setInt(int i) { reset(); type = INTEGER; values.intValue = i; }
		void setFloat(float i) { reset(); type = FLOAT; values.floatValue = i; }
		void setAgent(Agent *i) {
			if (type == AGENT)
				values.refValue->set(i);
			else {
				reset();
				type = AGENT;
				values.refValue = new AgentRef(i);
			}
		}
		void setAgent(const AgentRef &r) {
			setAgent(r.get());
		}
		void setString(const std::string &i) {
			if (type == STRING)
				*values.stringValue = i;
			else {
				reset();
				type = STRING;
				values.stringValue = new std::string(i);
			}
		}

		int getInt() const {
			caos_assert(hasDecimal());
			if (type == INTEGER) {
				return values.intValue;
			} else {
				return (int)values.floatValue;
			}
		}

		float getFloat() const {
			caos_assert(hasDecimal());
			if (type == FLOAT) {
				return values.floatValue;
			} else {
				return (float)values.intValue;
			}
		}

		void getString(std::string &s) const {
			caos_assert(hasString());
			s = *values.stringValue;
		}

		std::string getString() const {
			caos_assert(hasString());
			return *values.stringValue;
		}

		Agent *getAgent() const {
			caos_assert(hasAgent());
			return values.refValue->get();
		}

		const AgentRef &getAgentRef() const {
			caos_assert(hasAgent());
			return *values.refValue;
		}

		bool operator == (const caosVar &v) const;
		bool operator != (const caosVar &v) const { return !(*this == v); }
		bool operator > (const caosVar &v) const;
		bool operator < (const caosVar &v) const;

		std::string dump() const;
};

// Compatibility hacks
// All of these are deprecated, of course

#ifndef IN_CAOSVAR_CPP
#define floatValue getFloat()
#define intValue getInt()

// Grep for this later and replace with reference-passing version
#define stringValue getString()
#define agentValue getAgent()

#endif

#endif
/* vim: set noet: */
