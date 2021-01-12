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

#include "Camera.h"
#include "CameraPart.h"
#include "CompoundAgent.h"
#include "Engine.h"
#include "Map.h"
#include "MetaRoom.h"
#include "World.h"
#include "caosVM.h"
#include "caos_assert.h"

Camera* caosVM::getCamera() {
	Camera* c = camera.lock().get();
	if (!c)
		c = engine.camera.get();
	return c;
}

bool agentOnCamera(Agent* targ, bool checkall) {
	MetaRoom* m = world.map->metaRoomAt(targ->x, targ->y);
	if (!m || m != engine.camera->getMetaRoom())
		return false;

	// TODO: check non-main cameras?
	(void)checkall;
	// TODO: do compound parts stick out of the agent?

	// y coordinates don't wrap
	if (targ->y + targ->getHeight() < engine.camera->getY())
		return false;
	if (targ->y > engine.camera->getY() + engine.camera->getHeight())
		return false;

	// if an agent is off-camera to the right, it's not visible
	if (targ->x > engine.camera->getX() + engine.camera->getWidth())
		return false;

	if (targ->x + targ->getWidth() < engine.camera->getX()) {
		// if an agent is off-camera to the left, it might be wrapping
		if (!m->wraparound() || (targ->x + targ->getWidth() + m->width() < engine.camera->getX()))
			return false;
	}

	return true;
}

