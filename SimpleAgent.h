/*
 *  SimpleAgent.h
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

#include "Agent.h"
#include <string>
#include <vector>

#ifndef _C2E_SIMPLEAGENT_H
#define _C2E_SIMPLEAGENT_H

class SimpleAgent : public Agent {
protected:
	unsigned int first_image, image_count;
	unsigned int frameno;

public:
	bool carryable, mouseable, activateable, invisible, floatable;
	bool suffercollisions, sufferphysics, camerashy, rotatable, presence;

	std::vector<unsigned int> animation;

	SimpleAgent(unsigned int family, unsigned int genus, unsigned int species, unsigned int plane,
							unsigned int firstimage, unsigned int imagecount);
	unsigned int getFirstImage();
	unsigned int getImageCount();
	void setImage(std::string img);
	virtual bool isSimple() { return false; }
	virtual void setAttributes(unsigned int attr);
	virtual unsigned int getAttributes();

	void setFrameNo(unsigned int);
	unsigned int getCurrentSprite();

	virtual void tick();
};

#endif
