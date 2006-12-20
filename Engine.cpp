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
#include "MetaRoom.h"
#include "caosVM.h" // for setupCommandPointers()
#include "PointerAgent.h"
#include "dialect.h" // registerDelegates

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
namespace fs = boost::filesystem;
namespace po = boost::program_options;

Engine engine;

Engine::Engine() {
	backend = 0;
	done = false;
	tickdata = 0;
	for (unsigned int i = 0; i < 10; i++) ticktimes[i] = 0;
	ticktimeptr = 0;
	version = 0; // TODO: something something
	
	srand(time(NULL)); // good a place as any :)
	
	cmdline_enable_sound = true;
	cmdline_norun = false;
}

Engine::~Engine() {
	if (backend) delete backend;
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
	// handle debug keys, if they're enabled
	caosVar v = world.variables["engine_debug_keys"];
	if (v.hasInt() && v.getInt() == 1) {
		if (backend->keyDown(16)) { // shift down
			MetaRoom *n; // for pageup/pagedown

			switch (event.key) {
				case 45: // insert
					world.showrooms = !world.showrooms;
					break;

				case 19: // pause
					// TODO: debug pause game
					break;

				case 32: // space
					// TODO: force tick
					break;

				case 33: // pageup
					// TODO: previous metaroom
					if ((world.map.getMetaRoomCount() - 1) == world.camera.getMetaRoom()->id)
						break;
					n = world.map.getMetaRoom(world.camera.getMetaRoom()->id + 1);
					if (n)
						world.camera.goToMetaRoom(n->id);
					break;

				case 34: // pagedown
					// TODO: next metaroom
					if (world.camera.getMetaRoom()->id == 0)
						break;
					n = world.map.getMetaRoom(world.camera.getMetaRoom()->id - 1);
					if (n)
						world.camera.goToMetaRoom(n->id);
					break;

				default: break; // to shut up warnings
			}
		}
	}

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

extern fs::path homeDirectory(); // creaturesImage.cpp
extern fs::path cacheDirectory(); // creaturesImage.cpp
static const char data_default[] = "./data";

static void opt_version() {
	// We already showed the primary version bit, just throw in some random legalese
	std::cout << 
		"This is free software; see the source for copying conditions.  There is NO" << std::endl <<
		"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << std::endl << std::endl <<
		"...please don't sue us." << std::endl;
}

bool Engine::parseCommandLine(int argc, char *argv[]) {
	// variables for command-line flags
	int optret;
	std::vector<std::string> data_vec;

	// parse the command-line flags
	po::options_description desc;
	desc.add_options()
		("help,h", "Display help on command-line options")
		("version,V", "Display openc2e version")
		("silent,s", "Disable all sounds")
		("data-path,d", po::value< std::vector<std::string> >(&data_vec)->composing(),
		 "Set the path to the data directory")
		("bootstrap,b", po::value< std::vector<std::string> >(&cmdline_bootstrap)->composing(),
		 "Sets or adds a path or COS file to bootstrap from")
		("gametype,g", po::value< std::string >(&world.gametype), "Set the game type (c1, c2, cv or c3)")
		("norun,n", "Don't run the game, just execute scripts")
		("autokill,a", "Enable autokill")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	cmdline_enable_sound = !vm.count("silent");
	cmdline_norun = vm.count("norun");
	
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return false;
	}

	if (vm.count("version")) {
		opt_version();
		return false;
	}

	if (vm.count("autokill")) {
		world.autokill = true;
	}

	if (vm.count("data-path") == 0) {
		std::cout << "Warning: No data path specified, trying default of '" << data_default << "', see --help if you need to specify one." << std::endl;
		data_vec.push_back(data_default);
	}

	// add all the data directories to the list
	for (std::vector<std::string>::iterator i = data_vec.begin(); i != data_vec.end(); i++) {
		fs::path datadir(*i, fs::native);
		if (!fs::exists(datadir)) {
			throw creaturesException("data path '" + *i + "' doesn't exist");
		}
		world.data_directories.push_back(datadir);
	}

	return true;
}

