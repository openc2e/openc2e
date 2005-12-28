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
	
	if ((targ->x > (world.camera.getX() + world.camera.getWidth())) || ((targ->x + targ->getWidth()) < world.camera.getX()) ||
		(targ->y > (world.camera.getY() + world.camera.getHeight())) || ((targ->y + targ->getHeight()) < world.camera.getY())) {
		result.setInt(0);
		return;
	}

	result.setInt(1);
}

/**
 META (command) metaroom_id (integer) camera_x (integer) camera_y (integer) transition (integer)
 %status maybe
 
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
 META (integer)
 %status maybe

 Returns the metaroom the current camera is looking at.
*/
void caosVM::v_META() {
	if (world.camera.getMetaRoom())
		result.setInt(world.camera.getMetaRoom()->id);
	else // this is a hack for empathic vendor.cos in DS, which uses META before it's setup
		// TODO: work out what we should do instead of the hack
		result.setInt(-1);
}

/**
 CMRT (command) pan (integer)
 %status maybe

 centre current camera on TARG.

 pan: 0 for none (jump), 1 for smooth scroll if in same metaroom, 2 for smooth scroll if visible
 */
void caosVM::c_CMRT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pan)

	caos_assert(targ);
	
	MetaRoom *r = world.map.metaRoomAt(targ->x, targ->y);
	int xpos = targ->x - (world.camera.getWidth() / 2) - (targ->getWidth() / 2);
	int ypos = targ->y - (world.camera.getHeight() / 2) - (targ->getHeight() / 2);
	if (r)
		world.camera.goToMetaRoom(r->id, xpos, ypos, (cameratransition)pan); // TODO: pan okay?
}

/**
 CMRA (command) x (integer) y (integer) pan (integer)
 %status maybe
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
 %status maybe
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
 %status maybe
*/
void caosVM::v_CMRX() {
	VM_VERIFY_SIZE(0)

	result.setInt(world.camera.getXCentre());
}

/**
 CMRY (integer)
 %status maybe
*/
void caosVM::v_CMRY() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(world.camera.getYCentre());
}

/**
 WNDW (integer)
 %status maybe
*/
void caosVM::v_WNDW() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(world.camera.getWidth());
}

/**
 WNDH (integer)
 %status maybe
*/
void caosVM::v_WNDH() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(world.camera.getHeight());
}

/**
 WNDB (integer)
 %status maybe
*/
void caosVM::v_WNDB() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(world.camera.getY() + world.camera.getHeight());
}

/**
 WNDL (integer)
 %status maybe
*/
void caosVM::v_WNDL() {
	VM_VERIFY_SIZE(0)

	result.setInt(world.camera.getX());
}

/**
 WNDR (integer)
 %status maybe
*/
void caosVM::v_WNDR() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(world.camera.getX() + world.camera.getWidth());
}

/**
 WNDT (integer)
 %status maybe
*/
void caosVM::v_WNDT() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(world.camera.getY());
}

/**
 WDOW (command)
 %status stub

 Toggle full-screen mode.
*/
void caosVM::c_WDOW() {
	// TODO
}

/**
 WDOW (integer)
 %status stub

 Return 1 if in full-screen mode, or 0 otherwise (windowed).
*/
void caosVM::v_WDOW() {
	result.setInt(0);
}

/**
 TRCK (command) agent (agent) xpercent (integer) ypercent (integer) style (integer) transition (integer)
 %status maybe

 Tell the current camera to track the specified agent, or set to NULL to stop tracking.
 xpercent and ypercent define a rectangle on the screen which the camera should keep the tracked agent inside.
 style 0 is brittle, tracking is broken easily. style 1 is flexible, moving the camera back inside the rectangle resumes tracking. style 3 is hard, you can't move outside the rectangle.
 transition: 0 for none, 1 for flip horz, 2 for burst
*/
void caosVM::c_TRCK() {
	VM_PARAM_INTEGER(transition)
	VM_PARAM_INTEGER(style)
	VM_PARAM_INTEGER(ypercent)
	VM_PARAM_INTEGER(xpercent)
	VM_PARAM_VALIDAGENT(agent)

	world.camera.trackAgent(agent, xpercent, ypercent, (trackstyle)style, (cameratransition)transition);
}

/**
 LINE (command) x1 (integer) y1 (integer) x2 (integer) y2 (integer) r (integer) g (integer) b (integer) stipple_on (integer) stipple_off (integer)
 %status stub
*/
void caosVM::c_LINE() {
	VM_PARAM_INTEGER(stipple_off)
	VM_PARAM_INTEGER(stipple_on)
	VM_PARAM_INTEGER(b)
	VM_PARAM_INTEGER(g)
	VM_PARAM_INTEGER(r)
	VM_PARAM_INTEGER(y2)
	VM_PARAM_INTEGER(x2)
	VM_PARAM_INTEGER(y1)
	VM_PARAM_INTEGER(x1)
	
	caos_assert(targ);
	// TODO
}
/* vim: set noet: */
