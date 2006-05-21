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
#include "CompoundAgent.h"
#include "CameraPart.h"

Camera *caosVM::getCamera() {
	Camera *c = camera.lock().get();
	if (!c) c = &world.camera;
	return c;
}

/**
 VISI (integer) checkall (integer)
 %status maybe

 Returns 1 if the TARG agent is on camera, or 0 otherwise. If checkall is 0, only checks 
 main camera, otherwise checks all.
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
 
 Sets the metaroom that the current camera is pointing at.  Coordinates point to top left of new
 camera position.  Set a coordinate to -1 to use the top-left corner of the metaroom.
 
 Transition: 0 for none, 1 for flip horzizontally, 2 for burst.
 */
void caosVM::c_META() {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(transition)
	VM_PARAM_INTEGER(camera_y)
	VM_PARAM_INTEGER(camera_x)
	VM_PARAM_INTEGER(metaroom_id)

	caos_assert(metaroom_id >= 0);
	MetaRoom *m = world.map.getMetaRoom(metaroom_id);
	if (!m) return; // DS does 'meta 0 -1 -1 0' in !map.cos for some stupid reason
	
	int camerax = camera_x; if (camerax == -1) camerax = m->x();
	int cameray = camera_y; if (cameray == -1) cameray = m->y();

	getCamera()->goToMetaRoom(metaroom_id, camerax, cameray, (cameratransition)transition);
}

/**
 META (integer)
 %status maybe

 Returns the metaroom the current camera is looking at.
*/
void caosVM::v_META() {
	if (getCamera()->getMetaRoom())
		result.setInt(getCamera()->getMetaRoom()->id);
	else // this is a hack for empathic vendor.cos in DS, which uses META before it's setup
		// TODO: work out what we should do instead of the hack
		result.setInt(-1);
}

/**
 CMRT (command) pan (integer)
 %status maybe

 Focuses the current camera on the TARG agent.

 Set pan to 0 for no panning (jump), 1 for smooth scrolling if in the same metaroom, or 2 
 for smooth scrolling if the given location is already visible.
*/
void caosVM::c_CMRT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pan)

	caos_assert(targ);
	
	MetaRoom *r = world.map.metaRoomAt(targ->x, targ->y);
	int xpos = targ->x - (getCamera()->getWidth() / 2) - (targ->getWidth() / 2);
	int ypos = targ->y - (getCamera()->getHeight() / 2) - (targ->getHeight() / 2);
	if (r)
		getCamera()->goToMetaRoom(r->id, xpos, ypos, (cameratransition)pan); // TODO: pan okay?
}

/**
 CMRA (command) x (integer) y (integer) pan (integer)
 %status maybe

 Sets the position of the current camera so that the top left corner of the view is at the 
 given coordinates.

 Set pan to 0 for no panning (jump), or 1 for smooth scrolling if in the same metaroom.
*/
void caosVM::c_CMRA() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(pan)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	
	getCamera()->moveTo(x, y, (panstyle)pan);
}

/**
 CMRP (command) x (integer) y (integer) pan (integer)
 %status maybe

 Sets the position of the current camera so that the view centers on the given coordinates.

 Set pan to 0 for no panning (jump), 1 for smooth scrolling if in the same metaroom, or 2 
 for smooth scrolling if the given location is already visible.
*/
void caosVM::c_CMRP() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(pan)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	
	getCamera()->moveTo(x - (getCamera()->getWidth() / 2), y - (getCamera()->getHeight() / 2), (panstyle)pan);
}

/**
 CMRX (integer)
 %status maybe

 Returns the X position at the center of the current camera's view.
*/
void caosVM::v_CMRX() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(getCamera()->getXCentre());
}

/**
 CMRY (integer)
 %status maybe

 Returns the Y position at the center of the current camera's view.
*/
void caosVM::v_CMRY() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(getCamera()->getYCentre());
}

/**
 WNDW (integer)
 %status maybe

 Returns the width of the current camera's view.
*/
void caosVM::v_WNDW() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(getCamera()->getWidth());
}

/**
 WNDH (integer)
 %status maybe

 Returns the height of the current camera's view.
*/
void caosVM::v_WNDH() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(getCamera()->getHeight());
}

/**
 WNDB (integer)
 %status maybe

 Returns the position of the bottom edge of the current camera's view.
*/
void caosVM::v_WNDB() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(getCamera()->getY() + getCamera()->getHeight());
}

/**
 WNDL (integer)
 %status maybe

 Returns the position of the left edge of the current camera's view.
*/
void caosVM::v_WNDL() {
	VM_VERIFY_SIZE(0)

	result.setInt(getCamera()->getX());
}

