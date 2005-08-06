/*
 *  SDLBackend.h
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

#ifndef _SDLBACKEND_H
#define _SDLBACKEND_H

#include <SDL/SDL.h>
#include "SDL_mixer.h"
#include "creaturesImage.h"
#include "endianlove.h"
#include <map>

extern class SDLBackend *g_backend; // global hack, for now

class SDLBackend {
protected:
	bool soundenabled;
	static const unsigned int nosounds = 12;
	Mix_Chunk *sounds[12];
	int soundchannels[12];

	std::map<std::string, Mix_Chunk *> soundcache;
	
	int width, height;

public:
	SDL_Surface *screen;
	
public:
	SDLBackend() { g_backend = this; }
	unsigned int ticks() { return SDL_GetTicks(); }
	void init();
	void resizeNotify(int _w, int _h);
	void playFile(std::string filename);
	void render(creaturesImage *image, unsigned int frame, unsigned int x, unsigned int y);
	void renderLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int colour);
	int getWidth() const { return width; }
	int getHeight() const { return height; }
};

#endif
