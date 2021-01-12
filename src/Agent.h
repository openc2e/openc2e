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

#include "AgentRef.h"
#include "CompoundPart.h"
#include "Port.h"
#include "Sound.h"
#include "physics.h"
#include "serfwd.h"
#include "utils/heap_array.h"

#include <cassert>
#include <list>
#include <map>
#include <memory>
#include <set>

class script;
class genomeFile;

class caosValue;
struct caosValueCompare {
	bool operator()(const caosValue& v1, const caosValue& v2) const;
};

class caosVM;

class Agent : public std::enable_shared_from_this<Agent> {
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

	FRIEND_SERIALIZE(Agent)

  protected:
	bool initialized;
	int lifecount;
	int lastScript;

	int unused_cint;

	void zotstack();

	mutable int unid;
	unsigned int tickssincelasttimer;

	bool wasmoved;

	std::list<std::shared_ptr<Agent> >::iterator agents_iter;
	std::list<caosVM*> vmstack; // for CALL etc
	std::vector<AgentRef> floated;

	void updateAudio(Sound);
	bool dying : 1;

	void vmTick();

	virtual void physicsTick();
	void physicsTickC2();

	virtual void carry(AgentRef);
	virtual void drop(AgentRef);

	virtual std::pair<int, int> getCarryPoint();
	virtual std::pair<int, int> getCarriedPoint();
	virtual void adjustCarried(float xoffset, float yoffset);

  public:
	int emitca_index;
	float emitca_amount;
	std::map<unsigned int, std::shared_ptr<genomeFile> > genome_slots;
	int lastcollidedirection;
	std::map<caosValue, caosValue, caosValueCompare> name_variables;
	unsigned int timerrate;
	heap_array<caosValue, 100> var; // OVxx
	caosVM* vm;
	unsigned int zorder;
	virtual bool fireScript(unsigned short event, Agent* from, caosValue one, caosValue two);

	std::map<unsigned int, std::pair<int, int> > carry_points, carried_points;

	std::shared_ptr<class VoiceData> voice;
	std::vector<std::pair<std::string, unsigned int> > pending_voices;
	unsigned int ticks_until_next_voice = 0;
	void setVoice(std::string name);
	void speak(std::string sentence);
	void tickVoices();

	Sound sound;

	// these are maps rather than vectors because ports can be destroyed
	std::map<unsigned int, std::shared_ptr<InputPort> > inports; // XXX: do these need to be std::shared_ptr?
	std::map<unsigned int, std::shared_ptr<OutputPort> > outports;

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

	int clac[3];
	int clik;

	void setClassifier(unsigned char f, unsigned char g, unsigned short s);
	unsigned char family, genus;
	unsigned short species;
	int category;

	// motion
	float velx = 0;
	float vely = 0;

	float avel, fvel, svel;
	float admp, fdmp, sdmp;
	float spin;
	unsigned int spritesperrotation, numberrotations;

	float accg = 0;
	int aero = 0;
	unsigned int friction;
	int perm, elas;
	int rest = 0;

	float x, y;

	bool has_custom_core_size;
	float custom_core_xleft, custom_core_xright;
	float custom_core_ytop, custom_core_ybottom;

	bool falling : 1; // TODO: icky hack, possibly
	bool moved_last_tick : 1; // TODO: icky hack
	std::weak_ptr<class Room> roomcache[5];

	float range = 0;

	AgentRef floatingagent;

	// Creatures 1/2 bits
	AgentRef objp;
	int babymoniker = 0;

	// Creatures 2
	// TODO: size likely duplicate of perm
	int actv = 0;
	int thrt = 0;
	int size = 0;

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

	bool queueScript(unsigned short event);
	bool queueScript(unsigned short event, AgentRef from);
	bool queueScript(unsigned short event, AgentRef from, caosValue p0);
	bool queueScript(unsigned short event, AgentRef from, caosValue p0, caosValue p1);
	void stopScript();
	void pushVM(caosVM* newvm);
	bool vmStopped();

	void moveTo(float, float, bool force = false);
	bool tryMoveToPlaceAround(float x, float y);

	void setTimerRate(unsigned int r) {
		tickssincelasttimer = 0;
		timerrate = r;
	}

	virtual int handleClick(float, float);

	virtual CompoundPart* part(unsigned int id) = 0;

	unsigned int getWidth() { return part(0)->getWidth(); }
	unsigned int getHeight() { return part(0)->getHeight(); }
	Point const boundingBoxPoint(unsigned int n);
	Point const boundingBoxPoint(unsigned int n, Point p, float w, float h);
	std::shared_ptr<class Room> const bestRoomAt(unsigned int x, unsigned int y, unsigned int direction, class MetaRoom* m, std::shared_ptr<Room> exclude);
	void findCollisionInDirection(unsigned int i, class MetaRoom* m, Point src, int& dx, int& dy, Point& deltapt, double& delta, bool& collided, bool followrooms);

	bool validInRoomSystem();
	bool validInRoomSystem(Point p, float w, float h, int testperm);

	virtual void tick();
	virtual void kill();
	void unhandledException(std::string info, bool wasscript);

	virtual void setZOrder(unsigned int plane); // should be overridden!
	virtual unsigned int getZOrder() const;

	class std::shared_ptr<script> findScript(unsigned short event);

	int getUNID() const;
	std::string identify() const;

	void setAttributes(unsigned int a) { attr = a; }
	unsigned int getAttributes() const { return attr; }

	void playAudio(std::string filename, bool controlled, bool loop);

	void setSlot(unsigned int s, std::shared_ptr<genomeFile> g) { genome_slots[s] = g; }
	std::shared_ptr<genomeFile> getSlot(unsigned int s) { return genome_slots[s]; }
};

class LifeAssert {
  protected:
	Agent* p;

  public:
	LifeAssert(const AgentRef& ref) {
		p = ref.get();
		assert(p);
		p->lifecount++;
	}
	LifeAssert(const std::weak_ptr<Agent>& p_) {
		p = p_.lock().get();
		assert(p);
		p->lifecount++;
	}
	LifeAssert(const std::shared_ptr<Agent>& p_) {
		p = p_.get();
		assert(p);
		p->lifecount++;
	}
	LifeAssert(Agent* p_) {
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
