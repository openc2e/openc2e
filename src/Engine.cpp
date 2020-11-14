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

#include "Bubble.h"
#include "Room.h"
#include "Engine.h"
#include "World.h"
#include "Map.h"
#include "MetaRoom.h"
#include "MusicManager.h"
#include "SoundManager.h"
#include "NetBackend.h"
#include "caosVM.h" // for setupCommandPointers()
#include "caosScript.h" // for executeNetwork()
#include "PointerAgent.h"
#include "audiobackend/NullAudioBackend.h"
#include "backends/NullBackend.h"
#include "encoding.h"
#include "keycodes.h"
#include "SFCFile.h"
#include "fileformats/peFile.h"
#include "Camera.h"
#include "imageManager.h"
#include "prayManager.h"
#include "userlocale.h"

#include <cassert>
#include <ghc/filesystem.hpp>
#define CXXOPTS_VECTOR_DELIMITER '\0'
#include <cxxopts.hpp>
#include <fmt/core.h>
#include <memory>
#include <stdexcept>
namespace fs = ghc::filesystem;

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

	std::vector<std::string> languages = get_preferred_languages();
	if (!languages.empty()) {
		language = languages.front();
	} else {
		language = "en";
	}

	tickdata = 0;
	for (unsigned int i = 0; i < 10; i++) ticktimes[i] = 0;
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

static std::vector<std::string> read_wordlist(peFile* exefile, uint32_t lang) {
	resourceInfo *r = exefile->getResource(PE_RESOURCETYPE_STRING, lang, 14);
	if (!r) {
		std::cout << "Warning: Couldn't load word list (couldn't find resource)!" << std::endl;
		return {};
	}

	std::vector<std::string> strings = r->parseStrings();
	if (strings.size() < 6) {
		std::cout << "Warning: Couldn't load word list (string table too small)!" << std::endl;
		return {};
	}

	std::vector<std::string> wordlist;
	std::string wordlistdata = strings[5];

	std::string s;
	for (unsigned int i = 0; i < wordlistdata.size(); i++) {
		if (wordlistdata[i] == '|') {
			wordlist.push_back(s);
			s.clear();
		} else s += wordlistdata[i];
	}

	return wordlist;
}

