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
#include <memory>

class Agent;

class AgentRef {
	friend class Agent;
	
protected:
	std::weak_ptr<Agent> ref;
	void checkLife() const;

public:
	void dump() const;
	
	AgentRef() { }
	AgentRef(std::shared_ptr<Agent> a) { ref = a; }
	AgentRef(std::weak_ptr<Agent> a) { ref = a; }
	AgentRef(Agent *a) { set(a); }
	AgentRef(const AgentRef &r) : ref(r.ref) {}

	void clear() { ref.reset(); }
	
	~AgentRef() { clear(); };

	AgentRef &operator=(const AgentRef &r) { ref = r.ref; return *this; }
	Agent *operator=(Agent *a) { set(a); return a; }
	Agent &operator*() const { checkLife(); return *ref.lock().get(); }
	Agent *operator->() const { checkLife(); return ref.lock().get(); }
	bool operator!() const { return lock().get() == NULL; }
	/* This next line breaks builds with MSVC, tossing errors about ambiguous operators.
	operator bool() const { return ref; } */
	operator Agent *() const { return ref.lock().get(); }
	bool operator==(const AgentRef &r) const { return lock() == r.lock(); }
	bool operator==(const Agent *r) const { return r == lock().get(); }
	bool operator!=(const AgentRef &r) const { return !(*this == r);}
	bool operator!=(const Agent *r) const { return !(*this == r); }

	void set(Agent *a);
	void set(const AgentRef &r) { ref = r.ref; }
	void set(const std::shared_ptr<Agent> &r) { ref = r; }
	void set(const std::weak_ptr<Agent> &r) { ref = r; }

	std::shared_ptr<Agent> lock() const;
	Agent *get() const { return lock().get(); }
};
		

#endif

/* vim: set noet: */
