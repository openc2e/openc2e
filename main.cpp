/*
 *  main.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed 02 Jun 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005-2006 Bryan Donlan. All rights reserved.
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
#include <cstdlib> // EXIT_FAILURE

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#include <sstream> // for istringstream, used in networking code
#include <fstream>
#include "openc2e.h"
#include <iostream>
#include <algorithm>
#include <stdio.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include "World.h"
#include "caosVM.h"
#include "PointerAgent.h"
#include "SDLBackend.h"
#include "dialect.h"

#include <SDL_net.h>

#ifdef _MSC_VER
#undef main // because SDL is stupid
#endif

namespace fs = boost::filesystem;
namespace po = boost::program_options;
extern fs::path homeDirectory(); // creaturesImage.cpp
extern fs::path cacheDirectory(); // creaturesImage.cpp
static const char data_default[] = "./data";

static void opt_version() {
	// We already showed the primary version bit, just throw in some random
	// legalese
	std::cout << 
		"This is free software; see the source for copying conditions.  There is NO" << std::endl <<
		"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << std::endl << std::endl <<
		"...please don't sue us." << std::endl;
}

extern "C" int main(int argc, char *argv[]) {
	srand(time(NULL));

	try {
		
	std::cout << "openc2e (development build), built " __DATE__ " " __TIME__ "\nCopyright (c) 2004-2006 Alyssa Milburn and others\n\n";
	int optret;
	bool enable_sound = true;
	std::vector<std::string> bootstrap;
	std::vector<std::string> data_vec;
	bool bs_specd = false, d_specd = false;
	world.gametype = "c3";

	po::options_description desc;
	desc.add_options()
		("help,h", "Display help on command-line options")
		("version,V", "Display openc2e version")
		("silent,s", "Disable all sounds")
		("data-path,d", po::value< std::vector<std::string> >(&data_vec)->composing(),
		 "Set the path to the data directory")
		("bootstrap,b", po::value< std::vector<std::string> >(&bootstrap)->composing(),
		 "Sets or adds a path or COS file to bootstrap from")
		("gametype,g", po::value< std::string >(&world.gametype), "Set the game type (cv or c3)")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	enable_sound = !vm.count("silent");
	
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	if (vm.count("version")) {
		opt_version();
		return 0;
	}

	if (vm.count("data-path") == 0)
		data_vec.push_back(data_default);

	for (std::vector<std::string>::iterator i = data_vec.begin(); i != data_vec.end(); i++) {
		fs::path datadir(*i, fs::native);
		if (!fs::exists(datadir)) {
			std::cerr << "data path '" << *i << "' doesn't exist, try --help" << std::endl;
			return 1;
		}
		world.data_directories.push_back(datadir);
	}
	
	world.data_directories.push_back(cacheDirectory());
	
	registerDelegates();
	std::cout << "Reading catalogue files..." << std::endl;
	world.initCatalogue();
	std::cout << "Initial setup..." << std::endl;
	world.init(); // just reads mouse cursor (we want this after the catalogue reading so we don't play "guess the filename")
	std::cout << "Reading PRAY files..." << std::endl;
	world.praymanager.update();
	std::cout << "Initialising backend..." << std::endl;
	// moved backend.init() here because we need the camera to be valid - fuzzie
	world.backend.init(enable_sound);
	world.camera.setBackend(&world.backend); // TODO: hrr

	if (world.data_directories.size() < 3) {
		// TODO: This is a hack for DS, basically. Not sure if it works properly. - fuzzie
		caosVar name; name.setString("engine_no_auxiliary_bootstrap_1");
		caosVar contents; contents.setInt(1);
		world.eame_variables[name] = contents;
	}
	
	std::cout << "Executing initial scripts..." << std::endl;
	if (bootstrap.size() == 0) {
		world.executeBootstrap(false);
	} else {
		std::vector<std::string> scripts;
	
		for (std::vector< std::string >::iterator bsi = bootstrap.begin(); bsi != bootstrap.end(); bsi++) {
			fs::path scriptdir(*bsi, fs::native);
			if (!fs::exists(scriptdir)) {
				std::cerr << "couldn't find a specified script directory (trying " << *bsi << ")!\n";
				continue;
			}
			world.executeBootstrap(scriptdir);
		}
	}

	if (world.map.getMetaRoomCount() == 0) {
		std::cerr << "\nNo metarooms found in given bootstrap directories or files, exiting." << std::endl;
		SDL_Quit();
		return 0;
	}

	SDLNet_Init();
	TCPsocket listensocket = 0;
	int listenport = 20000;
	while ((!listensocket) && (listenport < 20050)) {
		listenport++;
		IPaddress ip;

		SDLNet_ResolveHost(&ip, 0, listenport);
		listensocket = SDLNet_TCP_Open(&ip); 
	}
	assert(listensocket);
	
	std::cout << "listening on port " << listenport << std::endl;
	fs::path p = fs::path(homeDirectory().native_directory_string() + "/.creaturesengine", fs::native);
	if (!fs::exists(p))
		fs::create_directory(p);
	if (fs::is_directory(p)) {
		std::ofstream f((p.native_directory_string() + "/port").c_str(), std::ios::trunc);
		f << boost::str(boost::format("%d") % listenport);
	}

	world.drawWorld();

	SDL_EnableUNICODE(1); // bz2 and I both think this is the only way to get useful ascii out of SDL

	bool done = false;
	unsigned int tickdata = 0;
	unsigned int ticktime[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned int ticktimeptr = 0;
	while (!done) {
		bool ticked = false;
		
//        Collectable::doCollect();
		/*
		 we calculate PACE below, but it's inaccurate because drawWorld(), our biggest cpu consumer, isn't in the loop
		 this is because it makes the game seem terribly unresponsive..
		*/
		if (!world.paused && (world.backend.ticks() > (tickdata + world.ticktime))) {
			tickdata = world.backend.ticks();
			
			world.tick();
			//if (world.hand()->carrying) // TODO: do this in world.tick()
			//	world.hand()->carrying->moveTo(world.hand()->x + 2, world.hand()->y + 2);
			world.drawWorld();
			
			ticktime[ticktimeptr] = world.backend.ticks() - tickdata;
			ticktimeptr++;
			if (ticktimeptr == 10) ticktimeptr = 0;
			float avgtime = 0;
			for (unsigned int i = 0; i < 10; i++) avgtime += ((float)ticktime[i] / world.ticktime);
			world.pace = avgtime / 10;

			ticked = true;
		} else SDL_Delay(10);

		while (TCPsocket connection = SDLNet_TCP_Accept(listensocket)) {
			IPaddress *remote_ip = SDLNet_TCP_GetPeerAddress(connection);
			unsigned char *rip = (unsigned char *)&remote_ip->host;
			if ((rip[0] != 127) || (rip[1] != 0) || (rip[2] != 0) || (rip[3] != 1)) {
				std::cout << "Someone tried connecting via non-localhost address! IP: " << (int)rip[0] << "." << (int)rip[1] << "." << (int)rip[2] << "." << (int)rip[3] << std::endl;
				SDLNet_TCP_Close(connection);
				continue;
			}
			
			std::string data;
			bool done = false;

			while (!done) {
				char buffer;
				int i = SDLNet_TCP_Recv(connection, &buffer, 1);
				if (i == 1) {
					data = data + buffer;
					if ((data.size() > 3) && (data.find("rscr", data.size() - 4) != data.npos)) done = true;
				} else done = true;
			}

			std::istringstream s(data);
			try {
				caosScript *script = new caosScript(world.gametype, "<network>"); // XXX
				script->parse(s);
				script->installScripts();
				caosVM vm(0);
				std::ostringstream o;
				vm.setOutputStream(o);
				vm.runEntirely(script->installer);
				SDLNet_TCP_Send(connection, (void *)o.str().c_str(), o.str().size());
			} catch (std::exception &e) {
				std::string o = std::string("### EXCEPTION: ") + e.what();
				SDLNet_TCP_Send(connection, (void *)o.c_str(), o.size());
			}

			SDLNet_TCP_Close(connection);
		}

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_VIDEORESIZE:
					world.backend.resizeNotify(event.resize.w, event.resize.h);
					for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
						if (!*i) continue;
						(*i)->queueScript(123, 0); // window resized script
					}
					break;
				case SDL_MOUSEMOTION:
					world.hand()->moveTo(event.motion.x + world.camera.getX(), event.motion.y + world.camera.getY());
					for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
						if (!*i) continue;
						if ((*i)->imsk_mouse_move) {
							caosVar x; x.setInt(world.hand()->x);
							caosVar y; y.setInt(world.hand()->y);
							(*i)->queueScript(75, 0, x, y); // Raw Mouse Move
						}
					}
					break;
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:
					for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
						if (!*i) continue;
						if ((event.type == SDL_MOUSEBUTTONUP && (*i)->imsk_mouse_up) ||
							(event.type == SDL_MOUSEBUTTONDOWN && (*i)->imsk_mouse_down)) {
							caosVar button;
							switch (event.button.button) {
								// TODO: the values here make fuzzie suspicious that c2e combines these events
								case SDL_BUTTON_LEFT: button.setInt(1); break;
								case SDL_BUTTON_RIGHT: button.setInt(2); break;
								case SDL_BUTTON_MIDDLE: button.setInt(4); break;
							}

							if (button.hasInt()) {
								if (event.type == SDL_MOUSEBUTTONUP)
									(*i)->queueScript(77, 0, button); // Raw Mouse Up
								else
									(*i)->queueScript(76, 0, button); // Raw Mouse Down
							}
						}
						if ((event.type == SDL_MOUSEBUTTONDOWN &&
							(event.button.button == SDL_BUTTON_WHEELUP || event.button.button == SDL_BUTTON_WHEELDOWN) &&
							(*i)->imsk_mouse_wheel)) {
							caosVar delta;
							if (event.button.button == SDL_BUTTON_WHEELDOWN)
								delta.setInt(-120);
							else
								delta.setInt(120);
							(*i)->queueScript(78, 0, delta); // Raw Mouse Wheel
						}
					}

					if (!world.hand()->handle_events) break;
					if (event.type != SDL_MOUSEBUTTONDOWN) break;

					if (event.button.button == SDL_BUTTON_LEFT) {
						CompoundPart *a = world.partAt(world.hand()->x, world.hand()->y, true);
						if (a /* && a->canActivate() */) { // TODO
							if (!a->getParent()->paused) a->handleClick(world.hand()->x - a->x - a->getParent()->x, world.hand()->y - a->y - a->getParent()->y);
							// TODO: not sure how to handle the following properly, needs research..
							world.hand()->firePointerScript(101, a->getParent()); // Pointer Activate 1
						} else
							world.hand()->queueScript(116, 0); // Pointer Clicked Background
					} else if (event.button.button == SDL_BUTTON_RIGHT) {
						if (world.paused) break; // TODO: wrong?
						
						// picking up and dropping are implictly handled by the scripts (well, messages) 4 and 5	
						// TODO: check if this is correct behaviour, one issue is that this isn't instant, another
						// is the messages might only be fired in c2e when you use MESG WRIT, in which case we'll
						// need to manually set world.hand()->carrying to NULL and a here, respectively - fuzzie
						if (world.hand()->carrying) {
							world.hand()->carrying->queueScript(5, world.hand()); // drop
							world.hand()->firePointerScript(105, world.hand()->carrying); // Pointer Drop
						} else {
							Agent *a = world.agentAt(event.button.x + world.camera.getX(), event.button.y + world.camera.getY(), false, true);
							if (a) {
								a->queueScript(4, world.hand()); // pickup
								world.hand()->firePointerScript(104, a); // Pointer Pickup
							}
						}
					} else if (event.button.button == SDL_BUTTON_MIDDLE) {
						Agent *a = world.agentAt(event.button.x + world.camera.getX(), event.button.y + world.camera.getY(), true);
						if (a)
							std::cout << "Agent under mouse is " << a->identify();
						else
							std::cout << "No agent under cursor";
						std::cout << std::endl;
					}
					break;
				case SDL_KEYDOWN:
					if (event.key.type == SDL_KEYDOWN) {
						int key = world.backend.translateKey(event.key.keysym.sym);
						if (key != -1) {
							if (world.focusagent) {
								TextEntryPart *t = (TextEntryPart *)((CompoundAgent *)world.focusagent.get())->part(world.focuspart);
								if (t)
									t->handleSpecialKey(key);
							}

							caosVar k;
							k.setInt(key);
							for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
								if (!*i) continue;
								if ((*i)->imsk_key_down)
									(*i)->queueScript(73, 0, k); // key down script
							}
						}

						if ((event.key.keysym.unicode) && ((event.key.keysym.unicode & 0xFF80) == 0) && (event.key.keysym.unicode >= 32)) {
							key = event.key.keysym.unicode & 0x7F;
							if (world.focusagent) {
								TextEntryPart *t = (TextEntryPart *)((CompoundAgent *)world.focusagent.get())->part(world.focuspart);
								if (t)
									t->handleKey(key);
							}

							caosVar k;
							k.setInt(key);
							for (std::list<boost::shared_ptr<Agent> >::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
								if (!*i) continue;
								if ((*i)->imsk_translated_char)
									(*i)->queueScript(79, 0, k); // translated char script
							}
						}
					
					/*
					 	MetaRoom *n;
						switch (event.key.keysym.sym) {
							case SDLK_PAGEDOWN:
								if (world.camera.getMetaRoom()->id == 0)
									break;
								n = world.map.getMetaRoom(world.camera.getMetaRoom()->id - 1);
								if (n)
									world.camera.goToMetaRoom(n->id);
								break;
							case SDLK_PAGEUP:
								if ((world.map.getMetaRoomCount() - 1) == world.camera.getMetaRoom()->id)
									break;
								n = world.map.getMetaRoom(world.camera.getMetaRoom()->id + 1);
								if (n)
									world.camera.goToMetaRoom(n->id);
								break;
							case SDLK_r: // insert in Creatures, but my iBook has no insert key - fuzzie
								if (!world.focusagent) { showrooms = !showrooms; break; }
							case SDLK_q:
								if (!world.focusagent) { done = true; break; }
						}
					*/
					}
					break;
				case SDL_QUIT:
					done = true;
					break;
				default:
					break;
			}
		}

		if (ticked) {
		static float accelspeed = 8, decelspeed = .5, maxspeed = 64;
		static float velx = 0;
		static float vely = 0;
		Uint8 *keys = SDL_GetKeyState(NULL);
		if (keys[SDLK_LEFT])
			velx -= accelspeed;
		if (keys[SDLK_RIGHT])
			velx += accelspeed;
		if (!keys[SDLK_LEFT] && !keys[SDLK_RIGHT]) {
			velx *= decelspeed;
			if (velx < 0.1) velx = 0;
		}
		if (keys[SDLK_UP])
			vely -= accelspeed;
		if (keys[SDLK_DOWN])
			vely += accelspeed;
		if (!keys[SDLK_UP] && !keys[SDLK_DOWN]) {
			vely *= decelspeed;
			if (vely < 0.1) vely = 0;
		}

		if (velx >=  maxspeed) velx =  maxspeed;
		if (velx <= -maxspeed) velx = -maxspeed;
		if (vely >=  maxspeed) vely =  maxspeed;
		if (vely <= -maxspeed) vely = -maxspeed;

		if (velx || vely) {
			int adjustx = world.camera.getX(), adjusty = world.camera.getY();
			int adjustbyx = (int)velx, adjustbyy = (int) vely;
			
			if ((adjustx + adjustbyx) < (int)world.camera.getMetaRoom()->x())
				adjustbyx = world.camera.getMetaRoom()->x() - adjustx;
			else if ((adjustx + adjustbyx + world.camera.getWidth()) >
					(world.camera.getMetaRoom()->x() + world.camera.getMetaRoom()->width()))
				adjustbyx = world.camera.getMetaRoom()->x() + 
					world.camera.getMetaRoom()->width() - world.camera.getWidth() - adjustx;
			
			if ((adjusty + adjustbyy) < (int)world.camera.getMetaRoom()->y())
				adjustbyy = world.camera.getMetaRoom()->y() - adjusty;
			else if ((adjusty + adjustbyy + world.camera.getHeight()) > 
					(world.camera.getMetaRoom()->y() + world.camera.getMetaRoom()->height()))
				adjustbyy = world.camera.getMetaRoom()->y() + 
					world.camera.getMetaRoom()->height() - world.camera.getHeight() - adjusty;
			
			world.camera.moveTo(adjustx + adjustbyx, adjusty + adjustbyy, jump);
		}
		} // ticked
	}

	SDLNet_Quit();
	SDL_Quit();

	} catch (std::exception &e) {
		std::cerr << "dying due to exception in main: " << e.what() << "\n";
		return 1;
	}
	return 0;
}

/* vim: set noet: */