void Engine::loadGameData() {
	// load word list translations for C1
	// C1 does not keep translations for all languages, so we embed them
	if (gametype == "c1") {
		if (language == "de") {
			wordlist_translations = std::map<std::string, std::string>{
				{ "come", "komm" },
				{ "drop", "lass" },
				{ "get", "hol" },
				{ "left", "links" },
				{ "look", "guck" },
				{ "no", "nein" },
				{ "pull", "zieh" },
				{ "push", "druck" },
				{ "right", "rechts" },
				{ "run", "lauf" },
				{ "sleep", "raste" },
				{ "stop", "halt" },
				{ "what", "was" },
				{ "yes", "ja" }
			};
		} else if (language == "fr") {
			wordlist_translations = std::map<std::string, std::string>{
				{ "come", "venir" },
				{ "drop", "lacher" },
				{ "get", "prendre" },
				{ "left", "gauche" },
				{ "look", "regarder" },
				{ "no", "non" },
				{ "pull", "tirer" },
				{ "push", "pousser" },
				{ "right", "droite" },
				{ "run", "courir" },
				{ "sleep", "se-reposer" },
				{ "stop", "arreter" },
				{ "what", "quoi" },
				{ "yes", "oui" }
			};
		} else if (language == "it") {
			wordlist_translations = std::map<std::string, std::string>{
				{ "come", "vieni" },
				{ "drop", "molla" },
				{ "get", "prendi" },
				{ "left", "sinistra" },
				{ "look", "guarda" },
				{ "no", "no" },
				{ "pull", "tira" },
				{ "push", "premi" },
				{ "right", "destra" },
				{ "run", "corri" },
				{ "sleep", "riposa" },
				{ "stop", "stop" },
				{ "what", "cosa" },
				{ "yes", "si" }
			};
		} else if (language == "nl") {
			wordlist_translations = std::map<std::string, std::string>{
				{ "come", "komen" },
				{ "drop", "latenvallen" },
				{ "get", "pakken" },
				{ "left", "links" },
				{ "look", "kijken" },
				{ "no", "nee" },
				{ "pull", "trekken" },
				{ "push", "duwen" },
				{ "right", "rechts" },
				{ "run", "rennen" },
				{ "sleep", "rusten" },
				{ "stop", "stoppen" },
				{ "what", "wat" },
				{ "yes", "ja" }
			};
		} else if (language == "es") {
			wordlist_translations = std::map<std::string, std::string>{
				{ "come", "venir" },
				{ "drop", "soltar" },
				{ "get", "coger" },
				{ "left", "izquierda" },
				{ "look", "mirar" },
				{ "no", "no" },
				{ "pull", "tirar" },
				{ "push", "empujar" },
				{ "right", "derecha" },
				{ "run", "correr" },
				{ "sleep", "descansar" },
				{ "stop", "parar" },
				{ "what", "que" },
				{ "yes", "si" }
			};
		}
	}

	// load word list for C2
	if (gametype == "c2") {
		fs::path exepath(world.findFile("Creatures2.exe"));
		if (fs::exists(exepath) && !fs::is_directory(exepath)) {
			try {
				exefile = new peFile(exepath);
			} catch (creaturesException &e) {
				std::cout << "Warning: Couldn't load word list (" << e.what() << ")!" << std::endl;
			}
		} else std::cout << "Warning: Couldn't load word list (couldn't find Creatures2.exe)!" << std::endl;

		if (exefile) {
			auto english_wordlist = read_wordlist(exefile, HORRID_LANG_ENGLISH);

			wordlist.clear();
			wordlist_translations.clear();
			if (language == "de") {
				wordlist = read_wordlist(exefile, HORRID_LANG_GERMAN);
			} else if (language == "fr") {
				wordlist = read_wordlist(exefile, HORRID_LANG_FRENCH);
			} else if (language == "it") {
				wordlist = read_wordlist(exefile, HORRID_LANG_ITALIAN);
			} else if (language == "nl") {
				wordlist = read_wordlist(exefile, HORRID_LANG_DUTCH);
			} else if (language == "es") {
				wordlist = read_wordlist(exefile, HORRID_LANG_SPANISH);
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
	} catch (creaturesException &e) {
		vm.outputstream = 0; // otherwise would point to dead stack
		return std::string("### EXCEPTION: ") + e.what();
	}
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
	musicmanager.tick();

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
		int adjustx = engine.camera->getX(), adjusty = engine.camera->getY();
		int adjustbyx = (int)velx, adjustbyy = (int) vely;
			
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
		if (!a) continue;
		a->queueScript(123, 0); // window resized script
	}
}

void Engine::handleMouseMove(BackendEvent &event) {
	// move the cursor
	world.hand()->handleEvent(event);

	// notify agents
	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;
		if ((*i)->imsk_mouse_move) {
			caosValue x; x.setFloat(world.hand()->pointerX());
			caosValue y; y.setFloat(world.hand()->pointerY());
			(*i)->queueScript(75, 0, x, y); // Raw Mouse Move
		}
	}
}

void Engine::handleMouseButton(BackendEvent &event) {
	// notify agents
	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;
		if ((event.type == eventmousebuttonup && (*i)->imsk_mouse_up) ||
			(event.type == eventmousebuttondown && (*i)->imsk_mouse_down)) {
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
					(*i)->queueScript(77, 0, button); // Raw Mouse Up
				else
					(*i)->queueScript(76, 0, button); // Raw Mouse Down
			}
		}
		if ((event.type == eventmousebuttondown &&
			(event.button == buttonwheelup || event.button == buttonwheeldown) &&
			(*i)->imsk_mouse_wheel)) {
			// fire the mouse wheel event with the relevant delta value
			caosValue delta;
			if (event.button == buttonwheeldown)
				delta.setInt(-120);
			else
				delta.setInt(120);
			(*i)->queueScript(78, 0, delta); // Raw Mouse Wheel
		}
	}

	world.hand()->handleEvent(event);
}

void Engine::handleTextInput(BackendEvent &event) {
	std::string cp1252_text;
	try {
		 cp1252_text = utf8_to_cp1252(event.text);
	 } catch (std::domain_error &e) {
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
		CompoundPart *t = world.focusagent.get()->part(world.focuspart);
		if (t && t->canGainFocus())
			t->handleTranslatedChar(translated_char);
	}

	// notify agents
	caosValue k;
	k.setInt(translated_char);
	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;
		if ((*i)->imsk_translated_char)
			(*i)->queueScript(79, 0, k); // translated char script
	}
}

