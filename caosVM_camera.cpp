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
 VISI (integer) checkall (integer)
 %status maybe

 Returns 1 if target agent is on camera, or 0 otherwise. If checkall is 0, only checks main camera, otherwise checks all.
*/
void caosVM::v_VISI() {
	VM_PARAM_INTEGER(checkall)

	// TODO: check non-main cameras
	// TODO: do compound parts stick out of the agent?
	
	caos_assert(targ);
	
	if (targ->x > (world.camera.getX() + world.camera.getWidth()))
		if (targ->y > (world.camera.getY() + world.camera.getHeight()))
			if ((targ->x + targ->getWidth()) < world.camera.getX())
				if ((targ->y + targ->getHeight()) < world.camera.getY()) {
					result.setInt(0);
					return;
				}

	result.setInt(1);
}

/**
 META (command) metaroom_id (integer) camera_x (integer) camera_y (integer) transition (integer)
 
 set metaroom current camera is pointing at. coordinates point to top left of new
 camera position. set a coordinate to -1 to use the top-left corner of the metaroom.
 
 transition: 0 for none, 1 for flip horz, 2 for burst
 */
void caosVM::c_META() {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(transition)
	VM_PARAM_INTEGER(camera_y)
	VM_PARAM_INTEGER(camera_x)
	VM_PARAM_INTEGER(metaroom_id)

	assert(metaroom_id >= 0);
	MetaRoom *m = world.map.getMetaRoom(metaroom_id);
	if (!m) return; // DS does 'meta 0 -1 -1 0' in !map.cos for some stupid reason
	
	int camerax = camera_x; if (camerax == -1) camerax = m->x();
	int cameray = camera_y; if (cameray == -1) cameray = m->y();
	
	world.camera.goToMetaRoom(metaroom_id, camerax, cameray, (cameratransition)transition);
}

/**
 CMRT (command) pan (integer)

 centre current camera on TARG.

 pan: 0 for none (jump), 1 for smooth scroll if in same metaroom, 2 for smooth scroll if visible
 */
void caosVM::c_CMRT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pan)
	// TODO
}

/**
 CMRA (command) x (integer) y (integer) pan (integer)
*/
void caosVM::c_CMRA() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(pan)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	world.camera.moveTo(x, y, (panstyle)pan);
}

/**
 CMRP (command) x (integer) y (integer) pan (integer)
*/
void caosVM::c_CMRP() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(pan)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	world.camera.moveTo(x - (world.camera.getWidth() / 2), y - (world.camera.getHeight() / 2), (panstyle)pan);
}

/**
 CMRX (integer)
*/
void caosVM::v_CMRX() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(0); // TODO
}

/**
 CMRY (integer)
*/
void caosVM::v_CMRY() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(0); // TODO
}

/**
 WNDW (integer)
*/
void caosVM::v_WNDW() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(world.camera.getWidth());
}

/**
 WNDH (integer)
*/
void caosVM::v_WNDH() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(world.camera.getHeight());
}

/**
 WNDB (integer)
*/
void caosVM::v_WNDB() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(0); // TODO
}

/**
 WNDL (integer)
*/
void caosVM::v_WNDL() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(0); // TODO
}

/**
 WNDR (integer)
*/
void caosVM::v_WNDR() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(0); // TODO
}

/**
 WNDT (integer)
*/
void caosVM::v_WNDT() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(0); // TODO
}

/* vim: set noet: */
