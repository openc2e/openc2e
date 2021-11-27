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

#include "Engine.h"

#include "Bubble.h"
#include "Camera.h"
#include "Map.h"
#include "MetaRoom.h"
#include "MusicManager.h"
#include "NetBackend.h"
#include "NullBackend.h"
#include "PathResolver.h"
#include "PointerAgent.h"
#include "Room.h"
#include "SFCFile.h"
#include "SoundManager.h"
#include "World.h"
#include "caosScript.h" // for executeNetwork()
#include "caosVM.h"
#include "common/case_insensitive_filesystem.h"
#include "common/encoding.h"
#include "common/find_if.h"
#include "common/userlocale.h"
#include "fileformats/cfgFile.h"
#include "fileformats/peFile.h"
#include "imageManager.h"
#include "keycodes.h"
#include "openc2e-audiobackend/NullAudioBackend.h"
#include "prayManager.h"

#ifdef _WIN32
#include "common/ComPtr.h"
#endif

#include <cassert>
#include <ghc/filesystem.hpp>
#define CXXOPTS_VECTOR_DELIMITER '\0'
#include <cxxopts.hpp>
#include <fmt/core.h>
#include <memory>
#include <stdexcept>

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#else
#include <pwd.h> // getpwuid
#include <sys/types.h> // passwd*
#endif

namespace fs = ghc::filesystem;

Engine engine;

Engine::Engine() {
	done = false;
	dorendering = true;
	fastticks = false;
	refreshdisplay = false;

	bmprenderer = false;

	std::vector<std::string> languages = get_preferred_languages();
	if (!languages.empty()) {
		language = languages.front();
	} else {
		language = "en";
	}

	tickdata = 0;
	for (unsigned int& ticktime : ticktimes)
		ticktime = 0;
	ticktimeptr = 0;
	version = 0; // TODO: something something

	srand(time(NULL)); // good a place as any :)

	cmdline_enable_sound = true;
	cmdline_norun = false;

	exefile = 0;

	addPossibleBackend("null", std::shared_ptr<Backend>(new NullBackend()));
	addPossibleAudioBackend("null", std::shared_ptr<AudioBackend>(new NullAudioBackend()));

	camera.reset(new MainCamera);
}

Engine::~Engine() {
}

void Engine::addPossibleBackend(std::string s, std::shared_ptr<Backend> b) {
	assert(!backend);
	assert(b);
	preferred_backend = s;
	possible_backends[s] = b;
}

void Engine::addPossibleAudioBackend(std::string s, std::shared_ptr<AudioBackend> b) {
	assert(!audio);
	assert(b);
	preferred_audiobackend = s;
	possible_audiobackends[s] = b;
}

void Engine::setBackend(std::shared_ptr<Backend> b) {
	backend = b;
	lasttimestamp = backend->ticks();
}

static std::vector<std::string> read_wordlist(peFile* exefile, PeLanguage lang) {
	optional<resourceInfo> r = exefile->findResource(PE_RESOURCETYPE_STRING, lang, 14);
	if (!r) {
		std::cout << "Warning: Couldn't load word list (couldn't find resource)!" << std::endl;
		return {};
	}

	std::vector<std::string> strings = exefile->getResourceStrings(*r);
	if (strings.size() < 6) {
		std::cout << "Warning: Couldn't load word list (string table too small)!" << std::endl;
		return {};
	}

	std::string wordlistdata = utf8_to_cp1252(strings[5]);
	std::vector<std::string> wordlist;
	std::string s;
	for (char i : wordlistdata) {
		if (i == '|') {
			wordlist.push_back(s);
			s.clear();
		} else
			s += i;
	}

	return wordlist;
}

