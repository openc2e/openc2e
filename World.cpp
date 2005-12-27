/*
 *  World.cpp
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

#include "World.h"
#include "caosVM.h" // for setupCommandPointers()
#include "PointerAgent.h"
#include "CompoundAgent.h" // for setFocus
#include <limits.h> // for MAXINT
#include "creaturesImage.h"

World world;

World::World() {
	ticktime = 50;
	tickcount = 0;
	quitting = saving = false;
}

// annoyingly, if we put this in the constructor, imageGallery isn't available yet
void World::init() {
	if (gallery.getImage("hand")) // TODO: we refcount one too many here, i expect // TODO: we refcount one too many here, i expect
		theHand = new PointerAgent("hand");
	else	
		theHand = new PointerAgent("syst"); // Creatures Village
}

caosVM *World::getVM(Agent *a) {
	if (vmpool.empty()) {
		return new caosVM(a);
	} else {
		caosVM *x = vmpool.back();
		vmpool.pop_back();
		x->setOwner(a);
		return x;
	}
}

void World::freeVM(caosVM *v) {
	v->setOwner(0);
	vmpool.push_back(v);
}

void World::queueScript(unsigned short event, AgentRef agent, AgentRef from, caosVar p0, caosVar p1) {
	scriptevent e;

	assert(agent);

	e.scriptno = event;
	e.agent = agent;
	e.from = from;
	e.p[0] = p0;
	e.p[1] = p1;

	scriptqueue.push_back(e);
}

void World::setFocus(CompoundAgent *a, TextEntryPart *p) {
	// Unfocus the current agent. Not sure if c2e always does this (what if the agent/part is bad?).
        if (focusagent) {
		CompoundAgent *c = dynamic_cast<CompoundAgent *>(focusagent.get());
		assert(c);
		TextEntryPart *p = dynamic_cast<TextEntryPart *>(c->part(focuspart));
		if (p)
			p->loseFocus();
	}

	if (!p)
		focusagent.clear();
	else {
		assert(p == a->part(p->id));
		p->gainFocus();
		focusagent = a;
		focuspart = p->id;
	}
}

void World::tick() {
	if (saving) {} // TODO: save
	if (quitting) { exit(0); }
	// Tick all agents.
	for (std::list<Agent *>::iterator i = agents.begin(); i != agents.end(); i++) {
		(**i).tick();
	}
	
	// Process the script queue.
	for (std::vector<scriptevent>::iterator i = scriptqueue.begin(); i != scriptqueue.end(); i++) {
		if (i->agent && i->agent->fireScript(i->scriptno, i->from)) {
			assert(i->agent->vm);
			i->agent->vm->setVariables(i->p[0], i->p[1]);
			i->agent->vmTick();
		}
	}
	scriptqueue.clear();
	
	// Do the actual killing of agent objects.
	// TODO: should we do this before+after the script queue processing? - fuzzie
	while (killqueue.size()) {
		Agent *rip = killqueue.back();
		killqueue.pop_back();
		assert(rip->dying);
		delete rip;
	}


	tickcount++;
	// todo: tick rooms
}

Agent *World::agentAt(unsigned int x, unsigned int y, bool needs_activateable) {
	Agent *temp = 0;

	// we're looking for the *last* agent in the set which is at this location (ie, topmost)
	// TODO: this needs to check if agents are USEFUL (ie, not background scenery etc)
	// TODO: we might well need to do more checking on compound agents
	for (std::multiset<Agent *, agentzorder>::iterator i = zorder.begin(); i != zorder.end(); i++) {
		if ((*i)->x <= x) if ((*i)->y <= y) if (((*i) -> x + (*i)->getCheckWidth()) >= x) if (((*i) -> y + (*i)->getCheckHeight()) >= y)
			if ((*i) != theHand)
				if ((!needs_activateable) || (*i)->activateable)
					temp = *i;
	}
	
	return temp;
}

int World::getUNID(Agent *whofor) {
	do {
		int unid = rand();
		if (!unidmap[unid]) {
			unidmap[unid] = whofor;
			return unid;
		}
	} while (1);
}

void World::freeUNID(int unid) {
	unidmap.erase(unid);
}

Agent *World::lookupUNID(int unid) {
	return unidmap[unid];
}

/* vim: set noet: */
