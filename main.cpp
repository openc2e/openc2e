#ifndef _GNU_SOURCE
#define _GNU_SOURCE // for getopt_long
#endif

#include <getopt.h>

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

#include "World.h"
#include "caosVM.h"
#include "PointerAgent.h"
#include "SDLBackend.h"
#include "dialect.h"

#include "SDL_gfxPrimitives.h" // remove once code is moved to SDLBackend
#include <SDL_net.h>

#ifdef _MSC_VER
#define snprintf _snprintf // guh guh guh ><
#undef main // because SDL is stupid
#endif

SDLBackend backend;

SDL_Surface **backsurfs[20]; // todo: grab metaroom count, don't arbitarily define 20
bool showrooms = false, paused = false;

void drawWorld() {
	MetaRoom *m = world.camera.getMetaRoom();
	if (!m) {
		// Whoops - the room we're in vanished, or maybe we were never in one?
		// Try to get a new one ...
		m = world.map.getFallbackMetaroom();
		if (!m) {
			std::cerr << "ERROR: No metarooms! Panicing ..." << std::endl;
			abort();
		}
		world.camera.goToMetaRoom(m->id);
	}
	int adjustx = world.camera.getX();
	int adjusty = world.camera.getY();
	blkImage *test = m->backImage();

	// draw the blk
	for (unsigned int i = 0; i < (test->totalheight / 128); i++) {
		for (unsigned int j = 0; j < (test->totalwidth / 128); j++) {
			// figure out which block number to use
			unsigned int whereweare = j * (test->totalheight / 128) + i;
			
			SDL_Rect destrect;
			destrect.x = (j * 128) - adjustx + m->x();
			destrect.y = (i * 128) - adjusty + m->y();

			// if the block's on screen, blit it.
			if ((destrect.x >= -128) && (destrect.y >= -128) &&
					(destrect.x - 128 <= backend.getWidth()) &&
					(destrect.y - 128 <= backend.getHeight()))
				SDL_BlitSurface(backsurfs[m->id][whereweare], 0, backend.screen, &destrect);
		}
	}

	// render all the agents
	for (std::multiset<Agent *, agentzorder>::iterator i = world.zorder.begin(); i != world.zorder.end(); i++) {
		(*i)->render(&backend, -adjustx, -adjusty);
	}

	if (showrooms) {
		Room *r = world.map.roomAt(world.hand()->x, world.hand()->y);
		for (std::vector<Room *>::iterator i = world.camera.getMetaRoom()->rooms.begin();
				 i != world.camera.getMetaRoom()->rooms.end(); i++) {
			unsigned int col = 0xFFFF00CC;
			if (*i == r) col = 0xFF00FFCC;
			else if (r) {
				if ((**i).doors[r])
					col = 0x00FFFFCC;
			}
			// ceiling
			aalineColor(backend.screen,
					(**i).x_left - adjustx,
					(**i).y_left_ceiling - adjusty,
					(**i).x_right - adjustx,
					(**i).y_right_ceiling - adjusty,
					col);
			// floor
			aalineColor(backend.screen, 
					(**i).x_left - adjustx, 
					(**i).y_left_floor - adjusty,
					(**i).x_right - adjustx,
					(**i).y_right_floor - adjusty,
					col);
			// left side
			aalineColor(backend.screen,
					(**i).x_left - adjustx,
					(**i).y_left_ceiling - adjusty,
					(**i).x_left - adjustx,
					(**i).y_left_floor - adjusty,
					col);
			// right side
			aalineColor(backend.screen,
					(**i).x_right  - adjustx,
					(**i).y_right_ceiling - adjusty,
					(**i).x_right - adjustx,
					(**i).y_right_floor - adjusty,
					col);
		}
	}
	//SDL_UpdateRect(backend.screen, 0, 0, 0, 0);
	SDL_Flip(backend.screen);
}

namespace fs = boost::filesystem;
extern fs::path homeDirectory(); // creaturesImage.cpp

extern "C" {
	extern char *optarg;
	extern int optind, opterr, optopt;
}