void Engine::loadGameData() {
	// load word list translations for C1
	// C1 does not keep translations for all languages, so we embed them
	if (gametype == "c1") {
		if (language == "de") {
			wordlist_translations = std::map<std::string, std::string>{
				{"come", "komm"},
				{"drop", "lass"},
				{"get", "hol"},
				{"left", "links"},
				{"look", "guck"},
				{"no", "nein"},
				{"pull", "zieh"},
				{"push", "druck"},
				{"right", "rechts"},
				{"run", "lauf"},
				{"sleep", "raste"},
				{"stop", "halt"},
				{"what", "was"},
				{"yes", "ja"}};
		} else if (language == "fr") {
			wordlist_translations = std::map<std::string, std::string>{
				{"come", "venir"},
				{"drop", "lacher"},
				{"get", "prendre"},
				{"left", "gauche"},
				{"look", "regarder"},
				{"no", "non"},
				{"pull", "tirer"},
				{"push", "pousser"},
				{"right", "droite"},
				{"run", "courir"},
				{"sleep", "se-reposer"},
				{"stop", "arreter"},
				{"what", "quoi"},
				{"yes", "oui"}};
		} else if (language == "it") {
			wordlist_translations = std::map<std::string, std::string>{
				{"come", "vieni"},
				{"drop", "molla"},
				{"get", "prendi"},
				{"left", "sinistra"},
				{"look", "guarda"},
				{"no", "no"},
				{"pull", "tira"},
				{"push", "premi"},
				{"right", "destra"},
				{"run", "corri"},
				{"sleep", "riposa"},
				{"stop", "stop"},
				{"what", "cosa"},
				{"yes", "si"}};
		} else if (language == "nl") {
			wordlist_translations = std::map<std::string, std::string>{
				{"come", "komen"},
				{"drop", "latenvallen"},
				{"get", "pakken"},
				{"left", "links"},
				{"look", "kijken"},
				{"no", "nee"},
				{"pull", "trekken"},
				{"push", "duwen"},
				{"right", "rechts"},
				{"run", "rennen"},
				{"sleep", "rusten"},
				{"stop", "stoppen"},
				{"what", "wat"},
				{"yes", "ja"}};
		} else if (language == "es") {
			wordlist_translations = std::map<std::string, std::string>{
				{"come", "venir"},
				{"drop", "soltar"},
				{"get", "coger"},
				{"left", "izquierda"},
				{"look", "mirar"},
				{"no", "no"},
				{"pull", "tirar"},
				{"push", "empujar"},
				{"right", "derecha"},
				{"run", "correr"},
				{"sleep", "descansar"},
				{"stop", "parar"},
				{"what", "que"},
				{"yes", "si"}};
		}
	}

	// load word list for C2
	if (gametype == "c2") {
		fs::path exepath(findMainDirectoryFile("Creatures2.exe"));
		if (fs::exists(exepath) && !fs::is_directory(exepath)) {
			try {
				exefile = new peFile(exepath);
			} catch (Exception& e) {
				std::cout << "Warning: Couldn't load word list (" << e.what() << ")!" << std::endl;
			}
		} else
			std::cout << "Warning: Couldn't load word list (couldn't find Creatures2.exe)!" << std::endl;

		if (exefile) {
			auto english_wordlist = read_wordlist(exefile, PE_LANGUAGE_ENGLISH);

			wordlist.clear();
			wordlist_translations.clear();
			if (language == "de") {
				wordlist = read_wordlist(exefile, PE_LANGUAGE_GERMAN);
			} else if (language == "fr") {
				wordlist = read_wordlist(exefile, PE_LANGUAGE_FRENCH);
			} else if (language == "it") {
				wordlist = read_wordlist(exefile, PE_LANGUAGE_ITALIAN);
			} else if (language == "nl") {
				wordlist = read_wordlist(exefile, PE_LANGUAGE_DUTCH);
			} else if (language == "es") {
				wordlist = read_wordlist(exefile, PE_LANGUAGE_SPANISH);
			}

			if (wordlist.empty()) {
				wordlist = english_wordlist;
			} else {
				for (size_t i = 0; i < english_wordlist.size() && i < wordlist.size(); ++i) {
					wordlist_translations[english_wordlist[i]] = wordlist[i];
				}
			}
		}
	}
}

std::string Engine::translateWordlistWord(const std::string& s) {
	auto it = wordlist_translations.find(s);
	if (it == wordlist_translations.end()) {
		return s;
	} else {
		return it->second;
	}
}

std::string Engine::executeNetwork(std::string in) {
	// now parse and execute the CAOS we obtained
	caosVM vm(0); // needs to be outside 'try' so we can reset outputstream on exception
	try {
		std::istringstream s(in);
		caosScript script(gametype, "<network>"); // XXX
		script.parse(s);
		script.installScripts();
		std::ostringstream o;
		vm.setOutputStream(o);
		vm.runEntirely(script.installer);
		vm.outputstream = 0; // otherwise would point to dead stack
		return o.str();
	} catch (Exception& e) {
		vm.outputstream = 0; // otherwise would point to dead stack
		return std::string("### EXCEPTION: ") + e.what();
	}
}

unsigned int Engine::msUntilTick() {
	if (fastticks)
		return 0;
	if (world.paused)
		return world.ticktime; // TODO: correct?

	int ival = (tickdata + world.ticktime) - backend->ticks();
	return (ival < 0) ? 0 : ival;
}

