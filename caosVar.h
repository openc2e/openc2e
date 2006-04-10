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

#include "serialization.h"

class Agent;

enum variableType {
	NULLTYPE = 0, AGENT, INTEGER, FLOAT, STRING
};
class caosVar {
	private:
		FRIEND_SERIALIZE(caosVar)
	public:
	protected:


		boost::variant< int, float, AgentRef, std::string> value;

		variableType type;
	public:
		variableType getType() const { return type; }
		
		void reset() {
			value = (int)0;
			type = NULLTYPE;
		}

		bool isNull() {
			return type == NULLTYPE;
		}

		caosVar() {
			type = INTEGER;
			value = 0;
		}

		~caosVar() {
		}

		caosVar &operator=(const caosVar &copyFrom) {
			type = copyFrom.type;
			value = copyFrom.value;
			return *this;
		}

		caosVar(const caosVar &copyFrom) : type(copyFrom.type) {
			*this = copyFrom;
		}

		bool isEmpty() const { return type == NULLTYPE; }
		bool hasInt() const { return type == INTEGER; }
		bool hasFloat() const { return type == FLOAT; }
		bool hasAgent() const { return type == AGENT; }
		bool hasString() const { return type == STRING; }
		bool hasDecimal() const { return type == INTEGER || type == FLOAT; }
		bool hasNumber() const { return hasDecimal(); }
		
		void setInt(int i) { type = INTEGER; value = i; }
		void setFloat(float i) { type = FLOAT; value = i; }
		void setAgent(Agent *i) {
			type = AGENT;
			value = AgentRef(i);
		}
		void setAgent(const AgentRef &r) {
			setAgent(r.get());
		}
		void setString(const std::string &i) {
			type = STRING;
			value = i;
		}

		int getInt() const {
			caos_assert(hasDecimal());
			if (type == INTEGER) {
				return boost::get<int>(value);
			} else {
				return (int)boost::get<float>(value);
			}
		}

		float getFloat() const {
			caos_assert(hasDecimal());
			if (type == FLOAT) {
				return boost::get<float>(value);
			} else {
				return (float)boost::get<int>(value);
			}
		}

		void getString(std::string &s) const {
			caos_assert(hasString());
			s = boost::get<std::string>(value);
		}

		std::string getString() const {
			caos_assert(hasString());
			return boost::get<std::string>(value);
		}

		Agent *getAgent() const {
			caos_assert(hasAgent());
			return boost::get<AgentRef>(value).get();
		}

		const AgentRef getAgentRef() const {
			caos_assert(hasAgent());
			return boost::get<AgentRef>(value).get();
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
