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

class Agent {
public:
	bool visible;
	unsigned char family, genus;
	unsigned short species;
	unsigned int zorder;
	caosVar var[100]; // OVxx
	
	caosVM vm;

	void fireScript(unsigned char event);
	virtual void render(SDLBackend *renderer, int xoffset, int yoffset) = 0;
	unsigned int x, y;
	void moveTo(unsigned int, unsigned int);
	Agent(unsigned char f, unsigned char g, unsigned short s, unsigned int p);
	virtual bool isSimple() { return false; }
	virtual ~Agent() { }

	virtual void setAttributes(unsigned int attr) = 0;
	virtual unsigned int getAttributes() = 0;

	virtual void tick();

	friend struct agentzorder;
};

struct agentzorder {
  bool operator()(const Agent *s1, const Agent *s2) const {
    return s1->zorder < s2->zorder;
  }
};

#endif
