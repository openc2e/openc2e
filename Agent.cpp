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
#include <iostream>
#include <sstream>
#include "caosVM.h"
#include "SDLBackend.h"
#include <boost/format.hpp>

void Agent::core_init() {
	initialized = false;
	lifecount = 0;
}

Agent::Agent(unsigned char f, unsigned char g, unsigned short s, unsigned int p) :
  visible(true), family(f), genus(g), species(s), zorder(p), vm(0), timerrate(0)
{
	core_init();
	lastScript = -1;
	initialized = true;
	velx.setFloat(0.0f);
	vely.setFloat(0.0f);
	accg = 0.3f;
	aero = 0;
	elas = 0;
	perm = 50; // TODO: correct default?
	range = 500;
	sufferphysics = false; falling = false;
	x = 0.0f; y = 0.0f;
	clac[0] = 1; // activate 1
	clik = -1;
	
	dying = false;
	unid = -1;

	soundslot = 0;
	paused = displaycore = false;

	floatable = false; setAttributes(0);
	cr_can_push = cr_can_pull = cr_can_stop = cr_can_hit = cr_can_eat = cr_can_pickup = false; // TODO: check this
	imsk_key_down = imsk_key_up = imsk_mouse_move = imsk_mouse_down = imsk_mouse_up = imsk_mouse_wheel = imsk_translated_char = false;

	emitca_index = -1; emitca_amount = 0.0f;
}

void Agent::finishInit() {
	// shared_from_this() can only be used if these is at least one extant
	// shared_ptr which owns this
	world.agents.push_front(boost::shared_ptr<Agent>(this));
	agents_iter = world.agents.begin();

	if (findScript(10))
		queueScript(10); // constructor
}

void Agent::zotstack() {
	for (std::list<caosVM *>::iterator i = vmstack.begin(); i != vmstack.end(); i++) {
		world.freeVM(*i);
	}
	vmstack.clear();
}

void Agent::moveTo(float _x, float _y, bool force) {
	if (carriedby && !force) return;
	
	// TODO: what if we move while being carried? doomy explosions ensue, that's what!
	float xoffset = _x - x;
	float yoffset = _y - y;		

	x = _x; y = _y;

	for (std::vector<AgentRef>::iterator i = floated.begin(); i != floated.end(); i++) {
		assert(*i);
		(*i)->moveTo((*i)->x + xoffset, (*i)->y + yoffset);
	}

	adjustCarried();
}

void Agent::floatTo(AgentRef a) {
	std::vector<AgentRef>::iterator i = std::find(floated.begin(), floated.end(), a);
	assert(i == floated.end()); // loops are bad, mmkay

	if (floatable) floatRelease();
	floatingagent = a;
	if (floatable) floatSetup();
}

void Agent::floatTo(float x, float y) {
	if (floatingagent) {
		moveTo(floatingagent->x + x, floatingagent->y + y);
	} else {
		moveTo(world.camera.getX() + x, world.camera.getY() + y);
	}
}

void Agent::floatSetup() {
	if (floatingagent)
		floatingagent->addFloated(this);
	else
		world.camera.addFloated(this);
}

void Agent::floatRelease() {
	if (floatingagent) {
		floatingagent->delFloated(this);
	} else
		world.camera.delFloated(this);
}

void Agent::addFloated(AgentRef a) {
	assert(a);
	assert(a != floatingagent); // loops are bad, mmkay
	floated.push_back(a);
}

void Agent::delFloated(AgentRef a) {
	assert(a);
	std::vector<AgentRef>::iterator i = std::find(floated.begin(), floated.end(), a);
	//if (i == floated.end()) return;
	assert(i != floated.end());
	floated.erase(i);
}

shared_ptr<script> Agent::findScript(unsigned short event) {
	return world.scriptorium.getScript(family, genus, species, event);
}

