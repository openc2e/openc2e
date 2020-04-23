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
#include <map>
#include <string>
#include "CompoundPart.h"

struct Hotspot {
	int left, top, right, bottom;
};

struct HotspotFunction {
	int hotspot;
	uint16_t message;
	uint8_t mask;
};

class CompoundAgent : public Agent {
protected:
	std::vector<CompoundPart *> parts;
	
	// C1/C2 data
	Hotspot hotspots[6];
	HotspotFunction hotspotfunctions[6];
	std::string spritefile;
	unsigned int firstimage, imagecount;
	unsigned int next_part_sequence_number = 0;

public:
	CompoundAgent(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
								std::string spritefile, unsigned int firstimage, unsigned int imagecount);
	CompoundAgent(std::string spritefile, unsigned int firstimage, unsigned int imagecount); // C1/C2 constructor
	virtual ~CompoundAgent();
		
	unsigned int partCount() { return parts.size(); }
	CompoundPart *part(unsigned int id);
	virtual void addPart(CompoundPart *);
	void delPart(unsigned int);
	unsigned int nextPartSequenceNumber();
	virtual void tick();
	void setZOrder(unsigned int plane);

	int handleClick(float, float);
	bool fireScript(unsigned short event, Agent *from, caosVar one, caosVar two);
	void setHotspotLoc(unsigned int id, int l, int t, int r, int b);
	void setHotspotFunc(unsigned int id, unsigned int h);
	void setHotspotFuncDetails(unsigned int id, uint16_t message, uint8_t flags);

	// accessor functions for NEW: PART and similar
	std::string getSpriteFile() { return spritefile; }
	unsigned int getFirstImage() { return firstimage; }
	unsigned int getImageCount() { return imagecount; }

	friend class caosVM;
};

#endif
/* vim: set noet: */
