#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

#include <fstream>
#include <assert.h>
#include <iostream>

#include "World.h"
#include "caosVM.h"

// testy!

extern "C" int main(int argc, char *argv[]) {
/*	if (argc != 2) {
		std::cout << "syntax: openc2e filename";
		return 1;
	} */
	std::ifstream sc("/home/fuzzie/openc2e/data/Bootstrap/001 World/!map.cos");
	caosScript testscript(sc);
	caosVM testvm(0);
	
/*	std::cout << "dump of script:\n";
	std::cout << testscript.dump(); */
	std::cout << "executing script...\n";
	testvm.script = &testscript;
	testvm.runEntirely();
	
	Uint32 initflags = SDL_INIT_VIDEO;
	SDL_Surface *screen;
	Uint8 video_bpp = 0;
	Uint32 videoflags = SDL_SWSURFACE + SDL_RESIZABLE;
	int done;
	SDL_Event event;

//	MetaRoom m(0, 0, 4112, 1300, "norn3.0");

	if ( SDL_Init(initflags) < 0 ) {
		std::cout << "SDL init failed: " << SDL_GetError();
		return 1;
	}

	blkImage *test = world.map.getMetaRoom(0)->backImage();
	assert(test != 0);

	SDL_Surface **backsurfs;
	backsurfs = new SDL_Surface *[test->numframes()];
	for (int i = 0; i < test->numframes(); i++) {
		backsurfs[i] = SDL_CreateRGBSurfaceFrom(test->data(i),
																						test->width(i),
																						test->height(i),
																						16, // depth
																						test->width(i) * 2, // pitch
																						0xF800, 0x07E0, 0x001F, 0); // RGBA mask
		assert(backsurfs[i] != 0);
	}
	
	screen = SDL_SetVideoMode(640, 480, video_bpp, videoflags);
	assert(screen != 0);

	done = 0;
	while ( !done ) {
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
				case SDL_VIDEORESIZE:
					screen = SDL_SetVideoMode(event.resize.w, event.resize.h, video_bpp, videoflags);
					assert(screen != 0);
				case SDL_MOUSEBUTTONDOWN:
					for (int i = 0; i < (test->totalheight / 128); i++) {
						for (int j = 0; j < (test->totalwidth / 128); j++) {
							int whereweare = j * (test->totalheight / 128) + i;
							SDL_Rect destrect;
							destrect.x = (j * 128); destrect.y = (i * 128);
							SDL_BlitSurface(backsurfs[whereweare], 0, screen, &destrect);
						}
					}
					for (std::vector<Room>::iterator i = world.map.getMetaRoom(0)->rooms.begin();
							 i != world.map.getMetaRoom(0)->rooms.end(); i++) {
						// ceiling
						lineColor(screen, i->x_left, i->y_left_ceiling, i->x_right, i->y_right_ceiling, 0xFF000000);
						// floor
						lineColor(screen, i->x_left, i->y_left_floor, i->x_right, i->y_right_floor, 0xFF000000);
						// left side
						lineColor(screen, i->x_left, i->y_left_ceiling, i->x_left, i->y_left_floor, 0xFF000000);
						// right side
						lineColor(screen, i->x_right, i->y_right_ceiling, i->x_right, i->y_right_floor, 0xFF000000);
					}
					SDL_UpdateRect(screen, 0, 0, 0, 0);
					std::cout << "blit ok\n";
					break;
				case SDL_KEYDOWN:
					break;
				case SDL_QUIT:
					done = 1;
					break;
				default:
					break;
			}
		}
	}
	
	SDL_Quit();

	return(0);
}
