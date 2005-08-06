/*
 *  SDLBackend.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Oct 24 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#include "SDLBackend.h"
#include "SDL_gfxPrimitives.h"
#include "openc2e.h"
#include <strings.h> // bzero

SDLBackend *g_backend;

void SDLBackend::resizeNotify(int _w, int _h) {
	width = _w;
	height = _h;
	screen = SDL_SetVideoMode(width, height, 0, SDL_SWSURFACE + SDL_RESIZABLE);
	assert(screen != 0);
}

void SDLBackend::init() {
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
		assert(false);
	}

	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096) < 0) {
		std::cerr << "SDL_mixer init failed, disabling sound: " << Mix_GetError() << std::endl;
		soundenabled = false;
	} else soundenabled = true;

	for (unsigned int i = 0; i++; i < nosounds) {
		sounds[i] = 0;
	}
	
	resizeNotify(640, 480);
	
	SDL_WM_SetCaption("openc2e - Creatures 3 (development build)", "openc2e");
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_ShowCursor(false);
}

void SDLBackend::playFile(std::string filename) {
	if (!soundenabled) return;

	unsigned int i = 0;

	while (i < nosounds) {
		if (sounds[i] == 0) break;
		if (!Mix_Playing(soundchannels[i])) {
			Mix_FreeChunk(sounds[i]);
			sounds[i] = 0;
			break;
		}
		i++;
	}
	
	if (i == nosounds) return; // no free slots, so return
	
	std::string fname = "data/Sounds/" + filename + ".wav"; // TODO: case sensitivity stuff
	//std::cout << "trying to play " << fname << std::endl;
	sounds[i] = Mix_LoadWAV(fname.c_str());
	if (!sounds[i]) return;

	soundchannels[i] = Mix_PlayChannel(-1, sounds[i], 0);
	// Mix_SetPosition(soundschannels[i], angle in degrees, distance from 0 (near) to 255);
}

void SDLBackend::renderLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int colour) {
	aalineColor(screen, x1, y1, x2, y2, colour);
}

void SDLBackend::render(creaturesImage *image, unsigned int frame, unsigned int x, unsigned int y) {
	SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(image->data(frame),
												 image->width(frame),
												 image->height(frame),
												 16, // depth
												 image->width(frame) * 2, // pitch
												 0xF800, 0x07E0, 0x001F, 0); // RGBA mask
	SDL_SetColorKey(surf, SDL_SRCCOLORKEY, 0);
	SDL_Rect destrect;
	destrect.x = x; destrect.y = y;
	SDL_BlitSurface(surf, 0, screen, &destrect);
	SDL_FreeSurface(surf);
}