void Engine::drawWorld() {
	// draw the world
	if (dorendering || refreshdisplay) {
		refreshdisplay = false;
		world.drawWorld();
	}
}

void Engine::update() {
	tickdata = backend->ticks();

	// tick the world
	world.tick();

	// update sounds
	soundmanager.tick();

	// play C1 background wavs and MNG and MIDI music
	musicmanager->tick();

	// update our data for things like pace, race, ticktime, etc
	ticktimes[ticktimeptr] = backend->ticks() - tickdata;
	ticktimeptr++;
	if (ticktimeptr == 10)
		ticktimeptr = 0;
	float avgtime = 0;
	for (unsigned int ticktime : ticktimes)
		avgtime += ((float)ticktime / world.ticktime);
	world.pace = avgtime / 10;

	world.race = backend->ticks() - lasttimestamp;
	lasttimestamp = backend->ticks();
}

bool Engine::tick() {
	assert(backend);

	// tick if necessary
	bool needupdate = fastticks || !backend->ticks() || (backend->ticks() - tickdata >= world.ticktime - 5);
	if (needupdate && !world.paused) {
		if (fastticks) {
			using clock = std::chrono::steady_clock;
			using std::chrono::duration_cast;
			using std::chrono::milliseconds;
			auto start = clock::now();
			while (duration_cast<milliseconds>(clock::now() - start).count() < 1000 / world.ticktime) {
				update();
			}
		} else {
			update();
		}
	}

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
	caosValue v = world.variables["engine_wasd"];
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
				world.variables["engine_wasd_scrolling"] = caosValue(0);
				wasdMode = false;
				break;
		}
	}

	// check keys
	bool leftdown = backend->keyDown(37) || (wasdMode && a_down);
	bool rightdown = backend->keyDown(39) || (wasdMode && d_down);
	bool updown = backend->keyDown(38) || (wasdMode && w_down);
	bool downdown = backend->keyDown(40) || (wasdMode && s_down);

	if (leftdown)
		velx -= accelspeed;
	if (rightdown)
		velx += accelspeed;
	if (!leftdown && !rightdown) {
		velx *= decelspeed;
		if (fabs(velx) < 0.1)
			velx = 0;
	}
	if (updown)
		vely -= accelspeed;
	if (downdown)
		vely += accelspeed;
	if (!updown && !downdown) {
		vely *= decelspeed;
		if (fabs(vely) < 0.1)
			vely = 0;
	}

	// enforced maximum speed
	if (velx >= maxspeed)
		velx = maxspeed;
	else if (velx <= -maxspeed)
		velx = -maxspeed;
	if (vely >= maxspeed)
		vely = maxspeed;
	else if (vely <= -maxspeed)
		vely = -maxspeed;

	// do the actual movement
	if (velx || vely) {
		int adjustx = engine.camera->getX(), adjusty = engine.camera->getY();
		int adjustbyx = (int)velx, adjustbyy = (int)vely;

		engine.camera->moveTo(adjustx + adjustbyx, adjusty + adjustbyy, jump);
	}
}

void Engine::processEvents() {
	net->handleEvents();

	BackendEvent event;
	while (backend->pollEvent(event)) {
		switch (event.type) {
			case eventresizewindow:
				handleResizedWindow();
				break;

			case eventmousemove:
				handleMouseMove(event);
				break;

			case eventmousebuttonup:
			case eventmousebuttondown:
				handleMouseButton(event);
				break;

			case eventtextinput:
				handleTextInput(event);
				break;

			case eventrawkeydown:
				handleRawKeyDown(event);
				break;

			case eventrawkeyup:
				handleRawKeyUp(event);
				break;

			case eventquit:
				done = true;
				break;

			default:
				break;
		}
	}
}

void Engine::handleResizedWindow() {
	// notify agents
	for (auto& a : world.agents) {
		if (!a)
			continue;
		a->queueScript(123, 0); // window resized script
	}
}

void Engine::handleMouseMove(BackendEvent& event) {
	// move the cursor
	world.hand()->handleEvent(event);

	// notify agents
	for (std::list<std::shared_ptr<Agent>>::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i)
			continue;
		if ((*i)->imsk_mouse_move) {
			caosValue x;
			x.setFloat(world.hand()->pointerX());
			caosValue y;
			y.setFloat(world.hand()->pointerY());
			(*i)->queueScript(75, 0, x, y); // Raw Mouse Move
		}
	}
}

