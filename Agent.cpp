/*
 *  Agent.cpp
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

#include "Agent.h"
#include "World.h"
#include "physics.h"
#include <iostream>
#include <sstream>
#include "caosVM.h"
#include "SDLBackend.h"

Agent::Agent(unsigned char f, unsigned char g, unsigned short s, unsigned int p) :
  visible(true), family(f), genus(g), species(s), zorder(p), vm(0), timerrate(0) {
	velx.setFloat(0.0f);
	vely.setFloat(0.0f);
	accg = 0.3f;
	range = 500;
	sufferphysics = false;
	x = 0.0f; y = 0.0f;
	clac[0] = 1; // activate 1
	clik = -1;
	
	// AgentRef stuff
	self.ref = this;
	self.next = self.prev = &self;
	
	dying = false;
	unid = -1;

	soundslot = 0;
	displaycore = false;

	floatingx = floatingy = 0;

	zorder_iter = world.zorder.insert(this);
	agents_iter = world.agents.insert(++world.agents.begin(), this);

	cr_can_push = cr_can_pull = cr_can_stop = cr_can_hit = cr_can_eat = cr_can_pickup = false; // TODO: check this

	emitca_index = -1; emitca_amount = 0.0f;
}

void Agent::zotstack() {
	for (std::list<caosVM *>::iterator i = vmstack.begin(); i != vmstack.end(); i++) {
		world.freeVM(*i);
	}
	vmstack.clear();
}


void Agent::moveTo(float _x, float _y) {
	x = _x; y = _y;
}

script *Agent::findScript(unsigned short event) {
	return world.scriptorium.getScript(family, genus, species, event);
}

bool Agent::fireScript(unsigned short event, Agent *from) {
	if (dying) return false;

	script *s = findScript(event);
	if (!s) return false;
	if (!vm) vm = world.getVM(this);
	if (vm->fireScript(s, (event == 9), from)) {
		zotstack();
		return true;
	} else if (vm->stopped()) {
		world.freeVM(vm);
		vm = 0;
	}
	return false;
}

void Agent::queueScript(unsigned short event, AgentRef from, caosVar p0, caosVar p1) {
	if (dying) return;

	world.queueScript(event, this, from, p0, p1);
}

void Agent::handleClick(float clickx, float clicky) {
	if (clik != -1) {
		// TODO: handle CLIK
	} else if (clac[0] != -1) {
		queueScript(clac[0], (Agent *)world.hand());
	}
}

void Agent::positionAudio(SoundSlot *slot) {
	assert(slot);

	float xoffset = x - world.camera.getXCentre();
	float yoffset = y - world.camera.getYCentre();
	int distance = (sqrt(xoffset*xoffset + yoffset*yoffset) * 1000) / 255;
	int angle;
	if (xoffset == 0) {
		if (yoffset > 0) angle = 90;
		else angle = 270;
	} else {
		angle = (atanf(abs(yoffset) / abs(xoffset)) / (2*M_PI)) * 360;
		if (xoffset < 0) angle += 180;
		if (yoffset < 0) angle += 90;
	}

	slot->adjustPanning(angle, distance);
}

void Agent::tick() {
	if (dying) return;

	if (emitca_amount != 0.0f) {
		Room *r = world.map.roomAt(x, y);
		if (r) {
			r->ca[emitca_index] += emitca_amount;
			if (r->ca[emitca_index] <= 0.0f) r->ca[emitca_index] = 0.0f;
			else if (r->ca[emitca_index] >= 1.0f) r->ca[emitca_index] = 1.0f;
		}
	}

	if (soundslot) positionAudio(soundslot);

	if (sufferphysics) {
		float newvely = vely.floatValue + accg;
		float destx = x + velx.floatValue;
		float desty = y + newvely;
		//std::cout << x << ", " << y << ": " << destx << ", " << desty << "! " << accg << "\n";
		Room *r1 = world.map.roomAt((unsigned int)x, (unsigned int)y);
		if (!r1) {
//			std::cout << "not doing physics on agent " << identify() << ", outside room system!\n";
		}
		Room *r2 = world.map.roomAt((unsigned int)destx + getWidth(), (unsigned int)desty + getHeight());
		Room *r3 = world.map.roomAt((unsigned int)destx, (unsigned int)desty);

		//if (r1 && (r1 == r2) && (r2 == r3)) {
			// If the object is already in a room, and the destination point is also in the room,
			// for now we don't bother checking. We *should*, because (crazily) it could take a
			// path out of the room and back in again, once we're doing more than dumb velocity.
		//	vely.setFloat(newvely);
		//	moveTo(destx, desty);
		/*} else */ if (r1) { // if we *are* actually in a room
			// Otherwise, check the motion pixel-by-pixel. Oh boy.
		
			bool moved = false, collided = false;

			int collidedirection = -1;

			int ix = x, iy = y, idestx = destx, idesty = desty;
			int dx = (ix < idestx ? 1 : -1);
			int dy = (iy < idesty ? 1 : -1);

			Room *room[4];
			Room *oroom[4];
			for (unsigned int i = 0; i < 4; i++) room[i] = 0;
			
			while (ix != idestx || iy != idesty) {
				// We just alternate here. There's probably a more
				// accurate method, but meh

				bool movedy = false, movedx = false;
				
				if (iy != idesty) {
					movedy = true;
					iy += dy;
				}
	
				if (ix != idestx) {
					movedx = true;
					ix += dx;
				}

				for (unsigned int i = 0; i < 4; i++) oroom[i] = room[i];

				if (room[0]) {
					if (!room[0]->containsPoint(ix, iy))
						room[0] = 0;	
				} else room[0] = 0;
				if (!room[0]) room[0] = world.map.roomAt(ix, iy);
					
				if (room[1]) {
					if (!room[1]->containsPoint(ix + getWidth(), iy + getHeight()))
						room[1] = 0;
				} else room[1] = 0;
				if (!room[1]) room[1] = world.map.roomAt(ix + getWidth(), iy + getHeight());

				if (room[2]) {
					if (!room[2]->containsPoint(ix, iy + getHeight()))
						room[2] = 0;
				} else room[2] = 0;
				if (!room[2]) room[2] = world.map.roomAt(ix, iy + getHeight());

				if (room[3]) {
					if (!room[3]->containsPoint(ix + getWidth(), iy))
						room[3] = 0;
				} else room[3] = 0;
				if (!room[3]) room[3] = world.map.roomAt(ix + getWidth(), iy);

				bool cannotmove = false;
				cannotmove = (!room[0]) || (!room[1]) || (!room[2]) || (!room[3]);
			
				if (cannotmove) {
					if (movedy) {
						iy -= dy;
						collided = true; // .. but only if moved is true
					}
					if (movedx) {
						ix -= dx;
						collided = true; // .. again
					}

					if (!moved) break;

					/*
					 * now we need to work out which direction of wall we collided against
					 *
					 * possibilities:
					 *
					 * we may have collided with the ceiling of the old room1/room4
					 * the floor of the old room2/room3
					 * the left wall of the old room1/room3
					 * or the right wall of the old room2/room4
					 *
					 * todo: we can use dx/dy to work out whether there's any possibility of having collided with walls
					 * (ie, if we're going in the opposite direction)
					 */
					for (unsigned int i = 0; i < 4; i++) {
						for (unsigned int j = 0; j < 2; j++) {
							Room *r = 0;
							Line l;
							switch (i) {
								case 0:
									if (j == 0) r = oroom[0]; else r = oroom[3];
									l = r->top;
									break;
								case 1:
									if (j == 0) r = oroom[1]; else r = oroom[2];
									l = r->bot;
									break;
								case 2:
									if (j == 0) r = oroom[0]; else r = oroom[2];
									l = r->left;
									break;
								case 3:
									if (j == 0) r = oroom[1]; else r = oroom[3];
									l = r->right;
									break;
							}

							bool foundpoint = false;
							for (int k = 0; k < 4; k++) {
								Point p;

								switch (k) {
									case 0: p = Point(ix, iy); break;
									case 1: p = Point(ix + getWidth(), iy); break;
									case 2: p = Point(ix, iy + getHeight()); break;
									case 3: p = Point(ix + getWidth(), iy + getHeight()); break;
								}

								/* std::cout << "testing point (" << p.x << ", " << p.y << ") with line ";
								std::cout << "(" << l.getStart().x << ", " << l.getStart().y << ") to ";
								std::cout << "(" << l.getEnd().x << ", " << l.getEnd().y << ")" << std::endl; */
								
								if (l.containsPoint(p)) {
									// std::cout << "found point!" << std::endl;
									foundpoint = true;
									break;
								}
							}
					
							if (foundpoint) {
								switch (i) {
									case 0: collidedirection = 2; break; // _UP_
									case 1: collidedirection = 3; break; // DOWN
									case 2: collidedirection = 0; break; // LEFT
									case 3: collidedirection = 1; break; // RGHT
								}
								
								i = 4; break; // break out of both loops
							}
						}
					}

					if (collidedirection == -1)
						std::cout << "huh? " << identify() << " collided with a wall, but we can't work out which!" << std::endl;

					collided = true;
					break;
				}
				
				moved = true;
			}
			
			vely.setFloat(newvely);
			
			if (moved) { // if we did actually try and go somewhere
				// TODO: this totally destroys floatness!
				x = ix;
				y = iy;
				
				if (collided) {
					lastcollidedirection = collidedirection;
					queueScript(6); // TODO: include this?
					if (vm) vm->setVariables(velx, vely);
					vely.setFloat(0);
				}
			}
		}
	} else if (!floatable) {
		if (vely.hasDecimal())
			y = y + vely.getFloat();
		if (velx.hasDecimal())
			x = x + velx.getFloat();
	} else { // handle floating..
		if (vely.hasDecimal())
			floatingy = floatingy + vely.getFloat();
		if (velx.hasDecimal())
			floatingx = floatingx + velx.getFloat();
	}

	if (floatable) {
		// TODO: should probably be done at the end of the tick after all agents have been repositioned
		// (or be a child of the parent agent, so it always gets set after it)
		
		if (floatingagent) {
			x = floatingagent->x + floatingx;
			y = floatingagent->y + floatingy;
		} else {
			x = world.camera.getX() + floatingx;
			y = world.camera.getY() + floatingy;
		}
	}

	if (timerrate) {
		tickssincelasttimer++;
		if (tickssincelasttimer == timerrate) {
			queueScript(9); // TODO: include this?
			tickssincelasttimer = 0;
		}
	}

	if (vm) vmTick();
}