void Engine::handleRawKeyUp(BackendEvent &event) {
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

void Engine::handleRawKeyDown(BackendEvent &event) {
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
			MetaRoom *n; // for pageup/pagedown

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
		CompoundPart *t = world.focusagent.get()->part(world.focuspart);
		if (t && t->canGainFocus())
			t->handleRawKey(event.key);
	}

	// notify agents
	caosValue k;
	k.setInt(event.key);
	for (std::list<std::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if (!*i) continue;
		if ((*i)->imsk_key_down)
			(*i)->queueScript(73, 0, k); // key down script
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
	for (std::map<std::string, std::shared_ptr<Backend> >::iterator i = possible_backends.begin(); i != possible_backends.end(); i++) {
		if (available_backends.empty()) available_backends = i->first;
		else available_backends += ", " + i->first;
	}
	available_backends = "Select the backend (options: " + available_backends + "); default is " + preferred_backend;
	
	std::string available_audiobackends;
	for (std::map<std::string, std::shared_ptr<AudioBackend> >::iterator i = possible_audiobackends.begin(); i != possible_audiobackends.end(); i++) {
		if (available_audiobackends.empty()) available_audiobackends = i->first;
		else available_audiobackends += ", " + i->first;
	}
	available_audiobackends = "Select the audio backend (options: " + available_audiobackends + "); default is " + preferred_audiobackend;
	
	// parse the command-line flags
	cxxopts::Options desc("openc2e", "");
	desc.add_options()
		("h,help", "Display help on command-line options")
		("V,version", "Display openc2e version")
		("s,silent", "Disable all sounds")
		("l,language", "Select the language; default is '" + language + "'", cxxopts::value<std::string>(language))
		("k,backend", available_backends, cxxopts::value<std::string>(preferred_backend))
		("o,audiobackend", available_audiobackends, cxxopts::value<std::string>(preferred_audiobackend))
		("d,data-path", "Sets or adds a path to a data directory",
		 cxxopts::value<std::vector<std::string>>(data_vec))
		("b,bootstrap", "Sets or adds a path or COS file to bootstrap from",
		 cxxopts::value<std::vector<std::string>>(cmdline_bootstrap))
		("g,gametype", "Set the game type (options: c1, c2, c3, cv, sm); if unspecified the engine will try to detect it automatically or fall back to c3", cxxopts::value<std::string>(gametype))
		("m,gamename", "Set the game name", cxxopts::value<std::string>(gamename))
		("n,norun", "Don't run the game, just execute scripts")
		("a,autokill", "Enable autokill")
		("autostop", "Enable autostop (or disable it, for CV)")
		;
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

		HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED |
			COINIT_DISABLE_OLE1DDE);

		if (SUCCEEDED(hr)) {
			struct coinit_scope {
				~coinit_scope() { CoUninitialize(); }
			} coinit_scope_instance;

			IFileOpenDialog* fo;
			hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL,
				IID_IFileOpenDialog, reinterpret_cast<void**>(&fo));

			if (SUCCEEDED(hr)) {
				fo->SetOptions(FOS_PICKFOLDERS);
				// Show the Open dialog box.
				hr = fo->Show(NULL);

				// Get the file name from the dialog box.
				if (SUCCEEDED(hr)) {
					IShellItem* pItem;
					hr = fo->GetResult(&pItem);
					if (SUCCEEDED(hr)) {
						PWSTR pszFilePath;
						hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
						if (SUCCEEDED(hr)) {
							std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> wtos;
							std::string path = wtos.to_bytes(pszFilePath);
							data_vec.push_back(path);
							CoTaskMemFree(pszFilePath);
						}
						pItem->Release();
					}
				}
				else {
					std::cout << "Couldn't show File Open Dialog" << std::endl;
				}
				fo->Release();
			}
			else {
				std::cout << "Couldn't open File Dialog" << std::endl;
			}
		}

		