#include "PointerAgent.h"
#include "Creature.h"
bool Agent::fireScript(unsigned short event, Agent *from) {
	if (dying) return false;

	Creature *c;
	if (event <= 3 || event == 4 || event == 12)
		c = dynamic_cast<Creature *>(from);

	switch (event) {
		case 0: // deactivate
			if (c && !cr_can_stop) return false;
			break;
		case 1: // activate 1
			if (c && !cr_can_push) return false;
			break;
		case 2: // activate 2
			if (c && !cr_can_pull) return false;
			break;
		case 3: // hit
			if (c && !cr_can_hit) return false;
			break;
		case 4: // pickup
			if (c && !cr_can_pickup) return false;
			if (!from) return false;
			if (from == world.hand()) {
				if (!mouseable) return false;
			} else if (!c) {
				if (!carryable) return false;
			}
			from->carry(this); // TODO: correct behaviour?
			break;
		case 5: // drop
			if (!from) return false;
			if (from != carriedby) return false;
			from->dropCarried(); // TODO: correct?
			break;
		case 12: // eat
			if (c && !cr_can_eat) return false;
			break;
		case 13: // hold hands with pointer
			if (c) {
				// TODO
			}
			break;
		case 14: // stop holding hands with pointer
			if (c) {
				// TODO
			}
			break;
		case 92: // TODO: hack for 'UI Mouse Down' event - we need a real event system!
			std::cout << "faking event 92 on " << identify() << std::endl;
			CompoundPart *p = world.partAt(world.hand()->x, world.hand()->y);
			if (!p || p->getParent() != this) // if something is horridly broken here, return
				return false; // was caos_assert(p && p->getParent() == this);
			p->handleClick(world.hand()->x - p->x - p->getParent()->x, world.hand()->y - p->y - p->getParent()->y);
			// TODO: we're [obviously] missing firing the pointer script here, but it's a hack for now
			break;
	}

	shared_ptr<script> s = findScript(event);
	if (!s) return false;
	
	bool madevm = false;
	if (!vm) { madevm = true; vm = world.getVM(this); }
	
	if (vm->fireScript(s, (event == 9), from)) {
		lastScript = event;
		zotstack();
		return true;
	} else if (madevm) {
		world.freeVM(vm);
		vm = 0;
	}

	return false;
}

bool Agent::queueScript(unsigned short event, AgentRef from, caosVar p0, caosVar p1) {
	if (dying) return false;

	// only bother firing the event if either it exists, or it's one with engine code attached
	// TODO: why don't we do the engine checks/etc here?
	switch (event) {
		default:
			if (!findScript(event)) return false;

		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 12:
		case 13:
		case 14:
		case 92:
			world.queueScript(event, this, from, p0, p1);
	}
	
	return true;
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
		angle = (atanf(fabs(yoffset) / fabs(xoffset)) / (2*M_PI)) * 360;
		if (xoffset < 0) angle += 180;
		if (yoffset < 0) angle += 90;
	}

	slot->adjustPanning(angle, distance);
}

Point Agent::boundingBoxPoint(unsigned int n) {
	return boundingBoxPoint(n, Point(x, y), getWidth(), getHeight());
}

Point Agent::boundingBoxPoint(unsigned int n, Point in, unsigned int w, unsigned int h) {
	Point p;
	
	switch (n) {
		case 0: // left
			p.x = in.x;
			p.y = in.y + (h / 2.0f);
			break;

		case 1: // right
			p.x = in.x + w;
			p.y = in.y + (h / 2.0f);
			break;

		case 2: // top
			p.x = in.x + (w / 2.0f);
			p.y = in.y;
			break;

		case 3: // bottom
			p.x = in.x + (w / 2.0f);
			p.y = in.y + h;
			break;

		default:
			throw creaturesException("Agent::boundingBoxPoint got unknown direction");
	}

	return p;
}

bool Agent::validInRoomSystem() {
	return validInRoomSystem(Point(x, y), getWidth(), getHeight(), perm);
}

