/*
 *  caosVM_camera.cpp
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

#include "caosVM.h"
#include "World.h"

/**
 META (command) metaroom_id (integer) camera_x (integer) camera_y (integer) transition (integer)
 
 set metaroom current camera is pointing at. coordinates point to top left of new
 camera position.
 
 transition: 0 for none, 1 for flip horz, 2 for burst
 */
void caosVM::c_META() {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(transition)
	VM_PARAM_INTEGER(camera_y)
	VM_PARAM_INTEGER(camera_x)
	VM_PARAM_INTEGER(metaroom_id)

	// todo: transitions

	assert(metaroom_id >= 0);
	assert((unsigned int)metaroom_id < world.map.getMetaRoomCount());
	world.map.SetCurrentMetaRoom(world.map.getCurrentMetaRoom()->id - 1);

/*
	adjustx = world.map.getCurrentMetaRoom()->x() + camera_x;
	adjusty = world.map.getCurrentMetaRoom()->y() + camera_y;
*/
}

/**
 CMRT (command) pan (integer)

 centre current camera on TARG.

 pan: 0 for none (jump), 1 for smooth scroll if in same metaroom, 2 for smooth scroll if visible
 */
void caosVM::c_CMRT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pan)
	// todo
}