#endif
		if (data_vec.empty()) {
			std::cout << "Warning: No data path specified, trying default of '" << data_default << "', see --help if you need to specify one." << std::endl;
			data_vec.push_back(data_default);
		}
		
	}

	// add all the data directories to the list
	for (std::vector<std::string>::iterator i = data_vec.begin(); i != data_vec.end(); i++) {
		fs::path datadir(*i);
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
	if (gametype.empty()) {
		std::string msg = "Warning: No gametype specified, ";
		// TODO: is this sane? especially unsure about about.exe
		if (!world.findFile("Creatures.exe").empty()) {
			msg += "found Creatures.exe, assuming C1 (c1)";
			gametype = "c1";
		} else if (!world.findFile("Creatures2.exe").empty()) {
			msg += "found Creatures2.exe, assuming C2 (c2)";
			gametype = "c2";
		} else if (!world.findFile("Sea-Monkeys.ico").empty()) {
			msg += "found Sea-Monkeys.ico, assuming Sea-Monkeys (sm)";
			gametype = "sm";
		} else if (!world.findFile("about.exe").empty()) {
			msg += "found about.exe, assuming CA, CP or CV (cv)";
			gametype = "cv";
		} else {
			msg += "assuming C3/DS (c3)";
			gametype = "c3";
		}
		msg += ", see --help if you need to specify one.\n";
		std::cout << msg;
	}

	// set engine version
	// TODO: set gamename
	if (gametype == "c1") {
		if (gamename.empty()) gamename = "Creatures 1";
		version = 1;
	} else if (gametype == "c2") {
		if (gamename.empty()) gamename = "Creatures 2";
		version = 2;
	} else if (gametype == "c3") {
		if (gamename.empty()) {
			if (!world.findFile("Docking Station.ico").empty()) {
				gamename = "Docking Station";
			} else {
				gamename = "Creatures 3";
			}
		}
		version = 3;
	} else if (gametype == "cv") {
		if (gamename.empty()) gamename = "Creatures Village";
		version = 3;
		world.autostop = !world.autostop;
	} else if (gametype == "sm") {
		if (gamename.empty()) gamename = "Sea-Monkeys";
		version = 3;
		bmprenderer = true;
	} else
		throw creaturesException(fmt::format("unknown gametype '{}'!", gametype));

	// finally, add our cache directory to the end
	world.data_directories.push_back(storageDirectory());
	
	// initialize backends
	if (cmdline_norun) preferred_backend = "null";
	if (preferred_backend != "null") std::cout << "* Initialising backend " << preferred_backend << "..." << std::endl;	
	std::shared_ptr<Backend> b = possible_backends[preferred_backend];
	if (!b)	throw creaturesException("No such backend " + preferred_backend);
	b->init(); setBackend(b);
	possible_backends.clear();

	if (cmdline_norun) preferred_audiobackend = "null";
	if (preferred_audiobackend != "null") std::cout << "* Initialising audio backend " << preferred_audiobackend << "..." << std::endl;	
	std::shared_ptr<AudioBackend> a = possible_audiobackends[preferred_audiobackend];
	if (!a)	throw creaturesException("No such audio backend " + preferred_audiobackend);
	try{
		a->init(); audio = a;
	} catch (creaturesException &e) {
		std::cerr << "* Couldn't initialize backend " << preferred_audiobackend << ": " << e.what() << std::endl << "* Continuing without sound." << std::endl;
		audio = std::shared_ptr<AudioBackend>(new NullAudioBackend());
		audio->init();
	}
	if (!cmdline_enable_sound) {
		soundmanager.setMuted(true);
		musicmanager.setMuted(true);
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
	
	// initial setup
	std::cout << "* Reading catalogue files..." << std::endl;
	world.initCatalogue();
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

	if (world.data_directories.size() < 3) {
		// TODO: This is a hack for DS, basically. Not sure if it works properly. - fuzzie
		eame_variables["engine_no_auxiliary_bootstrap_1"] = caosValue(1);
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
			fs::path scriptdir(*bsi);
			if (engine.version > 2 || scriptdir.extension().string() == ".cos") {
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
		throw creaturesException("No metarooms found in given bootstrap directories or files");
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

fs::path Engine::homeDirectory() {
	fs::path p;

#ifndef _WIN32
	char *envhome = getenv("HOME");
	if (envhome)
		p = fs::path(envhome);
	if ((!envhome) || (!fs::is_directory(p)))
		p = fs::path(getpwuid(getuid())->pw_dir);
	if (!fs::is_directory(p)) {
		std::cerr << "Can't work out what your home directory is, giving up and using /tmp for now." << std::endl;
		p = fs::path("/tmp"); // sigh
	}
#else
	TCHAR szPath[_MAX_PATH];
	SHGetSpecialFolderPath(NULL, szPath, CSIDL_PERSONAL, TRUE);

	p = fs::path(szPath);
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
	fs::path p = fs::path(homeDirectory().string() + dirname);
	if (!fs::exists(p))
		fs::create_directory(p);
	else if (!fs::is_directory(p))
		throw creaturesException("Your openc2e data directory " + p.string() + " is a file, not a directory. That's bad.");

	// game-specific storage dir
	p = fs::path(p.string() + std::string("/" + gamename));
	if (!fs::exists(p))
		fs::create_directory(p);
	else if (!fs::is_directory(p))
		throw creaturesException("Your openc2e game data directory " + p.string() + " is a file, not a directory. That's bad.");

	return p;
}

/* vim: set noet: */