bool Agent::validInRoomSystem(Point p, unsigned int w, unsigned int h, int testperm) {
	for (unsigned int i = 0; i < 4; i++) {
		Point src, dest;
		switch (i) {
			case 0: src = boundingBoxPoint(3, p, w, h); dest = boundingBoxPoint(0, p, w, h); break; // bottom to left
			case 1: src = boundingBoxPoint(3, p, w, h); dest = boundingBoxPoint(1, p, w, h); break; // bottom to right
			case 2: src = boundingBoxPoint(2, p, w, h); dest = boundingBoxPoint(0, p, w, h); break; // top to left
			case 3: src = boundingBoxPoint(2, p, w, h); dest = boundingBoxPoint(1, p, w, h); break; // top to right
		}
		float srcx = src.x, srcy = src.y;
		
		Room *ourRoom = world.map.roomAt(srcx, srcy);
		if (!ourRoom) return false;

		unsigned int dir; Line wall;
		world.map.collideLineWithRoomSystem(src, dest, ourRoom, src, wall, dir, testperm);

		if (src != dest) return false;
	}

	return true;
}

void Agent::physicsTick() {
	falling = false;

	if (carriedby) return; // We don't move when carried, so what's the point?

	if (x == 0 && y == 0) return; // TODO: is this correct behaviour? :P

	// set destination point based on velocities
	float destx = x + velx.getFloat();
	float desty = y + vely.getFloat();

	if (sufferphysics) {
		// increase speed according to accg
		// TODO: should we be changing vely first, instead of after a successful move (below)?
		desty += accg;
	}
	
	if (suffercollisions) {
		float lastdistance = 1000000.0f;
		bool collided = false;
		Line wall; // only valid when collided
		unsigned int collidedirection; // only valid when collided
		Point bestmove;

		// iterate through all four points of the bounding box
		for (unsigned int i = 0; i < 4; i++) {
			// this mess is because we want to start with the bottom point - DOWN (3) - before the others, for efficiency
			Point src = boundingBoxPoint((i == 0) ? 3 : i - 1);

			// store values
			float srcx = src.x, srcy = src.y;
			
			Room *ourRoom = world.map.roomAt(srcx, srcy);
			if (!ourRoom) {
				if (!displaycore)
					unhandledException(boost::str(boost::format("out of room system at (%f, %f)") % srcx % srcy), false);
				displaycore = true;
				return; // out of room system
			}
			
			Point dest(destx + (srcx - x), desty + (srcy - y));
			unsigned int local_collidedirection;
			Line local_wall;
		
			// this changes src to the point at which we end up
			bool local_collided = world.map.collideLineWithRoomSystem(src, dest, ourRoom, src, local_wall, local_collidedirection, perm);

			float dist;
			if (src.x == srcx && src.y == srcy)
				dist = 0.0f;
			else {
				float xdiff = src.x - srcx;
				float ydiff = src.y - srcy;
				dist = xdiff*xdiff + ydiff*ydiff;
			}

			if (dist >= lastdistance) {
				assert(i != 0); // this had better not be our first collision!
				continue; // further away than a previous collision
			}

			lastdistance = dist;
			bestmove.x = x + (src.x - srcx);
			bestmove.y = y + (src.y - srcy);
			collidedirection = local_collidedirection;
			wall = local_wall;
			collided = local_collided;

			if (dist == 0.0f)
				break; // no point checking any more, is there?
		}	

		// *** do actual movement
		if (lastdistance != 0.0f) {	
			moveTo(bestmove.x, bestmove.y);
		
			if (collided) {
				lastcollidedirection = collidedirection;
				queueScript(6, 0, velx, vely); // TODO: include this? .. we need to include SOMETHING, c3 ball checks for <3

				if (elas != 0) {
					if (wall.getType() == HORIZONTAL) {
						vely.setFloat(-vely.getFloat());
					} else if (wall.getType() == VERTICAL) {
						velx.setFloat(-velx.getFloat());
					} else {
						// line starts always have a lower x value than the end
						float xdiff = wall.getEnd().x - wall.getStart().x;
						float ydiff = wall.getEnd().y - wall.getStart().y;
						float fvelx = velx.getFloat(), fvely = vely.getFloat();
					
						// calculate input/slope angles
						double inputangle;
						if (fvelx == 0.0f) {
							if (fvely > 0.0f)
								inputangle = M_PI / 2.0;
							else
								inputangle = 3 * (M_PI / 2.0);
						} else {
							inputangle = atan(fvely / fvelx);
						}
						double slopeangle = atan(-ydiff / xdiff); // xdiff != 0 because wall isn't vertical

						// calculate output angle
						double outputangle = slopeangle + (slopeangle - inputangle) + M_PI;

						// turn back into component velocities
						double vectorlength = sqrt(fvelx*fvelx + fvely*fvely);
						float xoutput = cos(outputangle) * vectorlength;
						float youtput = sin(outputangle) * vectorlength;

						velx.setFloat(xoutput);
						vely.setFloat(-youtput);
					}

					if (elas != 100.0f) {
						velx.setFloat(velx.getFloat() * (elas / 100.0f));
						vely.setFloat(vely.getFloat() * (elas / 100.0f));
					}
				} else				
					vely.setFloat(0);
			} else if (sufferphysics && accg != 0) {
				falling = true; // TODO: icky
				vely.setFloat(vely.getFloat() + accg);
			}
		} else { velx.setFloat(0); vely.setFloat(0); } // TODO: correct?
	} else {
		if (vely.hasDecimal() || velx.hasDecimal())
			moveTo(destx, desty);
	}

	if (sufferphysics && (aero != 0)) {
		// reduce speed according to AERO
		// TODO: aero should be an integer!
		velx.setFloat(velx.getFloat() - (velx.getFloat() * (aero / 100.0f)));
		vely.setFloat(vely.getFloat() - (vely.getFloat() * (aero / 100.0f)));
	}
}

