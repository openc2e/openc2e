/*
 *  Agent.h
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

#ifndef __AGENT_H
#define __AGENT_H

#include "creaturesImage.h"
#include "SDLBackend.h"
#include "caosVar.h"
#include "AgentRef.h"
#include <set>
#include <list>

struct agentzorder {
	bool operator()(const class Agent *s1, const class Agent *s2) const;
};

class Agent {
	friend struct agentzorder;
	friend class caosVM;
	friend class AgentRef;
	friend class World;
	friend class opOVxx;
	friend class opMVxx;
	friend class SDLBackend; // TODO: should we make soundslot public instead?

protected:
	caosVar var[100]; // OVxx
	std::map<caosVar, caosVar> name_variables;
	AgentRef self;
	class caosVM *vm;

	void zotrefs();
	void zotstack();

	bool dying;
	int unid;
	unsigned int zorder;
	unsigned int tickssincelasttimer, timerrate;

	bool displaycore;

	int lastcollidedirection;

	std::multiset<Agent *, agentzorder>::iterator zorder_iter;
	std::list<caosVM *> vmstack; // for CALL etc

	struct SoundSlot *soundslot;
	void positionAudio(SoundSlot *);

public:
	inline bool isDying() const {
		return dying;
	}
	
	int clac[3]; int clik;
	// attr
	bool carryable, mouseable, activateable, invisible, floatable;
	bool suffercollisions, sufferphysics, camerashy, rotatable, presence;
	// bhvr
	bool cr_can_push, cr_can_pull, cr_can_stop;
	bool cr_can_hit, cr_can_eat, cr_can_pickup;
	
	bool visible;

	unsigned char family, genus;
	unsigned short species;

	// motion
	caosVar velx, vely;
	float accg, aero;
	unsigned int friction;
	int perm, elas;
	float x, y;

	float range;

	float floatingx, floatingy;
	AgentRef floatingagent;

	void fireScript(unsigned short event, Agent *from = 0);
	void moveTo(float, float);
	void setTimerRate(unsigned int r) { tickssincelasttimer = 0; timerrate = r; }
	void pushVM(caosVM *newvm);
	virtual void handleClick(float, float);
	
	Agent(unsigned char f, unsigned char g, unsigned short s, unsigned int p);
	virtual ~Agent();

	virtual void setAttributes(unsigned int attr) = 0;
	virtual unsigned int getAttributes() = 0;
	virtual unsigned int getWidth() = 0;
	virtual unsigned int getHeight() = 0;

	virtual void tick();
	virtual void render(SDLBackend *renderer, int xoffset, int yoffset) = 0;
	virtual void kill();
	void stopScript();

	virtual void setZOrder(unsigned int plane);
	virtual unsigned int getZOrder() const { return zorder; }

	class script *findScript(unsigned short event);
	
	int getUNID();
	std::string identify() const;
};

#endif
/* vim: set noet: */