const static struct option longopts[] = {
	{	"silent",
		no_argument,
		NULL,
		's',
	},
	{	"help",
		no_argument,
		NULL,
		'h'
	},
	{	"version",
		no_argument,
		NULL,
		'v'
	},
	{
		"gametype",
		required_argument,
		NULL,
		'g'
	},
	{	"data",
		required_argument,
		NULL,
		'd'
	},
	{	"bootstrap",
		required_argument,
		NULL,
		'b'
	},
	{ NULL, 0, NULL, 0 }
};

const static char optstring[] = "hvsd:b:";

static const char data_default[] = "./data";
static const char bootstrap_suffix[] = "/Bootstrap/001 World";
static const char bootstrapDS_suffix[] = "/Bootstrap/010 Docking Station";

static void opt_help(const char *exename) {
	if (!exename) // argc == 0? o_O
		exename = "openc2e";
	std::cout << "Usage: " << exename << " [options]" << std::endl;
	std::cout << std::endl;
	printf("  %-20s %s\n", "-h, -?, --help", "shows this help");
	printf("  %-20s %s\n", "-v, --version", "shows program version");
	printf("  %-20s %s\n", "-s, --silent", "disables game sounds");
	printf("  %-20s %s\n", "-d, --data", "sets base path for game data");
	printf("  %-20s %s\n", "-b, --bootstrap", "sets bootstrap directory");
	printf("  %-20s %s\n", "-g, --gametype", "sets the game type (cv or c3, c3 by default)");
	std::cout << std::endl <<
		"If --data is not specified, it defaults to \"" << data_default << "\". If --bootstrap is"<< std::endl <<
		"not specified, it defaults to \"<data directory>" << bootstrap_suffix << "\"."	<< std::endl;

}

static void opt_version() {
	// We already showed the primary version bit, just throw in some random
	// legalese
	std::cout << 
		"This is free software; see the source for copying conditions.  There is NO" << std::endl <<
		"warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." << std::endl << std::endl <<
		"...please don't sue us." << std::endl;
}