void Agent::vmTick() {
	assert(vm);

	if (!vm->timeslice) vm->timeslice = 5;

	while (vm && vm->timeslice && !vm->isBlocking() && !vm->stopped()) {
		vm->tick();
		if (vm && vm->stopped()) {
			world.freeVM(vm);
			vm = NULL;
		}
	}
	if (vm) vm->timeslice = 0;
	
	if (!vm && !vmstack.empty()) {
		vm = vmstack.front();
		vmstack.pop_front();
	}
}

Agent::~Agent() {
	world.agents.erase(agents_iter);
	if (vm)
		world.freeVM(vm);
	zotstack();
	zotrefs();
}

void Agent::kill() {
	assert(!dying);
	dying = true; // what a world, what a world...
	if (vm) {
		vm->stop();
		world.freeVM(vm);
		vm = 0;
	}
	zotstack();
	zotrefs();
	if (unid != -1)
		world.freeUNID(unid);
	world.zorder.erase(zorder_iter);
	world.killqueue.push_back(this);
}

void Agent::zotrefs() {
	while (self.next != &self)
		self.next->clear();
}

void Agent::setZOrder(unsigned int z) {
	if (dying) return;
	world.zorder.erase(zorder_iter);
	zorder = z;
	zorder_iter = world.zorder.insert(this);
}

int Agent::getUNID() {
	  if (unid != -1)
			  return unid;
	  return unid = world.getUNID(this);
}

std::string Agent::identify() const {
	std::ostringstream o;
	o << (int)family << " " << (int)genus << " " << species << " ";
	const std::string n = world.catalogue.getAgentName(family, genus, species);
	if (n.size())
		o << "(" + n + ")";
	if (unid != -1)
		o << " unid " << unid;
	else
		o << " (no unid assigned)";
	return o.str();
}

bool agentzorder::operator ()(const Agent *s1, const Agent *s2) const {
	return s1->zorder < s2->zorder;
}

void Agent::pushVM(caosVM *newvm) {
	assert(newvm);
	if (vm)
		vmstack.push_front(vm);
	vm = newvm;
}

void Agent::stopScript() {
	zotstack();
	vm->stop();
}

void Agent::setAttributes(unsigned int attr) {
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

unsigned int Agent::getAttributes() {
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
