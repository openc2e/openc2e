/*
 *  CompoundAgent.h
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

#ifndef _COMPOUNDAGENT_H
#define _COMPOUNDAGENT_H

#include "Agent.h"
#include "creaturesImage.h"
#include <map>
#include <string>
#include "CompoundPart.h"

class CompoundAgent : public Agent {
protected:
	std::vector<CompoundPart *> parts;

	// TODO: see note below
	unsigned int width, height;
	unsigned int fullwidth, fullheight; // getCheckXXX hack, see Agent.h

public:
	CompoundAgent(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
								std::string spritefile, unsigned int firstimage, unsigned int imagecount);
	virtual ~CompoundAgent();
		
	unsigned int partCount() { return parts.size(); }
	CompoundPart *part(unsigned int id);
	void addPart(CompoundPart *);
	void delPart(unsigned int);
	virtual void tick();
	virtual unsigned int getWidth() { return part(0)->getWidth(); }
	virtual unsigned int getHeight() { return part(0)->getHeight(); }
	virtual unsigned int getCheckWidth() { return fullwidth; }
	virtual unsigned int getCheckHeight() { return fullheight; }
	void handleClick(float, float);

	virtual void render(SDLBackend *renderer, int xoffset, int yoffset);

	friend class caosVM;
};

#endif
/* vim: set noet: */
