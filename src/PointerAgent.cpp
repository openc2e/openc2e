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
#include "MetaRoom.h"
#include "Camera.h"
#include <climits>

// TODO: change imagecount?
PointerAgent::PointerAgent(std::string spritefile) : SimpleAgent(2, 1, 1, INT_MAX, spritefile, 0, 0) {
	name = "hand";
	handle_events = true;
	holdingWire = 0;
	wireOriginID = 0;

	overlay = NULL;
	overlayTimer = 0;

	hotspotx = 0;
	hotspoty = 0;
}

void PointerAgent::finishInit() {
	Agent::finishInit();

	// float relative to main camera
	// TODO: this seems hard-coded in DS (pointer attr is 0)
	attr = attr & 32;
	floatSetup();
}

void PointerAgent::carry(AgentRef a) {
	Agent::carry(a);

	int eve; if (engine.version < 3) eve = 53; else eve = 104;
	firePointerScript(eve, a); // Pointer Pickup
}

void PointerAgent::drop(AgentRef a) {
	Agent::drop(a);

	int eve; if (engine.version < 3) eve = 54; else eve = 105;
	firePointerScript(eve, a); // Pointer Drop
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

void PointerAgent::tick() {
	if (!paused) {
		if (overlayTimer > 0) {
			overlayTimer--;
		} else if (overlay) {
			delete overlay;
			overlay = NULL;
		}
	}
	SimpleAgent::tick();
}

void PointerAgent::kill() {
	// pointer agent isn't killable
}

void PointerAgent::setHotspot(int x, int y) {
	hotspotx = x;
	hotspoty = y;
}

void PointerAgent::handleEvent(SomeEvent &event) {
	int x = pointerX(), y = pointerY();
		
	if (event.type == eventmousemove) {
		moveTo(event.x + world.camera->getX() - hotspotx, event.y + world.camera->getY() - hotspoty);
		velx.setInt(event.xrel * 4);
		vely.setInt(event.yrel * 4);

		if (editAgent)
			editAgent->moveTo(pointerX(), pointerY());

		// middle mouse button scrolling
		if (event.button & buttonmiddle)
			world.camera->moveTo(world.camera->getX() - event.xrel, world.camera->getY() - event.yrel, jump);
	} else if (!handle_events) {
		/* mouse move events are (apparently - see eg C3 agent help) still handled with handle_events disabled, but nothing else */
		return;
	} else if (event.type == eventmousebuttondown) {
		// do our custom handling
		if (event.button == buttonleft) {
			CompoundPart *a = world.partAt(x, y);
			if (a /* && a->canActivate() */) { // TODO
				Agent* parent = a->getParent();

				int eve = -1;
			
				bool foundport = false;
				if (engine.version > 2) {
					for (std::map<unsigned int, boost::shared_ptr<OutputPort> >::iterator i = parent->outports.begin();
							 i != parent->outports.end(); i++) {
						// TODO: 4 is a magic number i pulled out of nooooowhere
						if (abs(i->second->x + parent->x - x) < 4 && abs(i->second->y + parent->y - y) < 4) {
							foundport = true;
							if (holdingWire == 2) {
								parent->join(i->first, wireOriginAgent, wireOriginID);
								holdingWire = 0;
								eve = 111;
							} else if (holdingWire == 0) {
								eve = 110;
								holdingWire = 1;
								wireOriginAgent = parent;
								wireOriginID = i->first;
							}
							break;
						}
					}
					if (!foundport) {
						for (std::map<unsigned int, boost::shared_ptr<InputPort> >::iterator i = parent->inports.begin();
								 i != parent->inports.end(); i++) {
							// TODO: 4 is a magic number i pulled out of nooooowhere
							if (abs(i->second->x + parent->x - x) < 4 && abs(i->second->y + parent->y - y) < 4) {
								foundport = true;
								if (holdingWire == 1) {
									if (i->second->source) {
										eve = 114;
									} else {
										wireOriginAgent->join(wireOriginID, parent, i->first);
										holdingWire = 0;
										eve = 111;
									}
								} else if (holdingWire == 0) {
									if (i->second->source) {
										eve = 112;
										holdingWire = 1;
										wireOriginAgent = i->second->source;
										wireOriginID = i->second->sourceid;
										i->second->source->outports[i->second->sourceid]->dests.remove(std::pair<AgentRef, unsigned int>(parent, i->first));
										i->second->source.clear();
									} else {
										eve = 110;
										holdingWire = 2;
										wireOriginAgent = parent;
										wireOriginID = i->first;
									}
								}
								break;
							}
						}
					}
				}

				if (!foundport) {
					if (holdingWire) {
						holdingWire = 0;
						eve = 113;
					} else {
						// if the agent isn't paused, tell it to handle a click
						int scriptid = a->handleClick(x - a->x - parent->x, y - a->y - parent->y);
						if (scriptid != -1) {
							// fire the script
							// _p1_ is id of part for button clicks, according to Edynn code
							// TODO: should _p1_ always be set? :)
							if (!parent->paused) parent->queueScript(scriptid, world.hand(), (int)a->id);

							// annoyingly queueScript doesn't reliably tell us if it did anything useful.
							// TODO: work out the mess which is queueScript's return values etc
							if (!parent->findScript(scriptid)) return;

							// fire the associated pointer script too, if necessary
							// TODO: fuzzie has no idea if this code is remotely correct
							if (engine.version < 3) {
								eve = 50;
							} else {
								eve = 101; // Pointer Activate 1
							}
							switch (scriptid) {
								case 0: eve += 2; break; // deactivate
								case 1: break; // activate 1
								case 2: eve += 1; break; // activate 2
								default: return;
							}
						}
					}
				}

				if (eve != -1) firePointerScript(eve, a->getParent());
			} else if (engine.version > 2) {
				if (holdingWire) {
					holdingWire = 0;
					queueScript(113, 0);
				} else {
					queueScript(116, 0); // Pointer Clicked Background
				}
			}
		} else if (event.button == buttonright) {
			if (editAgent) { 
				editAgent.clear();
				return;
			}

			if (world.paused) return; // TODO: wrong?
							
			// picking up and dropping are implictly handled by the scripts (well,
			// messages) 4 and 5	TODO: check if this is correct behaviour, one issue
			// is that this isn't instant, another is the messages might only be
			// fired in c2e when you use MESG WRIT, in which case we'll need to
			// manually set carrying to NULL and a here, respectively - fuzzie
			if (carrying) {
				if (engine.version == 1) {
					// ensure there's a room to drop into, in C1
					MetaRoom* m = world.map.metaRoomAt(x, y);
					if (!m) return;
					shared_ptr<Room> r = m->nextFloorFromPoint(x, y);
					if (!r) return;

					carrying->queueScript(5, this); // drop

					return;
				}
			
				bool allowdrop = true;

				if (engine.version == 2) {
					/* check dropstatus for the room we're dropping into */
					MetaRoom* m = world.map.metaRoomAt(carrying->x, carrying->y);
					if (m) {
						shared_ptr<Room> r = m->roomAt(carrying->x, carrying->y);
						if (r) {
							if (r->dropstatus == 0) allowdrop = false; // Never
							else if (r->dropstatus == 1) { // Above-Floor
								float floory = r->floorYatX(carrying->x);
								if (floory < (carrying->y + carrying->getHeight())) {
									// snap to above the floor
									// TODO: snapping to above the floor isn't very well thought-out (eg, this will drop into vehicles)
									carrying->y = floory - carrying->getHeight();
									// try pushing upwards for a while if we're not validly in the room system
									while (!carrying->validInRoomSystem() && carrying->y - floory + carrying->getHeight() < 50) {
										carrying->y--;
									}
								}
							}
						} else allowdrop = false;
					} else allowdrop = false;
				}

				/* deny drops which result in agents lying outside the room system */
				if (allowdrop && carrying->suffercollisions()) {
					allowdrop = carrying->validInRoomSystem();
				}

				if (allowdrop) {
					carrying->queueScript(5, this); // drop

					// TODO: is this the correct check?
					if (carrying->sufferphysics() && carrying->suffercollisions()) {
						// TODO: do this in the pointer agent?
						carrying->velx.setFloat(velx.getFloat());
						carrying->vely.setFloat(vely.getFloat());
					}
				} else {
					if (engine.version == 2) {
						if (overlay) delete overlay;
						overlay = new DullPart(this, 0, "syst", 17, 0, 0, 1);
						overlayTimer = 8;

						// play 'deny drop' sound
						playAudio("excl", true, false); // TODO: should this be controlled?
					} else {
						// TODO: drop deny animation/etc for c2e
					}
				}
			} else {
				Agent *a = world.agentAt(x, y, false, true);
				if (a) {
					a->queueScript(4, this); // pickup
				}
			}
		} else if (event.button == buttonmiddle) {
			std::vector<shared_ptr<Room> > rooms = world.map.roomsAt(x, y);
			if (rooms.size() > 0) std::cout << "Room at cursor is " << rooms[0]->id << std::endl;
			Agent *a = world.agentAt(x, y, true);
			if (a)
				std::cout << "Agent under mouse is " << a->identify();
			else
				std::cout << "No agent under cursor";
			std::cout << std::endl;
		}
	}
}

/* vim: set noet: */
