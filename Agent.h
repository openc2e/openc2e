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
#include "caosVM.h" // caosVar and caosVM below
#include "AgentRef.h"

class Agent {
	friend struct agentzorder;
	friend class caosVM;
	friend class AgentRef;

protected:
	caosVar var[100]; // OVxx
	AgentRef self;
	caosVM *vm;

	void zotrefs();

	int unid;

public:
	bool carryable, mouseable, activateable, invisible, floatable;
	bool suffercollisions, sufferphysics, camerashy, rotatable, presence;
	
	bool visible;
	unsigned char family, genus;
	unsigned short species;
	unsigned int zorder;
	unsigned int tickssincelasttimer, timerrate;
	caosVar velx, vely;
	float accg, aero;
	unsigned int friction;
	float x, y;

	float range;

	bool immortal, dying;
	
	void fireScript(unsigned short event);
	void moveTo(float, float);
	void setTimerRate(unsigned int r) { tickssincelasttimer = 0; timerrate = r; }
	
	Agent(unsigned char f, unsigned char g, unsigned short s, unsigned int p);
	virtual ~Agent();

	virtual void setAttributes(unsigned int attr) = 0;
	virtual unsigned int getAttributes() = 0;
	virtual unsigned int getWidth() = 0;
	virtual unsigned int getHeight() = 0;

	virtual void tick();
	virtual void render(SDLBackend *renderer, int xoffset, int yoffset) = 0;
	virtual void kill();

	int getUNID();
	std::string identify() const;
};

struct agentzorder {
  bool operator()(const Agent *s1, const Agent *s2) const {
    return s1->zorder < s2->zorder;
  }
};

#endif
