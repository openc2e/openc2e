#include <fstream>
#include "openc2e.h"
#include <iostream>
#include <algorithm>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/exception.hpp"
#include "boost/filesystem/convenience.hpp"

#include "World.h"
#include "SimpleAgent.h"
#include "SDLBackend.h"

#include "SDL_gfxPrimitives.h" // remove once code is moved to SDLBackend

SDLBackend backend;

SDL_Surface **backsurfs[20]; // todo: grab metaroom count, don't arbitarily define 20
int adjustx, adjusty;
bool showrooms = false, paused = false;

void drawWorld() {
	MetaRoom *m = world.map.getCurrentMetaRoom();
	blkImage *test = m->backImage();
	for (unsigned int i = 0; i < (test->totalheight / 128); i++) {
		for (unsigned int j = 0; j < (test->totalwidth / 128); j++) {
			unsigned int whereweare = j * (test->totalheight / 128) + i;
			SDL_Rect destrect;
			destrect.x = (j * 128) - adjustx + m->x(); destrect.y = (i * 128) - adjusty + m->y();
			SDL_BlitSurface(backsurfs[m->id][whereweare], 0, backend.screen, &destrect);
		}
	}
	if (showrooms) {
		for (std::vector<Room *>::iterator i = world.map.getCurrentMetaRoom()->rooms.begin();
				 i != world.map.getCurrentMetaRoom()->rooms.end(); i++) {
			// ceiling
			aalineColor(backend.screen, (**i).x_left - adjustx, (**i).y_left_ceiling - adjusty, (**i).x_right - adjustx, (**i).y_right_ceiling - adjusty, 0xFF000077);
			// floor
			aalineColor(backend.screen, (**i).x_left - adjustx, (**i).y_left_floor - adjusty, (**i).x_right - adjustx, (**i).y_right_floor - adjusty, 0xFF000077);
			// left side
			aalineColor(backend.screen, (**i).x_left - adjustx, (**i).y_left_ceiling - adjusty, (**i).x_left - adjustx, (**i).y_left_floor - adjusty, 0xFF000077);
			// right side
			aalineColor(backend.screen, (**i).x_right  - adjustx, (**i).y_right_ceiling - adjusty, (**i).x_right - adjustx, (**i).y_right_floor - adjusty, 0xFF000077);
		}
	}
	for (std::multiset<Agent *, agentzorder>::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		(*i)->render(&backend, -adjustx, -adjusty);
	}
	SDL_UpdateRect(backend.screen, 0, 0, 0, 0);
}

namespace fs = boost::filesystem;

extern "C" int main(int argc, char *argv[]) {
	std::cout << "openc2e, built " __DATE__ " " __TIME__ "\nCopyright (c) 2004 Alyssa Milburn\n\n";

	setupCommandPointers();
	world.init();
	world.catalogue.initFrom("data/Catalogue/");

	std::vector<std::string> scripts;
	fs::path scriptdir((argc > 1 ? argv[1] : "data/Bootstrap/001 World/"), fs::native);

	assert(fs::exists(scriptdir));
	assert(fs::is_directory(scriptdir));
	fs::directory_iterator fsend;
	for (fs::directory_iterator i(scriptdir); i != fsend; ++i) {
		try {
			if ((!fs::is_directory(*i)) && (fs::extension(*i) == ".cos"))
				scripts.push_back(i->native_file_string());
		} catch (fs::filesystem_error &ex) {
			std::cerr << "directory_iterator died on '" << i->leaf() << "' with " << ex.what() << std::endl;
		}
	}

	std::sort(scripts.begin(), scripts.end());
	for (std::vector<std::string>::iterator i = scripts.begin(); i != scripts.end(); i++) {
		std::ifstream script(i->c_str());
		assert(script.is_open());
		std::cout << "loading script " << *i << "...\n";
		std::cout.flush();
		std::cerr.flush();
		caosScript testscript(script);
		caosVM testvm(0);
		/* std::cout << "dump of script:\n";
		std::cout << testscript.dump(); */
		std::cout.flush();
		std::cerr.flush();
		std::cout << "executing script " << *i << "...\n";
		std::cout.flush();
		std::cerr.flush();
		testvm.runEntirely(testscript.installer);
		std::cout.flush();
		std::cerr.flush();
	}

	if (world.map.getMetaRoomCount() == 0) {
		std::cerr << "\nNo metarooms found in given directory (" << scriptdir.native_directory_string() << "), exiting.\n";
		return 0;
	}

	backend.init();

	for (unsigned int j = 0; j < world.map.getMetaRoomCount(); j++) {
		world.map.SetCurrentMetaRoom(j);
		MetaRoom *m = world.map.getCurrentMetaRoom();
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
	
	world.map.SetCurrentMetaRoom(0);
	
	adjustx = world.map.getCurrentMetaRoom()->x();
	adjusty = world.map.getCurrentMetaRoom()->y();
	drawWorld();

	bool done = false;
	unsigned int tickdata = 0;
	while (!done) {
		if (!paused && (backend.ticks() > tickdata + 50)) {
			world.tick(); // TODO: use BUZZ value
			tickdata = backend.ticks();
		}
		drawWorld();

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_VIDEORESIZE:
					backend.resizeNotify(event.resize.w, event.resize.h);
					break;
				case SDL_MOUSEMOTION:
					world.hand()->moveTo(event.motion.x + adjustx, event.motion.y + adjusty);
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (event.button.button == SDL_BUTTON_LEFT) {
						std::cout << "got mouse click" << std::endl;
						Agent *a = world.agentAt(adjustx + event.button.x, adjusty + event.button.y);
						if (a) a->fireScript(1);
						else std::cout << "(mouse click ignored)" << std::endl;
					}
					break;
				case SDL_KEYDOWN:
					if (event.key.type == SDL_KEYDOWN) {
						switch (event.key.keysym.sym) {
							case SDLK_LEFT:
								adjustx -= 20; break;
							case SDLK_RIGHT:
								adjustx += 20; break;
							case SDLK_UP:
								adjusty -= 20; break;
							case SDLK_DOWN:
								adjusty += 20; break;
							case SDLK_r: // insert in Creatures, but my iBook has no insert key - fuzzie
								showrooms = !showrooms; break;
							case SDLK_q:
								done = true; break;
							case SDLK_PAGEDOWN:
								if (world.map.getCurrentMetaRoom()->id == 0)
									break;
								world.map.SetCurrentMetaRoom(world.map.getCurrentMetaRoom()->id - 1);
								adjustx = world.map.getCurrentMetaRoom()->x();
								adjusty = world.map.getCurrentMetaRoom()->y();
								break;
							case SDLK_PAGEUP:
								if ((world.map.getMetaRoomCount() - 1) == world.map.getCurrentMetaRoom()->id)
									break;
								world.map.SetCurrentMetaRoom(world.map.getCurrentMetaRoom()->id + 1);
								adjustx = world.map.getCurrentMetaRoom()->x();
								adjusty = world.map.getCurrentMetaRoom()->y();
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
	}
	
	SDL_Quit();

	return(0);
}
