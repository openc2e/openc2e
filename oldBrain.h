/*
 *  oldBrain.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon Aug 13 2007.
 *  Copyright (c) 2007 Alyssa Milburn. All rights reserved.
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

#ifndef __OLDBRAIN_H
#define __OLDBRAIN_H

#include "genome.h"
#include <boost/shared_ptr.hpp>
#include <set>
#include <map>

using boost::shared_ptr;

class Creature;

class oldLobe {
};

class oldBrain {
protected:
	// TODO: should be oldCreature?
	class c1Creature *parent;

public:
	std::map<unsigned int, oldLobe *> lobes;

	oldBrain(c1Creature *p);
	void tick();
	void init();
	oldLobe *getLobeByTissue(unsigned int id);
	c1Creature *getParent() { return parent; }
};

#endif

/* vim: set noet: */
