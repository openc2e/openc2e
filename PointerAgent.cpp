/*
 *  PointerAgent.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Aug 30 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
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

#include "PointerAgent.h"
#include "openc2e.h"
#include "World.h"
#include "Engine.h"
#include "caosVM.h"
#include "Room.h"

// TODO: change imagecount?
PointerAgent::PointerAgent(std::string spritefile) : SimpleAgent(2, 1, 1, INT_MAX, spritefile, 0, 0) {
	name = "hand";
	handle_events = true;
	// TODO: verify attributes on the pointer in c2e
	attr.setInt(256); // camera shy
}

void PointerAgent::finishInit() {
	Agent::finishInit();

	// float relative to main camera
	attr.setInt(attr.getInt() & 32);
	floatSetup();
}

// TODO: this should have a queueScript equiv too
void PointerAgent::firePointerScript(unsigned short event, Agent *src) {
	assert(src); // TODO: I /think/ this should only be called by the engine..
	shared_ptr<script> s = src->findScript(event);
	if (!s && engine.version < 3) { // TODO: are we sure this doesn't apply to c2e?
		s = findScript(event); // TODO: we should make sure this actually belongs to the pointer agent and isn't a fallback, maybe
	}
	if (!s) return;
	if (!vm) vm = world.getVM(this);
	if (vm->fireScript(s, false, src)) { // TODO: should FROM be src?
		vm->setTarg(this);
		zotstack();
	}
}

void PointerAgent::physicsTick() {
	// TODO: this is a hack, which does nothing, because we set a velocity in main() but also move the cursor manually
}

void PointerAgent::kill() {
	// pointer agent isn't killable
}

void PointerAgent::handleEvent(SomeEvent &event) {
	if (!handle_events) return;
	if (event.type == eventmousemove) {
		moveTo(event.x + world.camera.getX(), event.y + world.camera.getY());
		velx.setInt(event.xrel * 4);
		vely.setInt(event.yrel * 4);

		// middle mouse button scrolling
		if (event.button & buttonmiddle)
			world.camera.moveTo(world.camera.getX() - event.xrel, world.camera.getY() - event.yrel, jump);
	} else if (event.type == eventmousebuttondown) {
		// do our custom handling
		if (event.button == buttonleft) {
			CompoundPart *a = world.partAt(x, y);
			if (a /* && a->canActivate() */) { // TODO
				// if the agent isn't paused, tell it to handle a click
				if (!a->getParent()->paused)
					a->handleClick(x - a->x - a->getParent()->x, y - a->y - a->getParent()->y);

				// TODO: not sure how to handle the following properly, needs research..
				int eve;
				if (engine.version < 3) {
					eve = 50;
				} else {
					eve = 101;
				}
				firePointerScript(eve, a->getParent()); // Pointer Activate 1
			} else if (engine.version > 2)
				queueScript(116, 0); // Pointer Clicked Background
		} else if (event.button == buttonright) {
			if (world.paused) return; // TODO: wrong?
							
			// picking up and dropping are implictly handled by the scripts (well,
			// messages) 4 and 5	TODO: check if this is correct behaviour, one issue
			// is that this isn't instant, another is the messages might only be
			// fired in c2e when you use MESG WRIT, in which case we'll need to
			// manually set carrying to NULL and a here, respectively - fuzzie
			if (carrying) {
				// TODO: c1 support - these attributes are invalid for c1
				if (!carrying->suffercollisions() || (carrying->validInRoomSystem() || engine.version == 1)) {
					carrying->queueScript(5, this); // drop

					int eve; if (engine.version < 3) eve = 54; else eve = 105;
					firePointerScript(eve, carrying); // Pointer Drop

					// TODO: is this the correct check?
					if (carrying->sufferphysics() && carrying->suffercollisions()) {
						// TODO: do this in the pointer agent?
						carrying->velx.setFloat(velx.getFloat());
						carrying->vely.setFloat(vely.getFloat());
					}
				} else {
					// TODO: some kind of "fail to drop" animation/sound?
				}
			} else {
				Agent *a = world.agentAt(x, y, false, true);
				if (a) {
					a->queueScript(4, this); // pickup

					int eve; if (engine.version < 3) eve = 53; else eve = 104;
					firePointerScript(eve, a); // Pointer Pickup
				}
			}
		} else if (event.button == buttonmiddle) {
			std::vector<shared_ptr<Room> > rooms = world.map.roomsAt(event.x + world.camera.getX(), event.y + world.camera.getY());
			if (rooms.size() > 0) std::cout << "Room at cursor is " << rooms[0]->id << std::endl;
			Agent *a = world.agentAt(event.x + world.camera.getX(), event.y + world.camera.getY(), true);
			if (a)
				std::cout << "Agent under mouse is " << a->identify();
			else
				std::cout << "No agent under cursor";
			std::cout << std::endl;
		}
	}
}

/* vim: set noet: */