/**
 VISI (integer) checkall (integer)
 %status maybe

 Returns 1 if the TARG agent is on camera, or 0 otherwise. If checkall is 0, only checks 
 main camera, otherwise checks all.
*/
void v_VISI(caosVM* vm) {
	VM_PARAM_INTEGER(checkall)

	valid_agent(vm->targ);

	if (agentOnCamera(vm->targ, checkall))
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 ONTV (integer) agent (agent) checkall (integer)
 %status maybe

 Returns 1 if the specified agent is on camera, or 0 otherwise. If checkall is 0, only checks 
 main camera, otherwise checks all.
*/
void v_ONTV(caosVM* vm) {
	VM_PARAM_INTEGER(checkall)
	VM_PARAM_VALIDAGENT(agent)

	if (agentOnCamera(agent.get(), checkall))
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 META (command) metaroom_id (integer) camera_x (integer) camera_y (integer) transition (integer)
 %status maybe
 
 Sets the metaroom that the current camera is pointing at.  Coordinates point to top left of new
 camera position.  Set a coordinate to -1 to use the top-left corner of the metaroom.
 
 Transition: 0 for none, 1 for flip horzizontally, 2 for burst.
 */
void c_META(caosVM* vm) {
	VM_VERIFY_SIZE(4)
	VM_PARAM_INTEGER(transition)
	VM_PARAM_INTEGER(camera_y)
	VM_PARAM_INTEGER(camera_x)
	VM_PARAM_INTEGER(metaroom_id)

	caos_assert(metaroom_id >= 0);
	MetaRoom* m = world.map->getMetaRoom(metaroom_id);
	if (!m)
		return; // DS does 'meta 0 -1 -1 0' in !map.cos for some stupid reason

	int camerax = camera_x;
	if (camerax == -1)
		camerax = m->x();
	int cameray = camera_y;
	if (cameray == -1)
		cameray = m->y();

	vm->getCamera()->goToMetaRoom(metaroom_id, camerax, cameray, (cameratransition)transition);
}

/**
 META (integer)
 %status maybe

 Returns the metaroom the current camera is looking at.
*/
void v_META(caosVM* vm) {
	if (vm->getCamera()->getMetaRoom())
		vm->result.setInt(vm->getCamera()->getMetaRoom()->id);
	else // this is a hack for empathic vendor.cos in DS, which uses META before it's setup
		// TODO: work out what we should do instead of the hack
		vm->result.setInt(-1);
}

/**
 CMRT (command) pan (integer)
 %status maybe

 Focuses the current camera on the TARG agent.

 Set pan to 0 for no panning (jump), 1 for smooth scrolling if in the same metaroom, or 2 
 for smooth scrolling if the given location is already visible.
*/
void c_CMRT(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pan)

	valid_agent(vm->targ);

	MetaRoom* r = world.map->metaRoomAt(vm->targ->x, vm->targ->y);
	int xpos = (int)(vm->targ->x - (vm->getCamera()->getWidth() / 2.0f) + (vm->targ->getWidth() / 2.0f));
	int ypos = (int)(vm->targ->y - (vm->getCamera()->getHeight() / 2.0f) + (vm->targ->getHeight() / 2.0f));
	if (r)
		vm->getCamera()->goToMetaRoom(r->id, xpos, ypos, (cameratransition)pan); // TODO: pan okay?
}

/**
 CMRA (command) x (integer) y (integer) pan (integer)
 %status maybe

 Sets the position of the current camera so that the top left corner of the view is at the 
 given coordinates.

 Set pan to 0 for no panning (jump), or 1 for smooth scrolling if in the same metaroom.
*/
void c_CMRA(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(pan)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	vm->getCamera()->moveToGlobal(x, y, (panstyle)pan);
}

/**
 CMRP (command) x (integer) y (integer) pan (integer)
 %status maybe

 Sets the position of the current camera so that the view centers on the given coordinates.

 Set pan to 0 for no panning (jump), 1 for smooth scrolling if in the same metaroom, or 2 
 for smooth scrolling if the given location is already visible.
*/
void c_CMRP(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(pan)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	vm->getCamera()->moveToGlobal(x - (vm->getCamera()->getWidth() / 2), y - (vm->getCamera()->getHeight() / 2), (panstyle)pan);
}

/**
 CMRX (integer)
 %status maybe
 %variants c2 cv c3 sm

 Returns the X position at the center of the current camera's view.
*/
void v_CMRX(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(vm->getCamera()->getXCentre());
}

/**
 CMRY (integer)
 %status maybe
 %variants c2 cv c3 sm

 Returns the Y position at the center of the current camera's view.
*/
void v_CMRY(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(vm->getCamera()->getYCentre());
}

/**
 WNDW (integer)
 %status maybe

 Returns the width of the current camera's view.
*/
void v_WNDW(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(vm->getCamera()->getWidth());
}

/**
 WNDH (integer)
 %status maybe

 Returns the height of the current camera's view.
*/
void v_WNDH(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(vm->getCamera()->getHeight());
}

/**
 WNDB (integer)
 %status maybe

 Returns the position of the bottom edge of the current camera's view.
*/
void v_WNDB(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(vm->getCamera()->getY() + vm->getCamera()->getHeight());
}

/**
 WNDL (integer)
 %status maybe

 Returns the position of the left edge of the current camera's view.
*/
void v_WNDL(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(vm->getCamera()->getX());
}

/**
 WNDR (integer)
 %status maybe

 Returns the position of the right edge of the current camera's view.
*/
void v_WNDR(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(vm->getCamera()->getX() + vm->getCamera()->getWidth());
}

/**
 WNDT (integer)
 %status maybe

 Returns the position of the top edge of the current camera's view.
*/
void v_WNDT(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(vm->getCamera()->getY());
}

/**
 WDOW (command)
 %status stub

 Toggles full-screen mode on and off.
*/
void c_WDOW(caosVM*) {
	// TODO
}

/**
 WDOW (integer)
 %status stub

 Returns 1 if in full-screen mode, or 0 otherwise (windowed).
*/
void v_WDOW(caosVM* vm) {
	vm->result.setInt(0);
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
void c_TRCK(caosVM* vm) {
	VM_PARAM_INTEGER(transition)
	VM_PARAM_INTEGER(style)
	VM_PARAM_INTEGER(ypercent)
	VM_PARAM_INTEGER(xpercent)
	VM_PARAM_AGENT(agent)

	vm->getCamera()->trackAgent(agent, xpercent, ypercent, (trackstyle)style, (cameratransition)transition);
}

/**
 TRCK (agent)
 %status maybe

 Returns agent being tracked by the current camera, if any.
*/
void v_TRCK(caosVM* vm) {
	vm->result.setAgent(vm->getCamera()->trackedAgent());
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
void c_LINE(caosVM* vm) {
	VM_PARAM_INTEGER(stipple_off)
	VM_PARAM_INTEGER(stipple_on)
	VM_PARAM_INTEGER(b)
	VM_PARAM_INTEGER(g)
	VM_PARAM_INTEGER(r)
	VM_PARAM_INTEGER(y2)
	VM_PARAM_INTEGER(x2)
	VM_PARAM_INTEGER(y1)
	VM_PARAM_INTEGER(x1)

	valid_agent(vm->targ);
	// TODO
}

/**
 SNAX (integer) filename (string)
 %status maybe

 Determines whether or not the given image file exists in the world 
 images directory (0 or 1).
*/
void v_SNAX(caosVM* vm) {
	VM_PARAM_STRING(filename)

	vm->result.setInt(0);
	if (!world.findFile(std::string("Images/") + filename + ".s16").empty())
		vm->result.setInt(1);
	else if (!world.findFile(std::string("Images/") + filename + ".c16").empty())
		vm->result.setInt(1);
}

/**
 SCAM (command) agent (agent) part (integer)
 %status maybe

 Sets which camera to use in camera macro commands.  If 'agent' and 
 'part' are NULL, the main camera will be used.
*/
void c_SCAM(caosVM* vm) {
	VM_PARAM_INTEGER(part)
	VM_PARAM_AGENT(agent)

	if (!agent) {
		vm->camera.reset();
		return;
	}

	CompoundAgent* a = dynamic_cast<CompoundAgent*>(agent.get());
	caos_assert(a);
	CompoundPart* p = a->part(part);
	caos_assert(p);
	CameraPart* c = dynamic_cast<CameraPart*>(p);
	caos_assert(c);

	vm->camera = c->getCamera();
}

/**
 ZOOM (command) pixels (integer) x (integer) y (integer)
 %status stub

 Zoom the current camera by the given number of pixels on the given x/y 
 coordinates in the world.  If the coordinates are set to -1, the 
 current center position of the camera's view will be used.
*/
void c_ZOOM(caosVM* vm) {
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
void c_SNAP(caosVM* vm) {
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
void v_LOFT(caosVM* vm) {
	VM_PARAM_STRING(filename)

	vm->result.setInt(0); // TODO
}

/**
 BKGD (command) metaroom_id (integer) background (string) transition (integer)
 %status stub
*/
void c_BKGD(caosVM* vm) {
	VM_PARAM_INTEGER(transition)
	VM_PARAM_STRING(background)
	VM_PARAM_INTEGER(metaroomid)

	MetaRoom* metaroom = world.map->getMetaRoom(metaroomid);
	caos_assert(metaroom);

	// TODO
}

/**
 BKGD (string) metaroom_id (integer)
 %status stub
*/
void v_BKGD(caosVM* vm) {
	VM_PARAM_INTEGER(metaroomid)

	MetaRoom* metaroom = world.map->getMetaRoom(metaroomid);
	caos_assert(metaroom);

	vm->result.setString(""); // TODO
}

/**
 FRSH (command)
 %status stub
*/
void c_FRSH(caosVM*) {
	// TODO
}

/**
 SYS: CMRP (command) x (integer) y (integer)
 %status maybe
 %variants c2

 Smooth scroll the camera so that the specified coordinates are in the center of the window.
 (Yes, this differs from what the incorrect C2 documentation says.)
*/
void c_SYS_CMRP(caosVM* vm) {
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	vm->getCamera()->moveTo(x - (vm->getCamera()->getWidth() / 2), y - (vm->getCamera()->getWidth() / 2), smoothscroll);
}

/**
 SYS: CMRA (command) x (integer) y (integer)
 %status maybe
 %variants c1 c2
*/
void c_SYS_CMRA(caosVM* vm) {
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	vm->getCamera()->moveTo(x, y);
}

/**
 SYS: CAMT (command)
 %status maybe
 %variants c1 c2
*/
void c_SYS_CAMT(caosVM* vm) {
	// TODO: does CAMT behave like this in c1/c2?
	int xpos = (int)(vm->targ->x - (vm->getCamera()->getWidth() / 2.0f) + (vm->targ->getWidth() / 2.0f));
	int ypos = (int)(vm->targ->y - (vm->getCamera()->getHeight() / 2.0f) + (vm->targ->getHeight() / 2.0f));
	vm->getCamera()->moveTo(xpos, ypos);
}

/**
 SYS: WTOP (command)
 %status stub
 %variants c1 c2

 Move the main window to the front of the screen.
*/
void c_SYS_WTOP(caosVM*) {
	// TODO
}

/* vim: set noet: */
