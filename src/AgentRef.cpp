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
	assert(ref.expired() || !(ref.lock()->isDying()));
}

void AgentRef::set(Agent *a) {
	if (a && !a->isDying())
		set(a->shared_from_this());
	else
		ref.reset();
}

void AgentRef::dump() const {
	std::cerr << "AgentRef " << (void *)this << " pointing to " << (void *)ref.lock().get() << std::endl;
}

std::shared_ptr<Agent> AgentRef::lock() const {
	std::shared_ptr<Agent> p = ref.lock();
	if (p && p->isDying())
		p.reset();
	return p;
}

/* vim: set noet: */
