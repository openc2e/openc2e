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

#include "caosVar.h"
#include "AgentRef.h"
#include "CompoundPart.h"
#include "Port.h"
#include <set>
#include <list>
#include <map>
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
	friend class CreatureAgent;
	friend class QtOpenc2e; // i despise c++ - fuzzie

	FRIEND_SERIALIZE(Agent)
	
protected:
	bool initialized;
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

	bool wasmoved;

	int emitca_index; float emitca_amount;
	int lastcollidedirection;

	std::multiset<Agent *, agentzorder>::iterator zorder_iter;
	std::list<boost::shared_ptr<Agent> >::iterator agents_iter;
	std::list<caosVM *> vmstack; // for CALL etc
	std::vector<AgentRef> floated;

	void updateAudio(boost::shared_ptr<class AudioSource>);
	bool dying : 1;
	
	void vmTick();
	virtual bool fireScript(unsigned short event, Agent *from, caosVar one, caosVar two);

	virtual void physicsTick();
	void physicsTickC2();
	
	virtual void carry(AgentRef);
	virtual void drop(AgentRef);

	virtual std::pair<int, int> getCarryPoint();
	virtual std::pair<int, int> getCarriedPoint();
	virtual void adjustCarried(float xoffset, float yoffset);

public:
	std::map<unsigned int, std::pair<int, int> > carry_points, carried_points;
	
	boost::shared_ptr<class AudioSource> sound;

	// these are maps rather than vectors because ports can be destroyed
	std::map<unsigned int, boost::shared_ptr<InputPort> > inports; // XXX: do these need to be shared_ptr?
	std::map<unsigned int, boost::shared_ptr<OutputPort> > outports;

	void join(unsigned int outid, AgentRef dest, unsigned int inid);

	AgentRef carrying;
	AgentRef carriedby;
	AgentRef invehicle;
	
	inline bool isDying() const {
		return dying;
	}
	
	// attr
	unsigned int attr;
	// values which are always the same
	bool carryable() { return attr & 1; }
	bool mouseable() { return attr & 2; }
	bool activateable() { return attr & 4; }
	bool greedycabin() { return attr & 8; }
	bool invisible() { return attr & 16; }
	bool floatable() { return attr & 32; }
	// version-specific values
	// C1
	bool groundbound() { return attr & 64; }
	bool roombound() { return attr & 128; }
	// C2 and c2e
	bool suffercollisions() { return attr & 64; }
	bool sufferphysics() { return attr & 128; }
	// c2e
	bool camerashy() { return attr & 256; }
	bool openaircabin() { return attr & 512; }
	bool rotatable() { return attr & 1024; }
	bool presence() { return attr & 2048; }

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
	bool frozen : 1;
	bool visible : 1;
	bool displaycore : 1;

	int clac[3]; int clik;

	void setClassifier(unsigned char f, unsigned char g, unsigned short s);
	unsigned char family, genus;
	unsigned short species;
	int category;

	// motion
	caosVar velx, vely;

	float avel, fvel, svel;
	float admp, fdmp, sdmp;
	float spin;
	unsigned int spritesperrotation, numberrotations;

	caosVar accg, aero;
	unsigned int friction;
	int perm, elas;
	caosVar rest;

	float x, y;

	bool has_custom_core_size;
	float custom_core_xleft, custom_core_xright;
	float custom_core_ytop, custom_core_ybottom;

	bool falling : 1; // TODO: icky hack, possibly
	bool moved_last_tick : 1; // TODO: icky hack
	boost::weak_ptr<class Room> roomcache[5];

	caosVar range;

	AgentRef floatingagent;

	// Creatures 1/2 bits
	caosVar objp, babymoniker;

	// Creatures 2
	// TODO: size likely duplicate of perm
	caosVar actv, thrt, size;

	Agent(unsigned char f, unsigned char g, unsigned short s, unsigned int p);
	virtual ~Agent();
	
	virtual void finishInit();

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
	void stopScript();
	void pushVM(caosVM *newvm);
	bool vmStopped();

	void moveTo(float, float, bool force = false);
	bool tryMoveToPlaceAround(float x, float y);

	void setTimerRate(unsigned int r) { tickssincelasttimer = 0; timerrate = r; }
	
	virtual int handleClick(float, float);
	
	virtual CompoundPart *part(unsigned int id) = 0;
	
	unsigned int getWidth() { return part(0)->getWidth(); }
	unsigned int getHeight() { return part(0)->getHeight(); }
	Point const boundingBoxPoint(unsigned int n);
	Point const boundingBoxPoint(unsigned int n, Point p, float w, float h);
	shared_ptr<class Room> const bestRoomAt(unsigned int x, unsigned int y, unsigned int direction, class MetaRoom *m, shared_ptr<Room> exclude);
	void findCollisionInDirection(unsigned int i, class MetaRoom *m, Point src, int &dx, int &dy, Point &deltapt, double &delta, bool &collided, bool followrooms);

	bool validInRoomSystem();
	bool validInRoomSystem(Point p, float w, float h, int testperm);

	virtual void tick();
	virtual void kill();
	void unhandledException(std::string info, bool wasscript);

	virtual void setZOrder(unsigned int plane); // should be overridden!
	virtual unsigned int getZOrder() const;

	class shared_ptr<script> findScript(unsigned short event);
	
	int getUNID() const;
	std::string identify() const;

	void setAttributes(unsigned int a) { attr = a; }
	unsigned int getAttributes() const { return attr; }

	void playAudio(std::string filename, bool controlled, bool loop);

	shared_ptr<class genomeFile> getSlot(unsigned int s) { return slots[s]; }
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
