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
#include <boost/enable_shared_from_this.hpp>
#include "openc2e.h"
#include "physics.h"

class script;

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
	friend class LifeAssert;
	friend class SFCFile;
	friend class SFCSimpleObject;
	friend class SFCCompoundObject;

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

	void zotstack();

	mutable int unid;
	unsigned int zorder;
	unsigned int tickssincelasttimer, timerrate;

	int emitca_index; float emitca_amount;
	int lastcollidedirection;

	std::multiset<Agent *, agentzorder>::iterator zorder_iter;
	std::list<boost::shared_ptr<Agent> >::iterator agents_iter;
	std::list<caosVM *> vmstack; // for CALL etc
	std::vector<AgentRef> floated;

	void updateAudio(boost::shared_ptr<class AudioSource>);
	bool dying : 1;
	
	void unhandledException(std::string info, bool wasscript);
	void vmTick();
	virtual bool fireScript(unsigned short event, Agent *from, caosVar one, caosVar two);

	std::map<unsigned int, std::pair<int, int> > carry_points, carried_points;

	virtual void carry(AgentRef);
	virtual void drop(AgentRef);
	virtual void adjustCarried(float xoffset, float yoffset);

public:
	boost::shared_ptr<class AudioSource> sound;

	AgentRef carrying;
	AgentRef carriedby;
	AgentRef invehicle;
	
	inline bool isDying() const {
		return dying;
	}
	
	// attr
	caosVar attr;
	// values which are always the same
	bool carryable() { return attr.getInt() & 1; }
	bool mouseable() { return attr.getInt() & 2; }
	bool activateable() { return attr.getInt() & 4; }
	bool greedycabin() { return attr.getInt() & 8; }
	bool invisible() { return attr.getInt() & 16; }
	bool floatable() { return attr.getInt() & 32; }
	// version-specific values
	// C1
	bool groundbound() { return attr.getInt() & 64; }
	bool roombound() { return attr.getInt() & 128; }
	// C2 and c2e
	bool suffercollisions() { return attr.getInt() & 64; }
	bool sufferphysics() { return attr.getInt() & 128; }
	// c2e
	bool camerashy() { return attr.getInt() & 256; }
	bool openaircabin() { return attr.getInt() & 512; }
	bool rotatable() { return attr.getInt() & 1024; }
	bool presence() { return attr.getInt() & 2048; }

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
	caosVar accg, aero;
	unsigned int friction;
	int perm, elas;
	caosVar rest;
	float x, y;
	bool falling : 1; // TODO: icky hack, possibly

	caosVar range;

	AgentRef floatingagent;

	// Creatures 1/2 bits
	caosVar objp, babymoniker;

	// Creatures 2
	// TODO: size/grav likely duplicates of perm/falling
	caosVar actv, thrt, size, grav;

	void floatSetup();
	void floatRelease();
	void addFloated(AgentRef);
	void delFloated(AgentRef);
	void floatTo(AgentRef);
	void floatTo(float x, float y);

	bool beDropped();
	
	void addCarried(AgentRef);
	void dropCarried(AgentRef);

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
	Point boundingBoxPoint(unsigned int n);
	static Point boundingBoxPoint(unsigned int n, Point p, unsigned int w, unsigned int h);
	
	virtual void tick();
	virtual void physicsTick();
	virtual void kill();
	void stopScript();

	bool validInRoomSystem();
	static bool validInRoomSystem(Point p, unsigned int w, unsigned int h, int testperm);

	virtual void setZOrder(unsigned int plane); // should be overridden!
	virtual unsigned int getZOrder() const;

	class shared_ptr<script> findScript(unsigned short event);
	
	int getUNID() const;
	std::string identify() const;

	void setAttributes(unsigned int a) { attr.setInt(a); }
	unsigned int getAttributes() const { return attr.getInt(); }

	void playAudio(std::string filename, bool controlled, bool loop);
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
