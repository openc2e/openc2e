#include <sstream> // for istringstream, used in networking code
#include <fstream>
#include "openc2e.h"
#include <iostream>
#include <algorithm>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/exception.hpp"
#include "boost/filesystem/convenience.hpp"

#include "World.h"
#include "caosVM.h"
#include "SimpleAgent.h"
#include "SDLBackend.h"

#include "SDL_gfxPrimitives.h" // remove once code is moved to SDLBackend
#include "SDL_net.h"

SDLBackend backend;

SDL_Surface **backsurfs[20]; // todo: grab metaroom count, don't arbitarily define 20
bool showrooms = false, paused = false;

void drawWorld() {
	int adjustx = world.camera.getX();
	int adjusty = world.camera.getY();
	MetaRoom *m = world.camera.getMetaRoom();
	blkImage *test = m->backImage();
	for (unsigned int i = 0; i < (test->totalheight / 128); i++) {
		for (unsigned int j = 0; j < (test->totalwidth / 128); j++) {
			unsigned int whereweare = j * (test->totalheight / 128) + i;
			SDL_Rect destrect;
			destrect.x = (j * 128) - adjustx + m->x(); destrect.y = (i * 128) - adjusty + m->y();
			if ((destrect.x >= -128) && (destrect.y >= -128) && (destrect.x - 128 <= backend.getWidth()) && (destrect.y - 128 <= backend.getHeight()))
				SDL_BlitSurface(backsurfs[m->id][whereweare], 0, backend.screen, &destrect);
		}
	}
	for (std::multiset<Agent *, agentzorder>::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
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
	SDL_UpdateRect(backend.screen, 0, 0, 0, 0);
}

namespace fs = boost::filesystem;

extern "C" int main(int argc, char *argv[]) {
	try {
		
	std::cout << "openc2e, built " __DATE__ " " __TIME__ "\nCopyright (c) 2004 Alyssa Milburn\n\n";

	setupCommandPointers();
	world.init();
	world.catalogue.initFrom("data/Catalogue/");
	// moved backend.init() here because we need the camera to be valid - fuzzie
	backend.init();
	world.camera.setBackend(&backend);

	std::vector<std::string> scripts;
	fs::path scriptdir((argc > 1 ? argv[1] : "data/Bootstrap/001 World/"), fs::native);

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
		}
	} else {
		if (argc > 1) {
			std::cerr << "couldn't find script directory (trying " << argv[1] << ")!\n";
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
		caosScript script(s);
		caosVM vm(0);
		vm.runEntirely(script.installer);
		std::cout.flush();
		std::cerr.flush();
	}

	if (world.map.getMetaRoomCount() == 0) {
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

	for (unsigned int j = 0; j < world.map.getMetaRoomCount(); j++) {
		MetaRoom *m = world.map.getMetaRoom(j);
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
	
	world.camera.goToMetaRoom(0);
	drawWorld();

	bool done = false;
	unsigned int tickdata = 0;
	unsigned int ticktime[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned int ticktimeptr = 0;
	while (!done) {
		/*
		 we calculate PACE below, but it's inaccurate because drawWorld(), our biggest cpu consumer, isn't in the loop
		 this is because it makes the game seem terribly unresponsive..
		*/
		if (!paused && (backend.ticks() > (tickdata + world.ticktime))) {
			ticktime[ticktimeptr] = backend.ticks();
			
			world.tick();
			
			ticktime[ticktimeptr] = backend.ticks() - ticktime[ticktimeptr];
			ticktimeptr++;
			if (ticktimeptr == 10) ticktimeptr = 0;
			float avgtime = 0;
			for (unsigned int i = 0; i < 10; i++) avgtime += ((float)ticktime[i] / world.ticktime);
			world.pace = avgtime / 10;
			
			tickdata = backend.ticks();
		}
			
		drawWorld();
		
		while (TCPsocket connection = SDLNet_TCP_Accept(listensocket)) {
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
			caosScript script(s);
			caosVM vm(0);
			vm.runEntirely(script.installer);

			SDLNet_TCP_Close(connection);
		}

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_VIDEORESIZE:
					backend.resizeNotify(event.resize.w, event.resize.h);
					break;
				case SDL_MOUSEMOTION:
					world.hand()->moveTo(event.motion.x + world.camera.getX(), event.motion.y + world.camera.getY());
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT) {
						std::cout << "got mouse click" << std::endl;
						// TODO: just take the world.hand() x/y here?
						Agent *a = world.agentAt(event.button.x + world.camera.getX(), event.button.y + world.camera.getY(), true);
						if (a) {
							if (a->clik != -1) {
								// TODO: handle CLIK
							} else if (a->clac[0] != -1)
								a->fireScript(a->clac[0]);
						}
						else std::cout << "(mouse click ignored)" << std::endl;
					}
					break;
				case SDL_KEYDOWN:
					if (event.key.type == SDL_KEYDOWN) {
						switch (event.key.keysym.sym) {
						/*	case SDLK_LEFT:
								adjustbyx = -20;
								break;
							case SDLK_RIGHT:
								adjustbyx = 20;
								break;
							case SDLK_UP:
								adjustbyy = -20;
								break;
							case SDLK_DOWN:
								adjustbyy = 20;
								break; */
							case SDLK_r: // insert in Creatures, but my iBook has no insert key - fuzzie
								showrooms = !showrooms; break;
							case SDLK_q:
								done = true; break;
							case SDLK_PAGEDOWN:
								if (world.camera.getMetaRoom()->id == 0)
									break;
								world.camera.goToMetaRoom(world.camera.getMetaRoom()->id - 1);
								break;
							case SDLK_PAGEUP:
								if ((world.map.getMetaRoomCount() - 1) == world.camera.getMetaRoom()->id)
									break;
								world.camera.goToMetaRoom(world.camera.getMetaRoom()->id + 1);
								break;
							default:
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
		}
		if (keys[SDLK_UP])
			vely -= accelspeed;
		if (keys[SDLK_DOWN])
			vely += accelspeed;
		if (!keys[SDLK_UP] && !keys[SDLK_DOWN]) {
			vely *= decelspeed;
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
	}

	SDLNet_Quit();
	SDL_Quit();

	} catch (creaturesException &e) {
		std::cerr << "dying due to exception in main: " << e.what() << "\n";
		return 1;
	}
	return 0;
}