void Engine::handleMouseButton(BackendEvent& event) {
	// notify agents
	for (auto& agent : world.agents) {
		if (!agent)
			continue;
		if ((event.type == eventmousebuttonup && agent->imsk_mouse_up) ||
			(event.type == eventmousebuttondown && agent->imsk_mouse_down)) {
			// set the button value as necessary
			caosValue button;
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
					agent->queueScript(77, 0, button); // Raw Mouse Up
				else
					agent->queueScript(76, 0, button); // Raw Mouse Down
			}
		}
		if ((event.type == eventmousebuttondown &&
				(event.button == buttonwheelup || event.button == buttonwheeldown) &&
				agent->imsk_mouse_wheel)) {
			// fire the mouse wheel event with the relevant delta value
			caosValue delta;
			if (event.button == buttonwheeldown)
				delta.setInt(-120);
			else
				delta.setInt(120);
			agent->queueScript(78, 0, delta); // Raw Mouse Wheel
		}
	}

	world.hand()->handleEvent(event);
}

void Engine::handleTextInput(BackendEvent& event) {
	std::string cp1252_text;
	try {
		cp1252_text = utf8_to_cp1252(event.text);
	} catch (std::domain_error& e) {
		// ignore, will be printed next
	}
	if (cp1252_text.size() != 1) {
		fmt::print(stderr, "bad text input: ");
		for (unsigned char c : event.text) {
			fmt::print(stderr, "0x{:02x} ", c);
		}
		fmt::print(stderr, "\n");
	}

	int translated_char = cp1252_text[0];

	if (cp1252_isprint(translated_char)) {
		if (version < 3 && !world.focusagent) {
			Bubble::newBubble(world.hand(), false, std::string());
		}
	}

	// tell the agent with keyboard focus
	if (world.focusagent) {
		CompoundPart* t = world.focusagent.get()->part(world.focuspart);
		if (t && t->canGainFocus())
			t->handleTranslatedChar(translated_char);
	}

	// notify agents
	caosValue k;
	k.setInt(translated_char);
	for (auto& agent : world.agents) {
		if (!agent)
			continue;
		if (agent->imsk_translated_char)
			agent->queueScript(79, 0, k); // translated char script
	}
}

void Engine::handleRawKeyUp(BackendEvent& event) {
	switch (event.key) {
		case OPENC2E_KEY_W:
			w_down = false;
			break;
		case OPENC2E_KEY_A:
			a_down = false;
			break;
		case OPENC2E_KEY_S:
			s_down = false;
			break;
		case OPENC2E_KEY_D:
			d_down = false;
			break;
		default:
			break;
	}
}

void Engine::handleRawKeyDown(BackendEvent& event) {
	switch (event.key) {
		case OPENC2E_KEY_W:
			w_down = true;
			break;
		case OPENC2E_KEY_A:
			a_down = true;
			break;
		case OPENC2E_KEY_S:
			s_down = true;
			break;
		case OPENC2E_KEY_D:
			d_down = true;
			break;
		default:
			break;
	}

	// handle debug keys, if they're enabled
	caosValue v = world.variables["engine_debug_keys"];
	if (v.hasInt() && v.getInt() == 1) {
		if (backend->keyDown(OPENC2E_KEY_SHIFT)) {
			MetaRoom* n; // for pageup/pagedown

			switch (event.key) {
				case OPENC2E_KEY_INSERT:
					world.showrooms = !world.showrooms;
					break;

				case OPENC2E_KEY_PAUSE:
					// TODO: debug pause game
					break;

				case OPENC2E_KEY_SPACE:
					// TODO: force tick
					break;

				case OPENC2E_KEY_PAGEUP:
					// TODO: previous metaroom
					if ((world.map->getMetaRoomCount() - 1) == engine.camera->getMetaRoom()->id)
						break;
					n = world.map->getMetaRoom(engine.camera->getMetaRoom()->id + 1);
					if (n)
						engine.camera->goToMetaRoom(n->id);
					break;

				case OPENC2E_KEY_PAGEDOWN:
					// TODO: next metaroom
					if (engine.camera->getMetaRoom()->id == 0)
						break;
					n = world.map->getMetaRoom(engine.camera->getMetaRoom()->id - 1);
					if (n)
						engine.camera->goToMetaRoom(n->id);
					break;

				default: break; // to shut up warnings
			}
		}
	}

	// tell the agent with keyboard focus
	if (world.focusagent) {
		CompoundPart* t = world.focusagent.get()->part(world.focuspart);
		if (t && t->canGainFocus())
			t->handleRawKey(event.key);
	}

	// notify agents
	caosValue k;
	k.setInt(event.key);
	for (auto& agent : world.agents) {
		if (!agent)
			continue;
		if (agent->imsk_key_down)
			agent->queueScript(73, 0, k); // key down script
	}

	// certain raw keys get passed as translated chars, too, after the raw key event
	// these correspond to the CP1252/ASCII control codes
	// TODO: should this be handled in Backend instead?
	BackendEvent translatedevent;
	translatedevent.type = eventtextinput;
	switch (event.key) {
		case OPENC2E_KEY_BACKSPACE:
			translatedevent.text = "\x08";
			break;
		case OPENC2E_KEY_TAB:
			translatedevent.text = "\t";
			break;
		case OPENC2E_KEY_RETURN:
			translatedevent.text = "\n";
			break;
		case OPENC2E_KEY_ESCAPE:
			translatedevent.text = "\x1b";
			break;
		case OPENC2E_KEY_DELETE:
			translatedevent.text = "\x7f";
			break;
		default:
			break;
	}
	if (translatedevent.text.size()) {
		handleTextInput(translatedevent);
	}
}

