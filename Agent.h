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
#include "caosVar.h"
#include "AgentRef.h"
#include <set>
#include <list>
#include <boost/shared_ptr.hpp>

class script;
using boost::shared_ptr;

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

	int unid;
	unsigned int zorder;
	unsigned int tickssincelasttimer, timerrate;

	int emitca_index; float emitca_amount;
	int lastcollidedirection;

	std::multiset<Agent *, agentzorder>::iterator zorder_iter;
	std::list<Agent *>::iterator agents_iter;
	std::list<caosVM *> vmstack; // for CALL etc
	std::vector<AgentRef> floated;

	struct SoundSlot *soundslot;
	void positionAudio(SoundSlot *);
	bool dying : 1;
	
	void vmTick();
	bool fireScript(unsigned short event, Agent *from = 0);

public:
	inline bool isDying() const {
		return dying;
	}
	
	// attr
	bool carryable : 1;
	bool mouseable : 1;
	bool activateable : 1;
	bool invisible : 1;
	bool floatable : 1;
	bool suffercollisions : 1;
	bool sufferphysics : 1;
	bool camerashy : 1;
	bool rotatable :1 ;
	bool presence : 1;
	// bhvr
	bool cr_can_push : 1;
	bool cr_can_pull : 1;
	bool cr_can_stop : 1;
	bool cr_can_hit : 1;
	bool cr_can_eat : 1;
	bool cr_can_pickup : 1;
	// imsk
	bool imsk_key_down : 1;
	bool imsk_key_up : 1;
	bool imsk_mouse_move : 1;
	bool imsk_mouse_down : 1;
	bool imsk_mouse_up : 1;
	bool imsk_mouse_wheel : 1;
	bool imsk_translated_char : 1;
	
	bool visible : 1;
	bool displaycore : 1;

	int clac[3]; int clik;
	unsigned char family, genus;
	unsigned short species;

	// motion
	caosVar velx, vely;
	float accg, aero;
	unsigned int friction;
	int perm, elas;
	float x, y;
	bool falling : 1; // TODO: icky hack, possibly

	float range;

	AgentRef floatingagent;

	void floatSetup();
	void floatRelease();
	void addFloated(AgentRef);
	void delFloated(AgentRef);
	void floatTo(AgentRef);
	void floatTo(float x, float y);

	bool queueScript(unsigned short event, AgentRef from = AgentRef(), caosVar p0 = caosVar(), caosVar p1 = caosVar());
	void moveTo(float, float);
	void setTimerRate(unsigned int r) { tickssincelasttimer = 0; timerrate = r; }
	void pushVM(caosVM *newvm);
	virtual void handleClick(float, float);
	
	Agent(unsigned char f, unsigned char g, unsigned short s, unsigned int p);
	virtual ~Agent();

	virtual unsigned int getWidth() = 0;
	virtual unsigned int getHeight() = 0;
	// TODO: following two functions are horrible hacks, to workaround the issue that compound parts are
	// often not inside the width/height of the parent agent
	virtual unsigned int getCheckWidth() { return getWidth(); }
	virtual unsigned int getCheckHeight() { return getHeight(); }

	virtual void tick();
	void physicsTick();
	virtual void kill();
	void stopScript();

	virtual void setZOrder(unsigned int plane);
	virtual unsigned int getZOrder() const { return zorder; }

	class shared_ptr<script> findScript(unsigned short event);
	
	int getUNID();
	std::string identify() const;

	virtual void setAttributes(unsigned int attr);
	virtual unsigned int getAttributes();
};

#endif
/* vim: set noet: */
