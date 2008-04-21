/*
 *  CallButton.cpp
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

#include "CallButton.h"

// TODO: this code is a first attempt and is probably completely wrong

void CallButton::tick() {
	SimpleAgent::tick();
	if (paused) return;

	Lift *ourlift = dynamic_cast<Lift *>(lift.get());
	assert(ourlift);

	if (actv.getInt() == 1) {
		// TODO: hrm..
		if (ourlift->currentbutton == buttonid && ourlift->liftBottom() == ourlift->callbuttony[buttonid]) { // has arrived at us
			queueScript(0); // deactivate ourselves
		}
		
		if (ourlift->liftAvailable()) { // not moving
			if (ourlift->currentbutton != buttonid) {
				/*
				 * all active call buttons need to fight it out to work out who gets to be
				 * 'newbutton' on the lift (which should be the closest button)
				 */
				int buttondist = abs(ourlift->currentbutton - buttonid);
				int currbuttondist = abs(ourlift->currentbutton - ourlift->newbutton);

				// if there's no change to newbutton yet, or if we're 'nearer' than the last newbutton..
				if (currbuttondist == 0 || currbuttondist > buttondist)
					// .. then set newbutton to point to us instead
					ourlift->newbutton = buttonid;
			}
		}
	}
}

bool CallButton::fireScript(unsigned short event, Agent *from, caosVar one, caosVar two) {
	Lift *ourlift = dynamic_cast<Lift *>(lift.get());
	assert(ourlift);

	switch (event) {
		case 1:
			if (ourlift->currentbutton == buttonid) {
				return false; // nuh-uh
			}
	}

	return Agent::fireScript(event, from, one, two);
}

/* vim: set noet: */
