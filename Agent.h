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
#include "CompoundPart.h"
#include <set>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "openc2e.h"

class script;
using boost::shared_ptr;

struct agentzorder {
	bool operator()(const class Agent *s1, const class Agent *s2) const;
};

class Agent : public boost::enable_shared_from_this<Agent> {
	
	friend struct agentzorder;
	friend class caosVM;
	friend class AgentRef;
	friend class World;
	friend class opOVxx;
	friend class opMVxx;
	friend class SDLBackend; // TODO: should we make soundslot public instead?
	friend class LifeAssert;

	FRIEND_SERIALIZE(Agent);
	
protected:
	bool initialized;
	virtual void finishInit();
	int lifecount;
private:
	void core_init();
	Agent() { core_init(); } // for boost only
protected:
	int lastScript;

	caosVar var[100]; // OVxx
	std::map<caosVar, caosVar, caosVarCompare> name_variables;
	std::map<unsigned int, shared_ptr<class genomeFile> > slots;
	class caosVM *vm;

	void zotrefs();
	void zotstack();

	int unid;
	unsigned int zorder;
	unsigned int tickssincelasttimer, timerrate;

	int emitca_index; float emitca_amount;
	int lastcollidedirection;

	std::multiset<Agent *, agentzorder>::iterator zorder_iter;
	std::list<boost::shared_ptr<Agent> >::iterator agents_iter;
	std::list<caosVM *> vmstack; // for CALL etc
	std::vector<AgentRef> floated;

	struct SoundSlot *soundslot;
	void positionAudio(SoundSlot *);
	bool dying : 1;
	
	void vmTick();
	bool fireScript(unsigned short event, Agent *from = 0);

	std::map<unsigned int, std::pair<int, int> > carry_points, carried_points;

public:
	AgentRef carrying;
	AgentRef carriedby;
	
	inline bool isDying() const {
		return dying;
	}
	
	// attr
	bool carryable : 1;
	bool mouseable : 1;
	bool activateable : 1;
	bool greedycabin : 1;
	bool invisible : 1;
	bool floatable : 1;
	bool suffercollisions : 1;
	bool sufferphysics : 1;
	bool camerashy : 1;
	bool openaircabin : 1;
	bool rotatable : 1;
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
	
	bool paused : 1;
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

	void dropCarried();
	void carry(AgentRef);
	void adjustCarried();

	bool queueScript(unsigned short event, AgentRef from = AgentRef(), caosVar p0 = caosVar(), caosVar p1 = caosVar());
	void moveTo(float, float, bool force = false);
	void setTimerRate(unsigned int r) { tickssincelasttimer = 0; timerrate = r; }
	void pushVM(caosVM *newvm);
	virtual void handleClick(float, float);
	
	Agent(unsigned char f, unsigned char g, unsigned short s, unsigned int p);
	virtual ~Agent();

	virtual CompoundPart *part(unsigned int id) = 0;
	
	unsigned int getWidth() { return part(0)->getWidth(); }
	unsigned int getHeight() { return part(0)->getHeight(); }
	
	virtual void tick();
	virtual void physicsTick();
	virtual void kill();
	void stopScript();

	virtual void setZOrder(unsigned int plane); // should be overridden!
	virtual unsigned int getZOrder() const;

	class shared_ptr<script> findScript(unsigned short event);
	
	int getUNID();
	std::string identify() const;

	virtual void setAttributes(unsigned int attr);
	virtual unsigned int getAttributes();
};

class LifeAssert {
	protected:
		Agent *p;
	public:
		LifeAssert(const AgentRef &ref) {
			p = ref.get();
			assert(p);
			p->lifecount++;
		}
		LifeAssert(const boost::weak_ptr<Agent> &p_) {
			p = p_.lock().get();
			assert(p);
			p->lifecount++;
		}
		LifeAssert(const boost::shared_ptr<Agent> &p_) {
			p = p_.get();
			assert(p);
			p->lifecount++;
		}
		LifeAssert(Agent *p_) {
			p = p_;
			assert(p);
			p->lifecount++;
		}
		~LifeAssert() {
			p->lifecount--;
		}
};


#endif
/* vim: set noet: */