void Agent::tick() {
	LifeAssert la(this);
	if (dying) return;
	
	if (soundslot) positionAudio(soundslot);

	if (paused) return;

	if (emitca_index != -1 && emitca_amount != 0.0f) {
		assert(0 <= emitca_index && emitca_index <= 19);
		Room *r = world.map.roomAt(x, y);
		if (r) {
			r->ca[emitca_index] += emitca_amount;
			if (r->ca[emitca_index] <= 0.0f) r->ca[emitca_index] = 0.0f;
			else if (r->ca[emitca_index] >= 1.0f) r->ca[emitca_index] = 1.0f;
		}
	}

	physicsTick();
	if (dying) return; // in case we were autokilled

	if (timerrate) {
		tickssincelasttimer++;
		if (tickssincelasttimer == timerrate) {
			queueScript(9); // TODO: include this?
			tickssincelasttimer = 0;
		}
	}

	if (vm) vmTick();
}

void Agent::unhandledException(std::string info, bool wasscript) {
	// TODO: do something with this? empty the stack?
	if (world.autokill) {
		kill();
		if (wasscript)
			std::cerr << identify() << " was autokilled during script " << lastScript << " due to: " << info << std::endl;
		else
			std::cerr << identify() << " was autokilled due to: " << info << std::endl;
	} else {
		if (wasscript)
			std::cerr << identify() << " caused an exception during script " << lastScript << ": " << info << std::endl;
		else
			std::cerr << identify() << " caused an exception: " << info << std::endl;
	}
}

