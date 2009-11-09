/*
 *  Engine.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Nov 28 2006.
 *  Copyright (c) 2006-2008 Alyssa Milburn. All rights reserved.
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

#include "Room.h"
#include "Engine.h"
#include "World.h"
#include "MetaRoom.h"
#include "caosVM.h" // for setupCommandPointers()
#include "caosScript.h" // for executeNetwork()
#include "PointerAgent.h"
#include "dialect.h" // registerDelegates
#include "backends/NullBackend.h"
#include "backends/NullAudioBackend.h"
#include "SFCFile.h"
#include "peFile.h"
#include "Camera.h"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>
namespace fs = boost::filesystem;
namespace po = boost::program_options;

#ifndef _WIN32
#include <sys/types.h> // passwd*
#include <pwd.h> // getpwuid
#endif

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#endif

Engine engine;

Engine::Engine() {
	done = false;
	dorendering = true;
	fastticks = false;
	refreshdisplay = false;

	bmprenderer = false;

	tickdata = 0;
	for (unsigned int i = 0; i < 10; i++) ticktimes[i] = 0;
	ticktimeptr = 0;
	version = 0; // TODO: something something
	
	srand(time(NULL)); // good a place as any :)
	
	cmdline_enable_sound = true;
	cmdline_norun = false;

	palette = 0;
	exefile = 0;

	addPossibleBackend("null", shared_ptr<Backend>(new NullBackend()));
	addPossibleAudioBackend("null", shared_ptr<AudioBackend>(new NullAudioBackend()));
}

Engine::~Engine() {
	if (palette) delete[] palette;
}

void Engine::addPossibleBackend(std::string s, boost::shared_ptr<Backend> b) {
	assert(!backend);
	assert(b);
	preferred_backend = s;
	possible_backends[s] = b;
}

void Engine::addPossibleAudioBackend(std::string s, boost::shared_ptr<AudioBackend> b) {
	assert(!audio);
	assert(b);
	preferred_audiobackend = s;
	possible_audiobackends[s] = b;
}

void Engine::setBackend(shared_ptr<Backend> b) {
	backend = b;
	lasttimestamp = backend->ticks();
}

void Engine::loadGameData() {
	// load palette for C1
	if (world.gametype == "c1") {
		// TODO: case-sensitivity for the lose
		fs::path palpath(world.findFile("Palettes/palette.dta"));
		if (fs::exists(palpath) && !fs::is_directory(palpath)) {
			palette = new unsigned char[768];
			
			std::ifstream f(palpath.native_directory_string().c_str(), std::ios::binary);
			f >> std::noskipws;
			f.read((char *)palette, 768);
			
			for (unsigned int i = 0; i < 768; i++) {
				palette[i] = palette[i] * 4;
			}

			backend->setPalette((uint8 *)palette);
		} else
			throw creaturesException("Couldn't find C1 palette data!");
	}

	// load word list for C2
	if (world.gametype == "c2") {
		fs::path exepath(world.findFile("Creatures2.exe"));
		if (fs::exists(exepath) && !fs::is_directory(exepath)) {
			try {
				exefile = new peFile(exepath);
			} catch (creaturesException &e) {
				std::cout << "Warning: Couldn't load word list (" << e.what() << ")!" << std::endl;
			}
		} else std::cout << "Warning: Couldn't load word list (couldn't find Creatures2.exe)!" << std::endl;

		if (exefile) {
			// TODO: support multiple languages
			resourceInfo *r = exefile->getResource(PE_RESOURCETYPE_STRING, HORRID_LANG_ENGLISH, 14);
			if (r) {
				std::vector<std::string> strings = r->parseStrings();
				if (strings.size() > 5) {
					std::string wordlistdata = strings[5];

					std::string s;
					for (unsigned int i = 0; i < wordlistdata.size(); i++) {
						if (wordlistdata[i] == '|') {
							wordlist.push_back(s);
							s.clear();
						} else s += wordlistdata[i];
					}
				} else std::cout << "Warning: Couldn't load word list (string table too small)!" << std::endl;
			} else std::cout << "Warning: Couldn't load word list (couldn't find resource)!" << std::endl;
		}
	}
}

std::string Engine::executeNetwork(std::string in) {
	// now parse and execute the CAOS we obtained
	caosVM vm(0); // needs to be outside 'try' so we can reset outputstream on exception
	try {
		std::istringstream s(in);
		caosScript script(world.gametype, "<network>"); // XXX
		script.parse(s);
		script.installScripts();
		std::ostringstream o;
		vm.setOutputStream(o);
		vm.runEntirely(script.installer);
		vm.outputstream = 0; // otherwise would point to dead stack
		return o.str();
	} catch (std::exception &e) {
		vm.outputstream = 0; // otherwise would point to dead stack
		return std::string("### EXCEPTION: ") + e.what();
	}
}

bool Engine::needsUpdate() {
	return (!world.paused) && (fastticks || (backend->ticks() > (tickdata + world.ticktime)));
}

unsigned int Engine::msUntilTick() {
	if (fastticks) return 0;
	if (world.paused) return world.ticktime; // TODO: correct?

	int ival = (tickdata + world.ticktime) - backend->ticks();
	return (ival < 0) ? 0 : ival;
}

void Engine::drawWorld() {
	// draw the world
	if (dorendering || refreshdisplay) {
		refreshdisplay = false;	

		if (backend->selfRender()) {
			// TODO: this makes race/pace hilariously inaccurate, since render time isn't included
			backend->requestRender();
		} else {
			world.drawWorld();
		}
	}
}

void Engine::update() {
	tickdata = backend->ticks();
	
	// tick the world
	world.tick();

	// poll audio streams
	audio->poll();

	// play C1 music
	// TODO: this doesn't seem to actually be every 7 seconds, but actually somewhat random
	// TODO: this should be linked to 'real' time, so it doesn't go crazy when game speed is modified
	// TODO: is this the right place for this?
	if (version == 1 && (world.tickcount % 70) == 0) {
		int piece = 1 + (rand() % 28);
		std::string filename = boost::str(boost::format("MU%02d") % piece);
		boost::shared_ptr<AudioSource> s = world.playAudio(filename, AgentRef(), false, false, true);
		if (s) s->setVolume(0.4f);
	}

	// update our data for things like pace, race, ticktime, etc
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
	drawWorld();

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

	bool wasdMode = false;
	caosVar v = world.variables["engine_wasd"];
	if (v.hasInt()) {
		switch (v.getInt()) {
			case 1: // enable if CTRL is held
				wasdMode = backend->keyDown(17); // CTRL
				break;
			case 2: // enable unconditionally
				// (this needs agent support to suppress chat bubbles etc)
				wasdMode = true;
				break;
			case 0: // disable
				wasdMode = false;
				break;
			default: // disable
				std::cout << "Warning: engine_wasd_scrolling is set to unknown value " << v.getInt() << std::endl;
				world.variables["engine_wasd_scrolling"] = caosVar(0);
				wasdMode = false;
				break;
		}
	}

	// check keys
	bool leftdown = backend->keyDown(37)
		|| (wasdMode && a_down);
	bool rightdown = backend->keyDown(39)
		|| (wasdMode && d_down);
	bool updown = backend->keyDown(38)
		|| (wasdMode && w_down);
	bool downdown = backend->keyDown(40)
		|| (wasdMode && s_down);

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
		int adjustx = world.camera->getX(), adjusty = world.camera->getY();
		int adjustbyx = (int)velx, adjustbyy = (int) vely;
			
		world.camera->moveTo(adjustx + adjustbyx, adjusty + adjustbyy, jump);
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

			case eventspecialkeyup:
				handleSpecialKeyUp(event);
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
	world.hand()->handleEvent(event);

	// notify agents
	for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;
		if ((*i)->imsk_mouse_move) {
			caosVar x; x.setFloat(world.hand()->pointerX());
			caosVar y; y.setFloat(world.hand()->pointerY());
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
			switch (event.button) { // Backend guarantees that only one button will be set on a mousebuttondown event.
				// the values here make fuzzie suspicious that c2e combines these events
				// nornagon seems to think c2e doesn't
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

	world.hand()->handleEvent(event);
}

#include "Bubble.h"
void Engine::handleKeyDown(SomeEvent &event) {
	switch (event.key) {
		case 'w': w_down = true; break;
		case 'a': a_down = true; break;
		case 's': s_down = true; break;
		case 'd': d_down = true; break;
	}

	if (version < 3 && !world.focusagent) {
		Bubble::newBubble(world.hand(), false, std::string());	
	}

	// tell the agent with keyboard focus
	if (world.focusagent) {
		CompoundPart *t = world.focusagent.get()->part(world.focuspart);
		if (t && t->canGainFocus())
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

void Engine::handleSpecialKeyUp(SomeEvent &event) {
	switch (event.key) {
		case 0x57:
			w_down = false;
			break;
		case 0x41:
			a_down = false;
			break;
		case 0x53:
			s_down = false;
			break;
		case 0x44:
			d_down = false;
			break;
	}
}

void Engine::handleSpecialKeyDown(SomeEvent &event) {
	switch (event.key) {
		case 0x57:
			w_down = true;
			break;
		case 0x41:
			a_down = true;
			break;
		case 0x53:
			s_down = true;
			break;
		case 0x44:
			d_down = true;
			break;
	}


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
					if ((world.map.getMetaRoomCount() - 1) == world.camera->getMetaRoom()->id)
						break;
					n = world.map.getMetaRoom(world.camera->getMetaRoom()->id + 1);
					if (n)
						world.camera->goToMetaRoom(n->id);
					break;

				case 34: // pagedown
					// TODO: next metaroom
					if (world.camera->getMetaRoom()->id == 0)
						break;
					n = world.map.getMetaRoom(world.camera->getMetaRoom()->id - 1);
					if (n)
						world.camera->goToMetaRoom(n->id);
					break;

				default: break; // to shut up warnings
			}
		}
	}

	// tell the agent with keyboard focus
	if (world.focusagent) {
		CompoundPart *t = world.focusagent.get()->part(world.focuspart);
		if (t && t->canGainFocus())
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

	// generate help for backend options
	std::string available_backends;
	for (std::map<std::string, boost::shared_ptr<Backend> >::iterator i = possible_backends.begin(); i != possible_backends.end(); i++) {
		if (available_backends.empty()) available_backends = i->first;
		else available_backends += ", " + i->first;
	}
	available_backends = "Select the backend (options: " + available_backends + "), default is " + preferred_backend;
	
	std::string available_audiobackends;
	for (std::map<std::string, boost::shared_ptr<AudioBackend> >::iterator i = possible_audiobackends.begin(); i != possible_audiobackends.end(); i++) {
		if (available_audiobackends.empty()) available_audiobackends = i->first;
		else available_audiobackends += ", " + i->first;
	}
	available_audiobackends = "Select the audio backend (options: " + available_audiobackends + "), default is " + preferred_audiobackend;
	
	// parse the command-line flags
	po::options_description desc;
	desc.add_options()
		("help,h", "Display help on command-line options")
		("version,V", "Display openc2e version")
		("silent,s", "Disable all sounds")
		("backend,k", po::value<std::string>(&preferred_backend)->composing(), available_backends.c_str())
		("audiobackend,o", po::value<std::string>(&preferred_audiobackend)->composing(), available_audiobackends.c_str())
		("data-path,d", po::value< std::vector<std::string> >(&data_vec)->composing(),
		 "Sets or adds a path to a data directory")
		("bootstrap,b", po::value< std::vector<std::string> >(&cmdline_bootstrap)->composing(),
		 "Sets or adds a path or COS file to bootstrap from")
		("gametype,g", po::value< std::string >(&world.gametype), "Set the game type (c1, c2, cv or c3)")
		("gamename,m", po::value< std::string >(&gamename), "Set the game name")
		("norun,n", "Don't run the game, just execute scripts")
		("autokill,a", "Enable autokill")
		("autostop", "Enable autostop (or disable it, for CV)")
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

	if (vm.count("autostop")) {
		world.autostop = true;
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

	// make a vague attempt at blacklisting some characters inside the gamename
	// (it's used in directory names, registry keys, etc)
	std::string invalidchars = "\\/:*?\"<>|";
	for (unsigned int i = 0; i < invalidchars.size(); i++) {
		if (gamename.find(invalidchars[i]) != gamename.npos)
			throw creaturesException(std::string("The character ") + invalidchars[i] + " is not valid in a gamename.");
	}

	return true;
}

bool Engine::initialSetup() {
	assert(world.data_directories.size() > 0);

	// autodetect gametype if necessary
	if (world.gametype.empty()) {
		std::cout << "Warning: No gametype specified, ";
		// TODO: is this sane? especially unsure about about.exe
		if (!world.findFile("Creatures.exe").empty()) {
			std::cout << "found Creatures.exe, assuming C1 (c1)";
			world.gametype = "c1";
		} else if (!world.findFile("Creatures2.exe").empty()) {
			std::cout << "found Creatures2.exe, assuming C2 (c2)";
			world.gametype = "c2";
		} else if (!world.findFile("Sea-Monkeys.ico").empty()) {
			std::cout << "found Sea-Monkeys.ico, assuming Sea-Monkeys (sm)";
			world.gametype = "sm";
		} else if (!world.findFile("about.exe").empty()) {
			std::cout << "found about.exe, assuming CA, CP or CV (cv)";
			world.gametype = "cv";
		} else {
			std::cout << "assuming C3/DS (c3)";
			world.gametype = "c3";
		}
		std::cout << ", see --help if you need to specify one." << std::endl;
	}

	// set engine version
	// TODO: set gamename
	if (world.gametype == "c1") {
		if (gamename.empty()) gamename = "Creatures 1";
		version = 1;
	} else if (world.gametype == "c2") {
		if (gamename.empty()) gamename = "Creatures 2";
		version = 2;
	} else if (world.gametype == "c3") {
		if (gamename.empty()) gamename = "Creatures 3";
		version = 3;
	} else if (world.gametype == "cv") {
		if (gamename.empty()) gamename = "Creatures Village";
		version = 3;
		world.autostop = !world.autostop;
	} else if (world.gametype == "sm") {
		if (gamename.empty()) gamename = "Sea-Monkeys";
		version = 3;
		bmprenderer = true;
	} else
		throw creaturesException(boost::str(boost::format("unknown gametype '%s'!") % world.gametype));

	// finally, add our cache directory to the end
	world.data_directories.push_back(storageDirectory());
	
	// initial setup
	registerDelegates();
	std::cout << "* Reading catalogue files..." << std::endl;
	world.initCatalogue();
	std::cout << "* Initial setup..." << std::endl;
	world.init(); // just reads mouse cursor (we want this after the catalogue reading so we don't play "guess the filename")
	if (engine.version > 2) {
		std::cout << "* Reading PRAY files..." << std::endl;
		world.praymanager.update();
	}

#ifdef _WIN32
	// Here we need to set the working directory since apparently windows != clever
	char exepath[MAX_PATH] = "";
	GetModuleFileName(0, exepath, sizeof(exepath) - 1);
	char *exedir = strrchr(exepath, '\\');
	if(exedir) {
		// null terminate the string
        *exedir = 0;
		// Set working directory
		SetCurrentDirectory(exepath);
	}
	else // err, oops
		std::cerr << "Warning: Setting working directory to " << exepath << " failed.";
#endif

	if (cmdline_norun) preferred_backend = "null";
	if (preferred_backend != "null") std::cout << "* Initialising backend " << preferred_backend << "..." << std::endl;	
	shared_ptr<Backend> b = possible_backends[preferred_backend];
	if (!b)	throw creaturesException("No such backend " + preferred_backend);
	b->init(); setBackend(b);
	possible_backends.clear();

	if (cmdline_norun || !cmdline_enable_sound) preferred_audiobackend = "null";
	if (preferred_audiobackend != "null") std::cout << "* Initialising audio backend " << preferred_audiobackend << "..." << std::endl;	
	shared_ptr<AudioBackend> a = possible_audiobackends[preferred_audiobackend];
	if (!a)	throw creaturesException("No such audio backend " + preferred_audiobackend);
	try{
		a->init(); audio = a;
	} catch (creaturesException &e) {
		std::cerr << "* Couldn't initialize backend " << preferred_audiobackend << ": " << e.what() << std::endl << "* Continuing without sound." << std::endl;
		audio = shared_ptr<AudioBackend>(new NullAudioBackend());
		audio->init();
	}
	possible_audiobackends.clear();

	world.camera->setBackend(backend); // TODO: hrr
	
	int listenport = backend->networkInit();
	if (listenport != -1) {
		// inform the user of the port used, and store it in the relevant file
		std::cout << "Listening for connections on port " << listenport << "." << std::endl;
#ifndef _WIN32
		fs::path p = fs::path(homeDirectory().native_directory_string() + "/.creaturesengine", fs::native);
		if (!fs::exists(p))
			fs::create_directory(p);
		if (fs::is_directory(p)) {
			std::ofstream f((p.native_directory_string() + "/port").c_str(), std::ios::trunc);
			f << boost::str(boost::format("%d") % listenport);
		}
#endif
	}

	if (world.data_directories.size() < 3) {
		// TODO: This is a hack for DS, basically. Not sure if it works properly. - fuzzie
		caosVar name; name.setString("engine_no_auxiliary_bootstrap_1");
		caosVar contents; contents.setInt(1);
		eame_variables[name] = contents;
	}

	loadGameData();

	// execute the initial scripts!
	std::cout << "* Executing initial scripts..." << std::endl;
	if (cmdline_bootstrap.size() == 0) {
		world.executeBootstrap(false);
	} else {
		std::vector<std::string> scripts;

		if (engine.version < 3 && cmdline_bootstrap.size() != 1)
			throw creaturesException("multiple bootstrap files provided in C1/C2 mode");
		
		for (std::vector< std::string >::iterator bsi = cmdline_bootstrap.begin(); bsi != cmdline_bootstrap.end(); bsi++) {
			fs::path scriptdir(*bsi, fs::native);
			if (engine.version > 2 || fs::extension(scriptdir) == ".cos") {
				// pass it to the world to execute (it handles both files and directories)

				if (!fs::exists(scriptdir)) {
					std::cerr << "Warning: Couldn't find a specified script directory (trying " << *bsi << ")!\n";
					continue;
				}

				world.executeBootstrap(scriptdir);
			} else {
				// in c1/c2 mode, if not a cos file, assume it's an SFC file
				if (!fs::exists(scriptdir) || fs::is_directory(scriptdir))
					throw creaturesException("non-existant bootstrap file provided in C1/C2 mode");
				// TODO: the default SFCFile loading code is in World, maybe this should be too..
				SFCFile sfc;
				std::ifstream f(scriptdir.native_directory_string().c_str(), std::ios::binary);
				f >> std::noskipws;
				sfc.read(&f);
				sfc.copyToWorld();
			}
		}
	}

	// if there aren't any metarooms, we can't run a useful game, the user probably
	// wanted to execute a CAOS script or something went badly wrong.
	if (!cmdline_norun && world.map.getMetaRoomCount() == 0) {
		shutdown();
		throw creaturesException("No metarooms found in given bootstrap directories or files");
	}

	std::cout << "* Done startup." << std::endl;

	if (cmdline_norun) {
		// TODO: see comment above about avoiding backend when norun is set
		std::cout << "Told not to run the world, so stopping now." << std::endl;
		shutdown();
		return false;
	}

	return true;
}

void Engine::shutdown() {
	world.shutdown();
	audio->shutdown();
	backend->shutdown();
	freeDelegates(); // does nothing if there are none (ie, no call to initialSetup)
}

fs::path Engine::homeDirectory() {
	fs::path p;

#ifndef _WIN32
	char *envhome = getenv("HOME");
	if (envhome)
		p = fs::path(envhome, fs::native);
	if ((!envhome) || (!fs::is_directory(p)))
		p = fs::path(getpwuid(getuid())->pw_dir, fs::native);
	if (!fs::is_directory(p)) {
		std::cerr << "Can't work out what your home directory is, giving up and using /tmp for now." << std::endl;
		p = fs::path("/tmp", fs::native); // sigh
	}
#else
	TCHAR szPath[_MAX_PATH];
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, TRUE);

	p = fs::path(szPath, fs::native);
	if (!fs::exists(p) || !fs::is_directory(p))
		throw creaturesException("Windows reported that your My Documents folder is at '" + std::string(szPath) + "' but there's no directory there!");
#endif

	return p;
}

fs::path Engine::storageDirectory() {
#ifdef _WIN32
	std::string dirname = "/My Games";
#else
#ifdef __APPLE__
	std::string dirname = "/Documents/openc2e Data";
#else
	std::string dirname = "/.openc2e";
#endif
#endif
	
	// main storage dir
	fs::path p = fs::path(homeDirectory().native_directory_string() + dirname, fs::native);
	if (!fs::exists(p))
		fs::create_directory(p);
	else if (!fs::is_directory(p))
		throw creaturesException("Your openc2e data directory " + p.native_directory_string() + " is a file, not a directory. That's bad.");
	
	// game-specific storage dir
	p = fs::path(p.native_directory_string() + std::string("/" + gamename), fs::native);
	if (!fs::exists(p))
		fs::create_directory(p);
	else if (!fs::is_directory(p))
		throw creaturesException("Your openc2e game data directory " + p.native_directory_string() + " is a file, not a directory. That's bad.");
	
	return p;
}

/* vim: set noet: */