bool Engine::initialSetup(Backend *b) {
	assert(world.data_directories.size() > 0);

	// autodetect gametype if necessary
	if (world.gametype.empty()) {
		std::cout << "Warning: No gametype specified, ";
		// TODO: is this sane? what about CV?
		if (!world.findFile("Creatures.exe").empty()) {
			std::cout << "found Creatures.exe, assuming C1 (c1)";
			world.gametype = "c1";
		} else if (!world.findFile("Creatures2.exe").empty()) {
			std::cout << "found Creatures2.exe, assuming C2 (c2)";
			world.gametype = "c2";
		} else {
			std::cout << "assuming C3/DS (c3)";
			world.gametype = "c3";
		}
		std::cout << ", see --help if you need to specify one." << std::endl;
	}

	// finally, add our cache directory to the end
	world.data_directories.push_back(cacheDirectory());
	
	// initial setup
	registerDelegates();
	std::cout << "* Reading catalogue files..." << std::endl;
	world.initCatalogue();
	std::cout << "* Initial setup..." << std::endl;
	world.init(); // just reads mouse cursor (we want this after the catalogue reading so we don't play "guess the filename")
	std::cout << "* Reading PRAY files..." << std::endl;
	world.praymanager.update();
	std::cout << "* Initialising backend..." << std::endl;
	// TODO: ideally we shouldn't bother with the backend if norun is set (but we need one right now, for MainCamera/CAOS)
	engine.setBackend(b);
	engine.backend->init();
	if (cmdline_enable_sound) engine.backend->soundInit();
	world.camera.setBackend(engine.backend); // TODO: hrr
	
	int listenport = engine.backend->networkInit();
	if (listenport != -1) {
		// inform the user of the port used, and store it in the relevant file
		std::cout << "Listening for connections on port " << listenport << "." << std::endl;
		fs::path p = fs::path(homeDirectory().native_directory_string() + "/.creaturesengine", fs::native);
		if (!fs::exists(p))
			fs::create_directory(p);
		if (fs::is_directory(p)) {
			std::ofstream f((p.native_directory_string() + "/port").c_str(), std::ios::trunc);
			f << boost::str(boost::format("%d") % listenport);
		}
	}

	if (world.data_directories.size() < 3) {
		// TODO: This is a hack for DS, basically. Not sure if it works properly. - fuzzie
		caosVar name; name.setString("engine_no_auxiliary_bootstrap_1");
		caosVar contents; contents.setInt(1);
		engine.eame_variables[name] = contents;
	}

	// execute the initial scripts!
	std::cout << "* Executing initial scripts..." << std::endl;
	if (cmdline_bootstrap.size() == 0) {
		world.executeBootstrap(false);
	} else {
		std::vector<std::string> scripts;
	
		for (std::vector< std::string >::iterator bsi = cmdline_bootstrap.begin(); bsi != cmdline_bootstrap.end(); bsi++) {
			fs::path scriptdir(*bsi, fs::native);
			if (!fs::exists(scriptdir)) {
				std::cerr << "Warning: Couldn't find a specified script directory (trying " << *bsi << ")!\n";
				continue;
			}
			world.executeBootstrap(scriptdir);
		}
	}

	// if there aren't any metarooms, we can't run a useful game, the user probably
	// wanted to execute a CAOS script or something went badly wrong.
	if (!cmdline_norun && world.map.getMetaRoomCount() == 0) {
		engine.backend->shutdown();
		throw creaturesException("No metarooms found in given bootstrap directories or files");
	}

	std::cout << "* Done startup." << std::endl;

	if (cmdline_norun) {
		// TODO: see comment above about avoiding backend when norun is set
		std::cout << "Told not to run the world, so stopping now." << std::endl;
		engine.backend->shutdown();
		return false;
	}

	return true;
}

/* vim: set noet: */
