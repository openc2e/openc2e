/*
 *  Blackboard.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Jan 12 2008.
 *  Copyright (c) 2008 Alyssa Milburn. All rights reserved.
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

#include "CompoundAgent.h"

#ifndef _C2E_BLACKBOARD_H
#define _C2E_BLACKBOARD_H

class Blackboard : public CompoundAgent {
public:
	Blackboard(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
		std::string spritefile, unsigned int firstimage, unsigned int imagecount)
		: CompoundAgent(family, genus, species, plane, spritefile, firstimage, imagecount) { }
	Blackboard(std::string spritefile, unsigned int firstimage, unsigned int imagecount)
		: CompoundAgent(spritefile, firstimage, imagecount) { }
	
	// TODO: blackboard data
};

#endif
/* vim: set noet: */
