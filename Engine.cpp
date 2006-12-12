/*
 *  Engine.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Nov 28 2006.
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

#include "Engine.h"
#include "World.h"
#include "caosVM.h" // for setupCommandPointers()
#include "PointerAgent.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
namespace fs = boost::filesystem;

Engine engine;

Engine::Engine() {
	backend = 0;
	done = false;
	tickdata = 0;
	for (unsigned int i = 0; i < 10; i++) ticktimes[i] = 0;
	ticktimeptr = 0;
	version = 0; // TODO: something something
}

void Engine::setBackend(Backend *b) {
	backend = b;
	lasttimestamp = backend->ticks();

	// load palette for C1
	if (world.gametype == "c1") {
		// TODO: case-sensitivity for the lose
		fs::path palpath(world.data_directories[0] / "/Palettes/palette.dta");
		if (fs::exists(palpath) && !fs::is_directory(palpath)) {
			uint8 *buf = new uint8[768];
			std::ifstream f(palpath.native_directory_string().c_str(), std::ios::binary);
			f >> std::noskipws;
			f.read((char *)buf, 768);
			backend->setPalette(buf);
			delete[] buf;
		} else
			throw creaturesException("Couldn't find C1 palette data!");
	}
}

std::string Engine::executeNetwork(std::string in) {
	// now parse and execute the CAOS we obtained
	try {
		std::istringstream s(in);
		caosScript script(world.gametype, "<network>"); // XXX
		script.parse(s);
		script.installScripts();
		caosVM vm(0);
		std::ostringstream o;
		vm.setOutputStream(o);
		vm.runEntirely(script.installer);
		return o.str();
	} catch (std::exception &e) {
		return std::string("### EXCEPTION: ") + e.what();
	}
}

bool Engine::needsUpdate() {
	return (!world.paused) && (backend->ticks() > (tickdata + world.ticktime));
}

void Engine::update() {
	tickdata = backend->ticks();
			
	world.tick();
	// TODO: if (!backend->updateWorld())
	world.drawWorld();

	ticktimes[ticktimeptr] = backend->ticks() - tickdata;
	ticktimeptr++;
	if (ticktimeptr == 10) ticktimeptr = 0;
	float avgtime = 0;
	for (unsigned int i = 0; i < 10; i++) avgtime += ((float)ticktimes[i] / world.ticktime);
	world.pace = avgtime / 10;
		
	world.race = backend->ticks() - lasttimestamp;
	lasttimestamp = backend->ticks();
}

bool Engine::tick() {
	assert(backend);
	backend->handleEvents();

	// tick+draw the world, if necessary
	bool needupdate = needsUpdate();
	if (needupdate)
		update();

	processEvents();
	if (needupdate)
		handleKeyboardScrolling();

	return needupdate;
}

void Engine::handleKeyboardScrolling() {
	// keyboard-based scrolling
	static float accelspeed = 8, decelspeed = .5, maxspeed = 64;
	static float velx = 0;
	static float vely = 0;

	// check keys
	bool leftdown = backend->keyDown(37);
	bool rightdown = backend->keyDown(39);
	bool updown = backend->keyDown(38);
	bool downdown = backend->keyDown(40);
	if (leftdown)
		velx -= accelspeed;
	if (rightdown)
		velx += accelspeed;
	if (!leftdown && !rightdown) {
		velx *= decelspeed;
		if (fabs(velx) < 0.1) velx = 0;
	}
	if (updown)
		vely -= accelspeed;
	if (downdown)
		vely += accelspeed;
	if (!updown && !downdown) {
		vely *= decelspeed;
		if (fabs(vely) < 0.1) vely = 0;
	}

	// enforced maximum speed
	if (velx >=  maxspeed) velx =  maxspeed;
	else if (velx <= -maxspeed) velx = -maxspeed;
	if (vely >=  maxspeed) vely =  maxspeed;
	else if (vely <= -maxspeed) vely = -maxspeed;

	// do the actual movement
	if (velx || vely) {
		int adjustx = world.camera.getX(), adjusty = world.camera.getY();
		int adjustbyx = (int)velx, adjustbyy = (int) vely;
			
		world.camera.moveTo(adjustx + adjustbyx, adjusty + adjustbyy, jump);
	}
}

void Engine::processEvents() {
	SomeEvent event;
	while (backend->pollEvent(event)) {
		switch (event.type) {
			case eventresizewindow:
				handleResizedWindow(event);
				break;

			case eventmousemove:
				handleMouseMove(event);
				break;

			case eventmousebuttonup:
			case eventmousebuttondown:
				handleMouseButton(event);
				break;

			case eventkeydown:
				handleKeyDown(event);
				break;

			case eventspecialkeydown:
				handleSpecialKeyDown(event);
				break;
				
			case eventquit:
				done = true;
				break;

			default:
				break;
		}
	}
}

void Engine::handleResizedWindow(SomeEvent &event) {
	// notify agents
	for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;
		(*i)->queueScript(123, 0); // window resized script
	}
}

void Engine::handleMouseMove(SomeEvent &event) {
	// move the cursor
	world.hand()->moveTo(event.x + world.camera.getX(), event.y + world.camera.getY());
	world.hand()->velx.setInt(event.xrel * 4);
	world.hand()->vely.setInt(event.yrel * 4);
	
	// TODO: fix
	// middle mouse button scrolling
	//if (event.motion.state & SDL_BUTTON(2))
	//	world.camera.moveTo(world.camera.getX() - event.xrel, world.camera.getY() - event.yrel, jump);
					
	// notify agents
	for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;
		if ((*i)->imsk_mouse_move) {
			caosVar x; x.setInt(world.hand()->x);
			caosVar y; y.setInt(world.hand()->y);
			(*i)->queueScript(75, 0, x, y); // Raw Mouse Move
		}
	}
}

void Engine::handleMouseButton(SomeEvent &event) {
	// notify agents
	for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;
		if ((event.type == eventmousebuttonup && (*i)->imsk_mouse_up) ||
			(event.type == eventmousebuttondown && (*i)->imsk_mouse_down)) {
			// set the button value as necessary
			caosVar button;
			switch (event.button) {
				// TODO: the values here make fuzzie suspicious that c2e combines these events
				case buttonleft: button.setInt(1); break;
				case buttonright: button.setInt(2); break;
				case buttonmiddle: button.setInt(4); break;
				default: break;
			}

			// if it was a mouse button we're interested in, then fire the relevant raw event
			if (button.getInt() != 0) {
				if (event.type == eventmousebuttonup)
					(*i)->queueScript(77, 0, button); // Raw Mouse Up
				else
					(*i)->queueScript(76, 0, button); // Raw Mouse Down
			}
		}
		if ((event.type == eventmousebuttondown &&
			(event.button == buttonwheelup || event.button == buttonwheeldown) &&
			(*i)->imsk_mouse_wheel)) {
			// fire the mouse wheel event with the relevant delta value
			caosVar delta;
			if (event.button == buttonwheeldown)
				delta.setInt(-120);
			else
				delta.setInt(120);
			(*i)->queueScript(78, 0, delta); // Raw Mouse Wheel
		}
	}

	if (!world.hand()->handle_events) return;
	if (event.type != eventmousebuttondown) return;

	// do our custom handling
	if (event.button == buttonleft) {
		CompoundPart *a = world.partAt(world.hand()->x, world.hand()->y);
		if (a /* && a->canActivate() */) { // TODO
			// if the agent isn't paused, tell it to handle a click
			if (!a->getParent()->paused)
				a->handleClick(world.hand()->x - a->x - a->getParent()->x, world.hand()->y - a->y - a->getParent()->y);

			// TODO: not sure how to handle the following properly, needs research..
			world.hand()->firePointerScript(101, a->getParent()); // Pointer Activate 1
		} else
			world.hand()->queueScript(116, 0); // Pointer Clicked Background
	} else if (event.button == buttonright) {
		if (world.paused) return; // TODO: wrong?
						
		// picking up and dropping are implictly handled by the scripts (well, messages) 4 and 5	
		// TODO: check if this is correct behaviour, one issue is that this isn't instant, another
		// is the messages might only be fired in c2e when you use MESG WRIT, in which case we'll
		// need to manually set world.hand()->carrying to NULL and a here, respectively - fuzzie
		if (world.hand()->carrying) {
			// TODO: c1 support - these attributes are invalid for c1
			if (!world.hand()->carrying->suffercollisions() || world.hand()->carrying->validInRoomSystem()) {
				world.hand()->carrying->queueScript(5, world.hand()); // drop
				world.hand()->firePointerScript(105, world.hand()->carrying); // Pointer Drop

				// TODO: is this the correct check?
				if (world.hand()->carrying->sufferphysics() && world.hand()->carrying->suffercollisions()) {
					// TODO: do this in the pointer agent?
					world.hand()->carrying->velx.setFloat(world.hand()->velx.getFloat());
					world.hand()->carrying->vely.setFloat(world.hand()->vely.getFloat());
				}
			} else {
				// TODO: some kind of "fail to drop" animation/sound?
			}
		} else {
			Agent *a = world.agentAt(event.x + world.camera.getX(), event.y + world.camera.getY(), false, true);
			if (a) {
				a->queueScript(4, world.hand()); // pickup
				world.hand()->firePointerScript(104, a); // Pointer Pickup
			}
		}
	} else if (event.button == buttonmiddle) {
		Agent *a = world.agentAt(event.x + world.camera.getX(), event.y + world.camera.getY(), true);
		if (a)
			std::cout << "Agent under mouse is " << a->identify();
		else
			std::cout << "No agent under cursor";
		std::cout << std::endl;
	}
}