static const char data_default[] = "./data";

static void opt_version() {
	// We already showed the primary version bit, just throw in some random legalese
	std::cout << "This is free software; see the source for copying conditions.  There is NO" << std::endl
			  << "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << std::endl
			  << std::endl
			  << "...please don't sue us." << std::endl;
}

static std::string detectGameType(fs::path directory) {
	if (!case_insensitive_filesystem::resolve_filename(directory / "creatures.exe").empty()) {
		return "c1";
	}
	if (!case_insensitive_filesystem::resolve_filename(directory / "creatures2.exe").empty()) {
		return "c2";
	}

	auto catalogue_directory = directory / "Catalogue";
	auto machine_cfg_filename = case_insensitive_filesystem::resolve_filename(directory / "machine.cfg");
	if (!machine_cfg_filename.empty()) {
		auto machine_cfg = readcfgfile(machine_cfg_filename);
		if (machine_cfg.count("Catalogue Directory")) {
			catalogue_directory = machine_cfg["Catalogue Directory"];
			if (!catalogue_directory.is_absolute()) {
				catalogue_directory = directory / catalogue_directory;
			}
		}
	}

	if (!case_insensitive_filesystem::resolve_filename(catalogue_directory / "Docking Station.catalogue").empty()) {
		return "ds";
	}
	if (!case_insensitive_filesystem::resolve_filename(catalogue_directory / "Creatures 3.catalogue").empty()) {
		return "c3";
	}
	if (!case_insensitive_filesystem::resolve_filename(catalogue_directory / "Creatures Adventures.catalogue").empty()) {
		return "cv";
	}
	if (!case_insensitive_filesystem::resolve_filename(catalogue_directory / "Sea Monkeys.catalogue").empty()) {
		return "sm";
	}
	throw Exception("Couldn't auto-detect game type");
}

