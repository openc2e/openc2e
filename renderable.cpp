/*
 *  renderable.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Thu Dec 29 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
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

#include "renderable.h"
#include "World.h"

//#include "CompoundPart.h"
bool renderablezorder::operator ()(const renderable *s1, const renderable *s2) const {
	/*if (s1->getZOrder() == s2->getZOrder()) {
		const CompoundPart *p1 = dynamic_cast<const CompoundPart *>(s1);
		const CompoundPart *p2 = dynamic_cast<const CompoundPart *>(s2);
		if (p1 && p2)
			return p1->id < p2->id;
	}*/
	
	return s1->getZOrder() < s2->getZOrder();
}

void renderable::zapZOrder() {
	world.renders.erase(renders_iter);
}

void renderable::addZOrder() {
	renders_iter = world.renders.insert(this);
}

renderable::~renderable() {
	world.renders.erase(renders_iter);
}

/* vim: set noet: */
