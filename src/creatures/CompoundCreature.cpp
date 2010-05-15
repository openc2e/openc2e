/*
 *  CompoundCreature.cpp
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

#include "CompoundCreature.h"

CompoundCreature::CompoundCreature(unsigned char _family, unsigned int plane, std::string spritefile, unsigned int firstimage, unsigned int imagecount)
	: CompoundAgent(_family, 0, 0, plane, spritefile, firstimage, imagecount) {
}

CompoundCreature::~CompoundCreature() {
}

void CompoundCreature::tick() {
	CompoundAgent::tick();

	if (paused) return;

	CreatureAgent::tick();
}

/* vim: set noet: */