static std::vector<DataDirectory> data_directories_from_machine_cfg(fs::path machine_cfg_filename) {
	auto parent_directory = machine_cfg_filename.parent_path();
	machine_cfg_filename = case_insensitive_filesystem::resolve_filename(machine_cfg_filename);
	if (machine_cfg_filename.empty()) {
		return {parent_directory};
	}

	std::vector<DataDirectory> result;
	auto normalize = [&](fs::path s) {
		if (s.empty()) {
			return s;
		}
		if (s.is_absolute()) {
			return s.lexically_normal();
		}
		return fs::absolute((parent_directory / s).lexically_normal());
	};
	auto machine_cfg = readcfgfile(machine_cfg_filename);
	if (!machine_cfg.count("Main Directory")) {
		throw Exception("machine.cfg missing \"Main Directory\"");
	}
	DataDirectory dir(normalize(machine_cfg["Main Directory"]));
	dir.backgrounds = normalize(machine_cfg["Backgrounds Directory"]);
	dir.body_data = normalize(machine_cfg["Body Data Directory"]);
	dir.bootstrap = normalize(machine_cfg["Bootstrap Directory"]);
	dir.catalogue = normalize(machine_cfg["Catalogue Directory"]);
	dir.creature_galleries = normalize(machine_cfg["Creature Galleries Directory"]);
	dir.exported_creatures = normalize(machine_cfg["Exported Creatures Directory"]);
	dir.genetics = normalize(machine_cfg["Genetics Directory"]);
	dir.images = normalize(machine_cfg["Images Directory"]);
	dir.journal = normalize(machine_cfg["Journal Directory"]);
	dir.overlay_data = normalize(machine_cfg["Overlay Data Directory"]);
	dir.agents = normalize(machine_cfg["Resource Files Directory"]);
	dir.sounds = normalize(machine_cfg["Sounds Directory"]);
	dir.users = normalize(machine_cfg["Users Directory"]);
	dir.worlds = normalize(machine_cfg["Worlds Directory"]);
	result.push_back(dir);

	for (int i = 1;; ++i) {
		std::string prefix = fmt::format("Auxiliary {} ", i);
		if (!find_if(machine_cfg, [&](auto& kv) { return kv.first.rfind(prefix) == 0; })) {
			break;
		}
		if (!machine_cfg.count(prefix + "Main Directory")) {
			throw Exception(fmt::format("machine.cfg missing \"{}Main Directory\"", prefix));
		}
		DataDirectory dir(normalize(machine_cfg[prefix + "Main Directory"]));
		dir.backgrounds = normalize(machine_cfg[prefix + "Backgrounds Directory"]);
		dir.body_data = normalize(machine_cfg[prefix + "Body Data Directory"]);
		dir.bootstrap = normalize(machine_cfg[prefix + "Bootstrap Directory"]);
		dir.catalogue = normalize(machine_cfg[prefix + "Catalogue Directory"]);
		dir.creature_galleries = normalize(machine_cfg[prefix + "Creature Galleries Directory"]);
		dir.exported_creatures = normalize(machine_cfg[prefix + "Exported Creatures Directory"]);
		dir.genetics = normalize(machine_cfg[prefix + "Genetics Directory"]);
		dir.images = normalize(machine_cfg[prefix + "Images Directory"]);
		dir.journal = normalize(machine_cfg[prefix + "Journal Directory"]);
		dir.overlay_data = normalize(machine_cfg[prefix + "Overlay Data Directory"]);
		dir.agents = normalize(machine_cfg[prefix + "Resource Files Directory"]);
		dir.sounds = normalize(machine_cfg[prefix + "Sounds Directory"]);
		dir.users = normalize(machine_cfg[prefix + "Users Directory"]);
		dir.worlds = normalize(machine_cfg[prefix + "Worlds Directory"]);
		result.push_back(dir);
	}
	return result;
}

#ifdef _WIN32
static fs::path showDirectoryPicker() {
	if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) {
		// TODO: handle RPC_E_CHANGED_MODE, we can continu w/ a multithreaded COM.
		return {};
	}

	struct coinit_scope {
		~coinit_scope() { CoUninitialize(); }
	} coinit_scope_instance;

	ComPtr<IFileOpenDialog> fo;
	if (FAILED(CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, fo.receive_vpp()))) {
		std::cout << "Couldn't open File Dialog" << std::endl;
		return {};
	}
	fo->SetOptions(FOS_PICKFOLDERS);

	// Show the Open dialog box.
	if (FAILED(fo->Show(NULL))) {
		std::cout << "Couldn't show File Open Dialog" << std::endl;
		return {};
	}

	// Get the file name from the dialog box.
	ComPtr<IShellItem> pItem;
	if (FAILED(fo->GetResult(pItem.receive()))) {
		return {};
	}

	ComHeapPtr<WCHAR> pszFilePath;
	if (FAILED(pItem->GetDisplayName(SIGDN_FILESYSPATH, pszFilePath.receive()))) {
		return {};
	}

	return fs::path(pszFilePath.get());
}
#endif