void Agent::vmTick() {
	assert(vm);
	LifeAssert la(this);

	if (!vm->timeslice) vm->timeslice = 5;

	while (vm && vm->timeslice && !vm->isBlocking() && !vm->stopped()) {
		try {
			vm->tick();
		} catch (invalidAgentException &e) {
			// try letting the exception script handle it
			if (!queueScript(255))
				unhandledException(e.what(), true);
		} catch (std::exception &e) {
			unhandledException(e.what(), true);
		}
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
	assert(lifecount == 0);

	if (!initialized) return;
	
	if (vm)
		world.freeVM(vm);
	zotstack();
	zotrefs();
}

void Agent::kill() {
	assert(!dying);
	if (floatable) floatRelease();
	dropCarried();
	
	dying = true; // what a world, what a world...

	if (vm) {
		vm->stop();
		world.freeVM(vm);
		vm = 0;
	}
	
	zotstack();
	zotrefs();
	agents_iter->reset();
	if (soundslot) soundslot->stop();
}

void Agent::zotrefs() {
}

unsigned int Agent::getZOrder() const {
	if (carriedby) {
		// TODO: check for overflow
		// TODO: is adding our own zorder here correct behaviour? someone should check
		return carriedby->getZOrder() + zorder;
	} else {
		return zorder;
	}
}

void Agent::setZOrder(unsigned int z) {
	if (dying) return;
	zorder = z;
}

int Agent::getUNID() {
	  if (unid != -1)
			  return unid;
	  return unid = world.getUNID(this);
}

std::string Agent::identify() const {
	std::ostringstream o;
	o << (int)family << " " << (int)genus << " " << species;
	const std::string n = world.catalogue.getAgentName(family, genus, species);
	if (n.size())
		o << " (" + n + ")";
	/*if (unid != -1)
		o << " unid " << unid;
	else
		o << " (no unid assigned)"; */
	return o.str();
}

bool agentzorder::operator ()(const Agent *s1, const Agent *s2) const {
	return s1->getZOrder() < s2->getZOrder();
}

void Agent::pushVM(caosVM *newvm) {
	assert(newvm);
	if (vm)
		vmstack.push_front(vm);
	vm = newvm;
}

void Agent::stopScript() {
	zotstack();
	if (vm)
		vm->stop();
}

void Agent::setAttributes(unsigned int attr) {
	carryable = (attr & 1);
	mouseable = (attr & 2);
	activateable = (attr & 4);
	greedycabin = (attr & 8);
	invisible = (attr & 16);
	bool newfloatable = (attr & 32);
	if (floatable && !newfloatable)
		floatRelease();
	else if (!floatable && newfloatable)
		floatSetup();
	floatable = newfloatable;
	suffercollisions = (attr & 64);
	sufferphysics = (attr & 128);
	camerashy = (attr & 256);
	openaircabin = (attr & 512);
	rotatable = (attr & 1024);
	presence = (attr & 2048);
}

unsigned int Agent::getAttributes() const {
	unsigned int a = (carryable ? 1 : 0);
	a += (mouseable ? 2: 0);
	a += (activateable ? 4: 0);
	a += (greedycabin ? 8: 0);
	a += (invisible ? 16: 0);
	a += (floatable ? 32: 0);
	a += (suffercollisions ? 64: 0);
	a += (sufferphysics ? 128: 0);
	a += (camerashy ? 256: 0);
	a += (openaircabin ? 512: 0);
	a += (rotatable ? 1024: 0);
	return a + (presence ? 2048: 0);
}

void Agent::carry(AgentRef a) {
	// TODO: check for infinite loops (eg, us carrying an agent which is carrying us) :)

	if (carrying)
		dropCarried();

	carrying = a;
	if (!carrying) return;

	a->carriedby = AgentRef(this);
	// TODO: move carrying agent to the right position
	// TODO: this doesn't reorder children or anything..
	carrying->setZOrder(carrying->zorder);
	adjustCarried();

	// fire 'Got Carried Agent'
	queueScript(124, carrying); // TODO: is this the correct param?
}

void Agent::dropCarried() {
	if (!carrying) return;
	
	carrying->carriedby = AgentRef(0);
	// TODO: this doesn't reorder children or anything..
	carrying->setZOrder(carrying->zorder);

	// fire 'Lost Carried Agent'
	queueScript(125, carrying); // TODO: is this the correct param?
	
	carrying = AgentRef(0);
}

void Agent::adjustCarried() {
	if (!carrying) return;

	unsigned int ourpose = 0, theirpose = 0;

	SpritePart *s;
	if ((s = dynamic_cast<SpritePart *>(part(0))))
		ourpose = s->getBase() + s->getPose();
	if ((s = dynamic_cast<SpritePart *>(carrying->part(0))))
		theirpose = s->getBase() + s->getPose();

	int xoffset = 0, yoffset = 0;
	
	std::map<unsigned int, std::pair<int, int> >::iterator i = carry_points.find(ourpose);
	if (i != carry_points.end()) {
		xoffset += i->second.first;
		yoffset += i->second.second;
	}

	i = carrying->carried_points.find(theirpose);
	if (i != carrying->carried_points.end()) {
		xoffset -= i->second.first;
		yoffset -= i->second.second;
	} else if (s) {
		// c2e seems to default to (width / 2, 0)? don't know .. TODO look into this more
		xoffset -= s->getSprite()->width(s->getCurrentSprite()) / 2;
	}

	carrying->moveTo(x + xoffset, y + yoffset, true);
}

/* vim: set noet: */
