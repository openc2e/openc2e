/*
 *  CompoundAgent.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#include "CompoundAgent.h"
#include "openc2e.h"
#include "c16Image.h"
#include "World.h"
#include "Engine.h" // version
#include <algorithm> // sort
#include <functional> // binary_function
#include "caosVM.h" // calculateScriptId

// the list of parts is a list of pointers to CompoundPart, so we need a custom sort
struct less_part : public std::binary_function<CompoundPart *, CompoundPart *, bool> {
	bool operator()(CompoundPart *x, CompoundPart *y) { return *x < *y; }
};

void CompoundAgent::addPart(CompoundPart *p) {
	assert(p);
	assert(!part(p->id)); // todo: handle better

	// todo: we should prbly insert at the right place, not call sort
	parts.push_back(p);
	std::sort(parts.begin(), parts.end(), less_part());
}

void CompoundAgent::delPart(unsigned int id) {
	caos_assert(id != 0);

	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		if ((*x)->id == id) { delete *x; parts.erase(x); return; }
	}
	
	throw caosException("delPart got a bad id"); // TODO: handle this exception properly
}

CompoundPart *CompoundAgent::part(unsigned int id) {
	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		if ((*x)->id == id) return *x;
	}
	return 0;
}

CompoundAgent::CompoundAgent(unsigned char _family, unsigned char _genus, unsigned short _species, unsigned int plane,
				std::string spritefile, unsigned int firstimage, unsigned int imagecount) :
				Agent(_family, _genus, _species, plane) {
	// TODO: we ignore image count acos it sucks
	CompoundPart *p = new DullPart(this, 0, spritefile, firstimage, 0, 0, 0);
	caos_assert(p);
	addPart(p);
	
	for (unsigned int i = 0; i < 6; i++) {
		hotspots[i].left = -1; hotspots[i].right = -1; hotspots[i].top = -1;
		hotspots[i].bottom = -1; hotspots[i].function = -1;
	}
}

CompoundAgent::~CompoundAgent() {
	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		delete *x;
	}
}

void CompoundAgent::setZOrder(unsigned int plane) {
	Agent::setZOrder(plane);
	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) (*x)->zapZOrder();
	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) (*x)->addZOrder();
}

void CompoundAgent::tick() {
	Agent::tick();
	if (paused) return;

	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		(*x)->tick();
	}
}

void CompoundAgent::handleClick(float clickx, float clicky) {
	if (engine.version > 2) {
		Agent::handleClick(clickx, clicky);
		return;
	}

	// the hotspots are relative to us
	clickx -= x; clicky -= y;

	for (unsigned int i = 0; i < 6; i++) {
		if (hotspots[i].function == -1) continue;
		if (hotspots[i].left == -1) continue;
		// TODO: check other items for being -1?
	
		if (clickx >= hotspots[i].left && clickx <= hotspots[i].right)
			if (clicky >= hotspots[i].top && clicky <= hotspots[i].bottom) {
				// TODO: this isn't right for C2, at least
				queueScript(calculateScriptId(hotspots[i].function), (Agent *)world.hand());
				return;
			}
	}
}

void CompoundAgent::setHotspotLoc(unsigned int id, int l, int t, int r, int b) {
	assert(id < 6);

	hotspots[id].left = l;
	hotspots[id].top = t;
	hotspots[id].right = r;
	hotspots[id].bottom = b;
}

void CompoundAgent::setHotspotFunc(unsigned int id, unsigned int f) {
	assert(id < 6);

	hotspots[id].function = f;
}

/* vim: set noet: */