bool Engine::parseCommandLine(int argc, char* argv[]) {
	// variables for command-line flags
	std::vector<std::string> data_vec;

	// generate help for backend options
	std::string available_backends;
	for (auto& possible_backend : possible_backends) {
		if (available_backends.empty())
			available_backends = possible_backend.first;
		else
			available_backends += ", " + possible_backend.first;
	}
	available_backends = "Select the backend (options: " + available_backends + "); default is " + preferred_backend;

	std::string available_audiobackends;
	for (auto& possible_audiobackend : possible_audiobackends) {
		if (available_audiobackends.empty())
			available_audiobackends = possible_audiobackend.first;
		else
			available_audiobackends += ", " + possible_audiobackend.first;
	}
	available_audiobackends = "Select the audio backend (options: " + available_audiobackends + "); default is " + preferred_audiobackend;

	// parse the command-line flags
	cxxopts::Options desc("openc2e", "");
	desc.add_options()("h,help", "Display help on command-line options");
	desc.add_options()("V,version", "Display openc2e version");
	desc.add_options()("s,silent", "Disable all sounds");
	desc.add_options()("l,language", "Select the language; default is '" + language + "'", cxxopts::value<std::string>(language));
	desc.add_options()("k,backend", available_backends, cxxopts::value<std::string>(preferred_backend));
	desc.add_options()("o,audiobackend", available_audiobackends, cxxopts::value<std::string>(preferred_audiobackend));
	desc.add_options()("d,data-path", "Sets or adds a path to a data directory", cxxopts::value<std::vector<std::string>>(data_vec));
	desc.add_options()("b,bootstrap", "Sets or adds a path or COS file to bootstrap from", cxxopts::value<std::vector<std::string>>(cmdline_bootstrap));
	desc.add_options()("m,gamename", "Set the game name", cxxopts::value<std::string>(gamename));
	desc.add_options()("n,norun", "Don't run the game, just execute scripts");
	desc.add_options()("a,autokill", "Enable autokill");
	desc.add_options()("autostop", "Enable autostop (or disable it, for CV)");
	auto vm = desc.parse(argc, argv);
	cmdline_enable_sound = !vm.count("silent");
	cmdline_norun = vm.count("norun");

	if (vm.count("help")) {
		std::cout << desc.help() << std::endl;
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
#ifdef _WIN32
		fs::path picked_path = showDirectoryPicker();
		if (!picked_path.empty()) {
			data_vec.push_back(picked_path);
		}
#endif
		if (data_vec.empty()) {
			std::cout << "Warning: No data path specified, trying default of '" << data_default << "', see --help if you need to specify one." << std::endl;
			data_vec.push_back(data_default);
		}
	}

	// detect game type from first data directory
	gametype = detectGameType(data_vec[0]).c_str();
	fmt::print("* Detected game type: {}\n", gametype);

	// set engine version
	// TODO: set gamename
	if (gametype == "c1") {
		if (gamename.empty())
			gamename = "Creatures 1";
		version = 1;
	} else if (gametype == "c2") {
		if (gamename.empty())
			gamename = "Creatures 2";
		version = 2;
	} else if (gametype == "c3") {
		if (gamename.empty()) {
			gamename = "Creatures 3";
		}
		version = 3;
	} else if (gametype == "ds") {
		gametype = "c3";
		if (gamename.empty()) {
			gamename = "Docking Station";
		}
		version = 3;
	} else if (gametype == "cv") {
		if (gamename.empty())
			gamename = "Creatures Village";
		version = 3;
		world.autostop = !world.autostop;
	} else if (gametype == "sm") {
		if (gamename.empty())
			gamename = "Sea-Monkeys";
		version = 3;
		bmprenderer = true;
	} else
		throw Exception(fmt::format("unknown gametype '{}'!", gametype));

	// try to read machine.cfg
	if (!fs::exists(data_vec[0])) {
		throw Exception("data path '" + data_vec[0] + "' doesn't exist");
	}
	if (engine.version == 3) {
		data_directories = data_directories_from_machine_cfg(fs::path(data_vec[0]) / "machine.cfg");
	} else {
		data_directories.push_back(fs::path(data_vec[0]));
	}

	// add remaining data directories
	for (auto it = data_vec.begin() + 1; it != data_vec.end(); ++it) {
		if (!fs::exists(*it)) {
			throw Exception("data path '" + *it + "' doesn't exist");
		}
		if (find_if(data_directories, [&](auto d) { return fs::absolute(d.main) == fs::absolute(*it); })) {
			printf("* Warning: ignoring duplicate data directory %s\n", it->c_str());
			continue;
		}
		data_directories.push_back(fs::path(*it));
	}

	// make a vague attempt at blacklisting some characters inside the gamename
	// (it's used in directory names, registry keys, etc)
	std::string invalidchars = "\\/:*?\"<>|";
	for (char invalidchar : invalidchars) {
		if (gamename.find(invalidchar) != gamename.npos)
			throw Exception(std::string("The character ") + invalidchar + " is not valid in a gamename.");
	}

	return true;
}