std::string datapath;
extern "C" int main(int argc, char *argv[]) {
	try {
		
	std::cout << "openc2e, built " __DATE__ " " __TIME__ "\nCopyright (c) 2004-2005 Alyssa Milburn\n\n";
	int optret;
	bool enable_sound = true;
	std::string bootstrap;
	std::string data;
	bool bs_specd = false, d_specd = false;
	std::string gametype = "c3";
	while (-1 != (optret = getopt_long(argc, argv, optstring, longopts, NULL))) {
		switch (optret) {
			case 'h': //fallthru
			case '?': // -?, or any unrecognized option
				opt_help(argv[0]);
				return 0;
			case 'v':
				opt_version();
				return 0;
			case 's':
				enable_sound = false;
				break;
			case 'd':
				if (d_specd) {
					std::cerr << "Error: --data specified twice." << std::endl;
					opt_help(argv[0]);
					return 1;
				}
				d_specd = true;
				data = optarg;
				break;
			case 'b':
				if (bs_specd) {
					std::cerr << "Error: --bootstrap specified twice." << std::endl;
					opt_help(argv[0]);
					return 1;
				}
				bs_specd = true;
				bootstrap = optarg;
				break;
			case 'g':
				gametype = optarg;
				if ((gametype != "c3") && (gametype != "cv")) {
					std::cerr << "Error: unrecognized game type." << std::endl;
					opt_help(argv[0]);
					return 1;
				}
				break;
		}
	}
	
	if (!d_specd) {
		data = data_default;
	}

	if (!bs_specd) {
		bootstrap = data + bootstrap_suffix;
		fs::path scriptdir(bootstrap, fs::native);
		if (!fs::exists(scriptdir)) {
			caosVar name; name.setString("engine_no_auxiliary_bootstrap_1");
			caosVar contents; contents.setInt(1);
			world.eame_variables[name] = contents;
			bootstrap = data + bootstrapDS_suffix;
		}
	}

	if (optind < argc) {
		// too many args
		opt_help(argv[0]);
		return 1;
	}

	datapath = data;
	
	fs::path datadir(datapath, fs::native);
	if (!fs::exists(datadir)) {
		std::cerr << "data path '" << datapath << "' doesn't exist, try --help" << std::endl;
		return 1;
	}
	
	registerDelegates();
	world.init();
	world.catalogue.initFrom(fs::path(datapath + "/Catalogue/", fs::native));
	// moved backend.init() here because we need the camera to be valid - fuzzie
	backend.init(enable_sound);
	world.camera.setBackend(&backend);

	std::vector<std::string> scripts;
	fs::path scriptdir(bootstrap, fs::native);

	bool singlescript = false;
	
	if (fs::exists(scriptdir)) {
		if (fs::is_directory(scriptdir)) {
			fs::directory_iterator fsend;
			for (fs::directory_iterator i(scriptdir); i != fsend; ++i) {
				try {
					if ((!fs::is_directory(*i)) && (fs::extension(*i) == ".cos"))
						scripts.push_back(i->native_file_string());
				} catch (fs::filesystem_error &ex) {
					std::cerr << "directory_iterator died on '" << i->leaf() << "' with " << ex.what() << std::endl;
				}
			}
		} else {
			scripts.push_back(scriptdir.native_file_string());
			singlescript = true;
		}
	} else {
		if (argc > 1) {
			std::cerr << "couldn't find script directory (trying " << scriptdir.native_file_string() << ")!\n";
		} else {
			std::cerr << "couldn't find bootstrap directory!\n";
		}
		return 1;
	}

	std::sort(scripts.begin(), scripts.end());
	for (std::vector<std::string>::iterator i = scripts.begin(); i != scripts.end(); i++) {
		std::ifstream s(i->c_str());
		assert(s.is_open());
		std::cout << "executing script " << *i << "...\n";
		std::cout.flush();
		std::cerr.flush();
		try {
			caosScript *script = new caosScript(gametype, *i); // XXX
			script->parse(s);
			caosVM vm(0);
			script->installScripts();
//			std::cout << script->installer->dump();
			vm.runEntirely(script->installer);

		} catch (std::exception &e) {
			std::cerr << "script exec failed due to exception " << e.what();
			std::cerr << std::endl;
		}
		std::cout.flush();
		std::cerr.flush();
//        Collectable::doCollect();
	}

	if (world.map.getMetaRoomCount() == 0) {
		if (!singlescript)
			std::cerr << "\nNo metarooms found in given directory (" << scriptdir.native_directory_string() << "), exiting.\n";
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
		std::ofstream f((p.native_directory_string() + "/port").c_str(), ios::trunc);
		char buf[6];
		snprintf(buf, 6, "%i", listenport);
		f << buf;
	}

	AgentRef handAgent;

	for (unsigned int j = 0; j < world.map.getMetaRoomCount(); j++) {
		MetaRoom *m = world.map.getArrayMetaRoom(j);
		blkImage *test = m->backImage();
		assert(test != 0);

		backsurfs[m->id] = new SDL_Surface *[test->numframes()];
		for (unsigned int i = 0; i < test->numframes(); i++) {
			backsurfs[m->id][i] = SDL_CreateRGBSurfaceFrom(test->data(i),
														   test->width(i),
														   test->height(i),
														   16, // depth
														   test->width(i) * 2, // pitch
														   0xF800, 0x07E0, 0x001F, 0); // RGBA mask
			assert(backsurfs[m->id][i] != 0);
		}
	}

	drawWorld();

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
		if (!paused && (backend.ticks() > (tickdata + world.ticktime))) {
			tickdata = backend.ticks();
			
			world.tick();
			if (handAgent) // TODO: do this in world.tick()
				handAgent->moveTo(world.hand()->x + 2, world.hand()->y + 2);
			drawWorld();
			
			ticktime[ticktimeptr] = backend.ticks() - tickdata;
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
				caosScript *script = new caosScript(gametype, "<network>"); // XXX
				script->parse(s);
				script->installScripts();
				caosVM vm(0);
				std::ostringstream o;
				vm.setOutputStream(o);
				vm.runEntirely(script->installer);
				SDLNet_TCP_Send(connection, (void *)o.str().c_str(), o.str().size());
			} catch (creaturesException &e) {
				std::string o = std::string("### EXCEPTION: ") + e.what();
				SDLNet_TCP_Send(connection, (void *)o.c_str(), o.size());
			}

			SDLNet_TCP_Close(connection);
		}

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_VIDEORESIZE:
					backend.resizeNotify(event.resize.w, event.resize.h);
					for (std::list<Agent *>::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
						(*i)->fireScript(123, 0); // window resized script
					}
					break;
				case SDL_MOUSEMOTION:
					world.hand()->moveTo(event.motion.x + world.camera.getX(), event.motion.y + world.camera.getY());
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT) {
						Agent *a = world.agentAt(world.hand()->x, world.hand()->y, false);
						if (a) {
							a->handleClick(world.hand()->x - a->x, world.hand()->y - a->y);
							// TODO: not sure how to handle the following properly, needs research..
							world.hand()->firePointerScript(101, a); // Pointer Activate 1
						} else
							world.hand()->fireScript(116, 0); // Pointer Clicked Background
					} else if (event.button.button == SDL_BUTTON_RIGHT) {
						// for now, hack!
						if (handAgent) {
							handAgent->fireScript(5, world.hand()); // drop
							world.hand()->firePointerScript(105, handAgent); // Pointer Drop
							handAgent = 0;
						} else {
							handAgent = world.agentAt(event.button.x + world.camera.getX(), event.button.y + world.camera.getY(), false);
							if (handAgent) {
								if (handAgent->mouseable) { // hack: agentAt should check this
									handAgent->fireScript(4, world.hand()); // pickup
									world.hand()->firePointerScript(104, handAgent); // Pointer Pickup
								} else
									handAgent = 0;
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
							case SDLK_BACKSPACE:
								if (world.focusagent) {
									TextEntryPart *t = (TextEntryPart *)((CompoundAgent *)world.focusagent.get())->part(world.focuspart);
									t->handleKey(0);
								}
								break;
							case SDLK_RETURN:
								if (world.focusagent) {
									TextEntryPart *t = (TextEntryPart *)((CompoundAgent *)world.focusagent.get())->part(world.focuspart);
									t->handleKey(1);
								}
								break;
							case SDLK_r: // insert in Creatures, but my iBook has no insert key - fuzzie
								if (!world.focusagent) { showrooms = !showrooms; break; }
							case SDLK_q:
								if (!world.focusagent) { done = true; break; }
							default:
								if (event.key.keysym.unicode) {
									if ((event.key.keysym.unicode & 0xFF80) == 0) {
										char ch = event.key.keysym.unicode & 0x7F;
										if (world.focusagent) {
											// TODO: why isn't focuspart a TextEntryPart*?
											TextEntryPart *t = (TextEntryPart *)((CompoundAgent *)world.focusagent.get())->part(world.focuspart);
											t->handleKey(ch);
										}
									}
								}
								break;
						}

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
			else if ((adjustx + adjustbyx + backend.getWidth()) >
					(world.camera.getMetaRoom()->x() + world.camera.getMetaRoom()->width()))
				adjustbyx = world.camera.getMetaRoom()->x() + 
					world.camera.getMetaRoom()->width() - backend.getWidth() - adjustx;
			
			if ((adjusty + adjustbyy) < (int)world.camera.getMetaRoom()->y())
				adjustbyy = world.camera.getMetaRoom()->y() - adjusty;
			else if ((adjusty + adjustbyy + backend.getHeight()) > 
					(world.camera.getMetaRoom()->y() + world.camera.getMetaRoom()->height()))
				adjustbyy = world.camera.getMetaRoom()->y() + 
					world.camera.getMetaRoom()->height() - backend.getHeight() - adjusty;
			
			world.hand()->moveTo(world.hand()->x + adjustbyx, world.hand()->y + adjustbyy);
			world.camera.moveTo(adjustx + adjustbyx, adjusty + adjustbyy, jump);
		}
		} // ticked
	}

	SDLNet_Quit();
	SDL_Quit();

	} catch (creaturesException &e) {
		std::cerr << "dying due to exception in main: " << e.what() << "\n";
		return 1;
	}
	return 0;
}

/* vim: set noet: */
