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

void SDLBackend::resizeNotify(int width, int height) {
	screen = SDL_SetVideoMode(width, height, 0, SDL_SWSURFACE + SDL_RESIZABLE);
	assert(screen != 0);
}

void SDLBackend::init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
		assert(false);
	}
	
	resizeNotify(640, 480);
	
	SDL_WM_SetCaption("openc2e - Creatures 3", "openc2e");
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_ShowCursor(false);
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