bool Engine::initialSetup() {
	assert(data_directories.size() > 0);

	// finally, add our cache directory to the end
	data_directories.push_back(storageDirectory());

	// initialize backends
	if (cmdline_norun)
		preferred_backend = "null";
	if (preferred_backend != "null")
		std::cout << "* Initialising backend " << preferred_backend << "..." << std::endl;
	std::shared_ptr<Backend> b = possible_backends[preferred_backend];
	if (!b)
		throw Exception("No such backend " + preferred_backend);
	b->init();
	setBackend(b);
	possible_backends.clear();

	if (cmdline_norun)
		preferred_audiobackend = "null";
	if (preferred_audiobackend != "null")
		std::cout << "* Initialising audio backend " << preferred_audiobackend << "..." << std::endl;
	std::shared_ptr<AudioBackend> a = possible_audiobackends[preferred_audiobackend];
	if (!a)
		throw Exception("No such audio backend " + preferred_audiobackend);
	try {
		a->init();
		audio = a;
	} catch (Exception& e) {
		std::cerr << "* Couldn't initialize backend " << preferred_audiobackend << ": " << e.what() << std::endl
				  << "* Continuing without sound." << std::endl;
		audio = std::shared_ptr<AudioBackend>(new NullAudioBackend());
		audio->init();
	}
	possible_audiobackends.clear();

	net = std::make_shared<NetBackend>();
	int listenport = net->init();
	if (listenport != -1) {
		// inform the user of the port used, and store it in the relevant file
		std::cout << "* Listening for connections on port " << listenport << "." << std::endl;
#ifndef _WIN32
		fs::path p = fs::path(homeDirectory().string() + "/.creaturesengine");
		if (!fs::exists(p))
			fs::create_directory(p);
		if (fs::is_directory(p)) {
			std::ofstream f((p.string() + "/port").c_str(), std::ios::trunc);
			f << std::to_string(listenport);
		}
#endif
	}

	// load palette for C1
	world.gallery->loadDefaultPalette();

	// audio
	musicmanager = std::make_unique<MusicManager>(audio);
	if (!cmdline_enable_sound) {
		soundmanager.setMuted(true);
		musicmanager->setMuted(true);
		musicmanager->setMIDIMuted(true);
	}

	// initial setup
	if (engine.version == 3) {
		std::cout << "* Reading catalogue files..." << std::endl;
		world.initCatalogue();
	}
	std::cout << "* Initial setup..." << std::endl;
	world.init(); // just reads mouse cursor (we want this after the catalogue reading so we don't play "guess the filename")
	if (engine.version > 2) {
		std::cout << "* Reading PRAY files..." << std::endl;
		world.praymanager->update();
	}

#ifdef _WIN32
	// Here we need to set the working directory since apparently windows != clever
	char exepath[MAX_PATH] = "";
	GetModuleFileName(0, exepath, sizeof(exepath) - 1);
	char* exedir = strrchr(exepath, '\\');
	if (exedir) {
		// null terminate the string
		*exedir = 0;
		// Set working directory
		SetCurrentDirectory(exepath);
	} else // err, oops
		std::cerr << "Warning: Setting working directory to " << exepath << " failed.";
#endif

	loadGameData();

	// execute the initial scripts!
	std::cout << "* Executing initial scripts..." << std::endl;
	if (cmdline_bootstrap.size() == 0) {
		world.executeBootstrap(false);
	} else {
		std::vector<std::string> scripts;

		if (engine.version < 3 && cmdline_bootstrap.size() != 1)
			throw Exception("multiple bootstrap files provided in C1/C2 mode");

		for (auto& bsi : cmdline_bootstrap) {
			fs::path scriptdir(bsi);
			if (engine.version > 2 || scriptdir.extension().string() == ".cos") {
				// pass it to the world to execute (it handles both files and directories)

				if (!fs::exists(scriptdir)) {
					std::cerr << "Warning: Couldn't find a specified script directory (trying " << bsi << ")!\n";
					continue;
				}

				world.executeBootstrap(scriptdir);
			} else {
				// in c1/c2 mode, if not a cos file, assume it's an SFC file
				if (!fs::exists(scriptdir) || fs::is_directory(scriptdir))
					throw Exception("non-existant bootstrap file provided in C1/C2 mode");
				// TODO: the default SFCFile loading code is in World, maybe this should be too..
				SFCFile sfc;
				std::ifstream f(scriptdir.string().c_str(), std::ios::binary);
				f >> std::noskipws;
				sfc.read(&f);
				sfc.copyToWorld();
			}
		}
	}

	// if there aren't any metarooms, we can't run a useful game, the user probably
	// wanted to execute a CAOS script or something went badly wrong.
	if (!cmdline_norun && world.map->getMetaRoomCount() == 0) {
		shutdown();
		throw Exception("No metarooms found in given bootstrap directories or files");
	}

	std::cout << "* Done startup." << std::endl;

	if (cmdline_norun) {
		// TODO: see comment above about avoiding backend when norun is set
		std::cout << "Told not to run the world, so stopping now." << std::endl;
		shutdown();
		return false;
	}

	// Let agents know the window size (makes the DS sound options panel update
	// to match actual engine state when starting muted)
	// TODO: does this happen in real c2e?
	handleResizedWindow();

	return true;
}

void Engine::shutdown() {
	world.shutdown();
	audio->shutdown();
	backend->shutdown();
	net->shutdown();
}

/* vim: set noet: */
