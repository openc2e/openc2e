/*
 *  AgentRef.h
 *  openc2e
 *
 *  Created by Bryan Donlan on The Apr 11 2005
 *  Copyright (c) 2005 Bryan Donlan. All rights reserved.
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

#ifndef AGENTREF_H
#define AGENTREF_H 1

#include <cstdlib> // for NULL
#include <iostream>
class Agent;

class AgentRef {
	friend class Agent;
	
protected:
	AgentRef *next, *prev;
	Agent *ref;
	void checkLife() const;

public:
	void dump() const;
	
	AgentRef() { ref = NULL; next = NULL; prev = NULL; }
	AgentRef(Agent *a) { ref = NULL; next = NULL; prev = NULL; set(a);
		}
	AgentRef(const AgentRef &r) {
		ref = NULL; next = NULL; prev = NULL; set(r);
	}

	void clear();
	
	virtual ~AgentRef() { clear(); };

	AgentRef &operator=(const AgentRef &r) { set(r); return *this; }
	Agent *operator=(Agent *a) { set(a); return a; }
	Agent &operator*() const { return *ref; }
	Agent *operator->() const { return ref; }
	bool operator!() const { return !ref; }
	/* This next line breaks builds with MSVC, tossing errors about ambiguous operators.
	operator bool() const { return ref; } */
	operator Agent *() const { return ref; }
	bool operator==(const AgentRef &r) const { return r.ref == ref; }
	bool operator==(const Agent *r) const { return r == ref; }
	bool operator!=(const AgentRef &r) const { return !(*this == r);}
	bool operator!=(const Agent *r) const { return !(*this == r); }

	void set(Agent *a);
	void set(const AgentRef &r);

	Agent *get() const { checkLife(); return ref; }
};
		

#endif

/* vim: set noet: */
