/*
 *  Lift.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Dec 2 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

#include "Lift.h"
#include "Engine.h" // version

Lift::Lift(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
		std::string spritefile, unsigned int firstimage, unsigned int imagecount)
		: Vehicle(family, genus, species, plane, spritefile, firstimage, imagecount) {
	if (engine.version == 1) {
		alignwithcabin = true;
	} else {
		alignwithcabin = false;
	}
}

/*
 * TODO: this code is a first attempt and might be completely wrong
 */

bool Lift::fireScript(unsigned short event, Agent *from, caosVar one, caosVar two) {
	if (event == 1 || event == 2) {
		if (!liftAvailable()) return false; // TODO: hack to make sure the lifts aren't activated when not ready
	}

	// if we need to select a new callbutton.. TODO: this is hacky
	if (y + cabinbottom == callbuttony[currentbutton]) {
		if (event == 1) {
			if (currentbutton + 1 == callbuttony.size()) return false;
			currentbutton++; newbutton = currentbutton;
		} else if (event == 2) {
			if (currentbutton == 0) return false;
			currentbutton--; newbutton = currentbutton;
		}
	}

	return Agent::fireScript(event, from, one, two);
}

void Lift::tick() {
	Vehicle::tick();
	if (paused) return;

	// if we're moving..
	if (yvec.getInt() != 0) {
		// are we beyond the call button y point?
		if (
			(yvec.getInt() < 0 && liftBottom() <= callbuttony[currentbutton]) || // upwards
			(yvec.getInt() > 0 && liftBottom() >= callbuttony[currentbutton]) // downwards 
			) {
			// stop movement (and make sure we're in the right spot)
			yvec.setInt(0);
			y = callbuttony[currentbutton] - (alignwithcabin ? cabinbottom : getHeight());

			// send deactivate event
			queueScript(0);
		}
	}

	if (liftAvailable() && newbutton != currentbutton) {
		currentbutton = newbutton;

		if (liftBottom() < callbuttony[currentbutton])
			queueScript(1, this);
		else
			queueScript(2, this);
	}
}

float Lift::liftBottom() {
	if (alignwithcabin)
		return y + cabinbottom;
	else
		return y + getHeight();
}

/* vim: set noet: */
