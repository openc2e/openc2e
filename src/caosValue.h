/*
 *  caosValue.h
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

#pragma once

#include <mpark/variant.hpp>
#include <string>
#include "AgentRef.h"
#include "alloc_count.h"
#include "bytestring.h"
#include "caosException.h"
#include "physics.h"
#include "serfwd.h"

class Agent;

class wrongCaosValueTypeException : public caosException {
	public:
		using caosException::caosException;
};

struct nulltype_tag { };

enum variableType {
	CAOSNULL = 0, CAOSAGENT, CAOSINT, CAOSFLOAT, CAOSSTR, CAOSBYTESTRING, CAOSVEC
};

const char* variableTypeToString(variableType);

class caosValue {
	private:
		COUNT_ALLOC(caosValue)
		FRIEND_SERIALIZE(caosValue)
	protected:
		mpark::variant<int, float, AgentRef, std::string, bytestring_t, nulltype_tag, Vector<float>> value;

	public:
		caosValue();
		caosValue(int v);
		caosValue(float v);
		caosValue(Agent *v);
		caosValue(const AgentRef &v);
		caosValue(const std::string &v);
		caosValue(const bytestring_t &v);
		caosValue(const Vector<float> &v);
		caosValue(const caosValue&);
		caosValue &operator=(const caosValue&);
		~caosValue();
		
		void reset();
		
		variableType getType() const;
		bool isNull() const;
		bool isEmpty() const;
		bool hasInt() const;
		bool hasFloat() const;
		bool hasAgent() const;
		bool hasString() const;
		bool hasDecimal() const;
		bool hasNumber() const;
		bool hasByteStr() const;
		bool hasVector() const;
		
		void setInt(int i);
		void setFloat(float i);
		void setAgent(Agent *i);
		void setAgent(const AgentRef &r);
		void setString(const std::string &i);
		void setByteStr(const bytestring_t &bs);
		void setVector(const Vector<float> &v);
		int getInt() const;
		float getFloat() const;
		void getString(std::string &s) const;
		const std::string &getString() const;
		std::shared_ptr<Agent> getAgent() const;
		const AgentRef &getAgentRef() const;
		const bytestring_t &getByteStr() const;
		const Vector<float> &getVector() const;

		bool operator == (const caosValue &v) const;
		bool operator != (const caosValue &v) const { return !(*this == v); }
		bool operator > (const caosValue &v) const;
		bool operator < (const caosValue &v) const;

		std::string dump() const;
};

/* vim: set noet: */