/**
 WNDR (integer)
 %status maybe

 Returns the position of the right edge of the current camera's view.
*/
void caosVM::v_WNDR() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(getCamera()->getX() + getCamera()->getWidth());
}

/**
 WNDT (integer)
 %status maybe

 Returns the position of the top edge of the current camera's view.
*/
void caosVM::v_WNDT() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(getCamera()->getY());
}

/**
 WDOW (command)
 %status stub

 Toggles full-screen mode on and off.
*/
void caosVM::c_WDOW() {
	// TODO
}

/**
 WDOW (integer)
 %status stub

 Returns 1 if in full-screen mode, or 0 otherwise (windowed).
*/
void caosVM::v_WDOW() {
	result.setInt(0);
}

/**
 TRCK (command) agent (agent) xpercent (integer) ypercent (integer) style (integer) transition (integer)
 %status maybe

 Tell the current camera to track the specified agent, or set to NULL to stop tracking.
 xpercent and ypercent define a rectangle on the screen which the camera should keep the tracked agent inside.
 Style 0 is brittle-- tracking is broken easily.  Style 1 is flexible-- moving the camera 
 back inside the rectangle resumes tracking.  Style 3 is hard-- you can't move outside the 
 rectangle.
 Rransition: 0 for none, 1 for flip horizontal, 2 for burst.
*/
void caosVM::c_TRCK() {
	VM_PARAM_INTEGER(transition)
	VM_PARAM_INTEGER(style)
	VM_PARAM_INTEGER(ypercent)
	VM_PARAM_INTEGER(xpercent)
	VM_PARAM_AGENT(agent)

	getCamera()->trackAgent(agent, xpercent, ypercent, (trackstyle)style, (cameratransition)transition);
}

/**
 LINE (command) x1 (integer) y1 (integer) x2 (integer) y2 (integer) r (integer) g (integer) b (integer) stipple_on (integer) stipple_off (integer)
 %status stub

 Draw a line between two endpoints with the given coordinates, in the color specified by 
 the given red, green, and blue values. 
 
 Set stipple_on to a number of pixels to alternate for stippling, or set stipple_off to 
 turn stippling off.

 Setting the the endpoints to the same point will remove all lines for the agent.
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

/**
 SNAX (integer) filename (string)
 %status maybe

 Determines whether or not the given image file exists in the world 
 images directory (0 or 1).
*/
void caosVM::v_SNAX() {
	VM_PARAM_STRING(filename)
	
	result.setInt(0);
	if (!world.findFile(std::string("/Images/") + filename + ".s16").empty())
		result.setInt(1);
	else if (!world.findFile(std::string("/Images/") + filename + ".c16").empty())
		result.setInt(1);
}

/**
 SCAM (command) agent (agent) part (integer)
 %status maybe

 Sets which camera to use in camera macro commands.  If 'agent' and 
 'part' are NULL, the main camera will be used.
*/
void caosVM::c_SCAM() {
	VM_PARAM_INTEGER(part)
	VM_PARAM_AGENT(agent)

	if (!agent) {
		camera.reset();
		return;
	}
	
	CompoundAgent *a = dynamic_cast<CompoundAgent *>(agent.get());
	caos_assert(a);
	CompoundPart *p = a->part(part);
	caos_assert(p);
	CameraPart *c = dynamic_cast<CameraPart *>(p);
	caos_assert(c);

	camera = c->getCamera();
}

/**
 ZOOM (command) pixels (integer) x (integer) y (integer)
 %status stub

 Zoom the current camera by the given number of pixels on the given x/y 
 coordinates in the world.  If the coordinates are set to -1, the 
 current center position of the camera's view will be used.
*/
void caosVM::c_ZOOM() {
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(pixels)

	// TODO
}

/**
 SNAP (command) filename (string) x (integer) y (integer) width (integer) height (integer) zoom (integer)
 %status stub

 Take a snapshot and save it to the given filename (don't include the extension). x/y are the centerpoint, width/height the size, and zoom the percentage to zoom out.
*/
void caosVM::c_SNAP() {
	VM_PARAM_INTEGER(zoom)
	VM_PARAM_INTEGER(height)
	VM_PARAM_INTEGER(width)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_STRING(filename)

	// TODO
}

/**
 LOFT (integer) filename (string)
 %status stub
*/
void caosVM::v_LOFT() {
	VM_PARAM_STRING(filename)

	result.setInt(0); // TODO
}

/* vim: set noet: */
