/*
 *  CompoundCreature.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon Jul 7 2008.
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
#include "CreatureAgent.h"

class CompoundCreature : public CompoundAgent, public CreatureAgent {
  protected:
	Agent* getAgent() { return this; }

  public:
	CompoundCreature(unsigned char _family, unsigned int plane, std::string spritefile, unsigned int firstimage, unsigned int imagecount);
	virtual ~CompoundCreature();

	void tick();
};

/* vim: set noet: */