void Engine::handleKeyDown(SomeEvent &event) {
	// handle debug keys, if they're enabled
	caosVar v = world.variables["engine_debug_keys"];
	if (v.hasInt() && v.getInt() == 1) {
		// TODO: fix
		/*Uint8 *keystate = SDL_GetKeyState(NULL);
		if (keystate[SDLK_LSHIFT] || keystate[SDLK_RSHIFT]) {
			MetaRoom *n; // for pageup/pagedown

			switch (event.key.keysym.sym) {
				case SDLK_INSERT:
					world.showrooms = !world.showrooms;
					break;

				case SDLK_PAUSE:
					// TODO: debug pause game
					break;

				case SDLK_SPACE:
					// TODO: force tick
					break;

				case SDLK_PAGEUP:
					// TODO: previous metaroom
					if ((world.map.getMetaRoomCount() - 1) == world.camera.getMetaRoom()->id)
						break;
					n = world.map.getMetaRoom(world.camera.getMetaRoom()->id + 1);
					if (n)
						world.camera.goToMetaRoom(n->id);
					break;

				case SDLK_PAGEDOWN:
					// TODO: next metaroom
					if (world.camera.getMetaRoom()->id == 0)
						break;
					n = world.map.getMetaRoom(world.camera.getMetaRoom()->id - 1);
					if (n)
						world.camera.goToMetaRoom(n->id);
					break;

				default: break; // to shut up warnings
			}
		}*/
	}

	// tell the agent with keyboard focus
	if (world.focusagent) {
		TextEntryPart *t = (TextEntryPart *)((CompoundAgent *)world.focusagent.get())->part(world.focuspart);
		if (t)
			t->handleKey(event.key);
	}

	// notify agents
	caosVar k;
	k.setInt(event.key);
	for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;
		if ((*i)->imsk_translated_char)
			(*i)->queueScript(79, 0, k); // translated char script
	}
}

void Engine::handleSpecialKeyDown(SomeEvent &event) {
	// tell the agent with keyboard focus
	if (world.focusagent) {
		TextEntryPart *t = (TextEntryPart *)((CompoundAgent *)world.focusagent.get())->part(world.focuspart);
		if (t)
			t->handleSpecialKey(event.key);
	}

	// notify agents
	caosVar k;
	k.setInt(event.key);
	for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;
		if ((*i)->imsk_key_down)
			(*i)->queueScript(73, 0, k); // key down script
	}
}

/* vim: set noet: */
