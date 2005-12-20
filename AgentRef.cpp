/*
 *  AgentRef.cpp
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

#include "AgentRef.h"
#include "Agent.h"
#include <cassert>
#include <iostream>

void AgentRef::checkLife() const {
	assert(!ref || !ref->isDying());
}

void AgentRef::clear() {
	if (!ref) return;
	next->prev = prev;
	prev->next = next;
	ref = NULL;
	next = prev = NULL;
}

void AgentRef::set(Agent *a) {
	clear();
	if (!a || a->isDying()) return;

	// Verify consistency of the linked list
	assert(a->self.ref == a);
	assert(a->self.next->prev = &a->self);
	assert(a->self.prev->next = &a->self);

	// We insert ourselves at a->self.next
	next = a->self.next;
	prev = &a->self;

	next->prev = this;
	prev->next = this;

	ref = a;
}

void AgentRef::set(const AgentRef &r) {
	/* Since r is const, we can't touch it directly.
	 * Pass its ref to set(Agent*)
	 */
	set(r.ref);
}

void AgentRef::dump() const {
	std::cerr << "AgentRef " << (void *)this << " pointing to " << (void *)ref << ", next=" << (void *)next <<
		" prev=" << (void *)prev << std::endl;
}
/* vim: set noet: */
