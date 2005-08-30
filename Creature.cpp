/*
 *  Creature.cpp
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

#include "Creature.h"

Creature::Creature(genomeFile *g, unsigned char _family, bool is_female, unsigned char _variant)
 : Agent(_family, 0, 0, 0) { // TODO: fill in correct values
	female = is_female;
}

void Creature::setAttributes(unsigned int attr) {
	carryable = (attr & 1);
	mouseable = (attr & 2);
	activateable = (attr & 4);
	invisible = (attr & 16);
	floatable = (attr & 32);
	suffercollisions = (attr & 64);
	sufferphysics = (attr & 128);
	camerashy = (attr & 256);
	rotatable = (attr & 1024);
	presence = (attr & 2048);
}

unsigned int Creature::getAttributes() {
	unsigned int a = (carryable ? 1 : 0);
	a += (mouseable ? 2: 0);
	a += (activateable ? 4: 0);
	a += (invisible ? 16: 0);
	a += (floatable ? 32: 0);
	a += (suffercollisions ? 64: 0);
	a += (sufferphysics ? 128: 0);
	a += (camerashy ? 256: 0);
	a += (rotatable ? 1024: 0);
	return a + (presence ? 2048: 0);
}
/* vim: set noet: */
