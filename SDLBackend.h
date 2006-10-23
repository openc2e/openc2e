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
#include <SDL_mixer.h>
#include "creaturesImage.h"
#include "endianlove.h"
#include <map>

struct SoundSlot {
	int soundchannel;
	Mix_Chunk *sound;
	class Agent *agent;

	SoundSlot() { sound = 0; }

	void play();
	void playLooped();
	void adjustPanning(int angle, int distance);
	void fadeOut();
	void stop();
};

class SDLSurface {
	friend class SDLBackend;

protected:
	SDL_Surface *surface;
	int width, height;

public:
	void render(creaturesImage *image, unsigned int frame, int x, int y, bool trans = false, unsigned char transparency = 0, bool mirror = false, bool is_background = false);
	void renderLine(int x1, int y1, int x2, int y2, unsigned int colour);
	void blitSurface(SDLSurface *src, int x, int y, int w, int h);
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	void renderDone();
};

class SDLBackend {
protected:
	bool soundenabled;
	static const unsigned int nosounds = 12;
	SoundSlot sounddata[12];

	std::map<std::string, Mix_Chunk *> soundcache;

	SDLSurface mainsurface;

public:
	SDLBackend() { }
	unsigned int ticks() { return SDL_GetTicks(); }
	void init(bool enable_sound);
	void resizeNotify(int _w, int _h);
	SoundSlot *getAudioSlot(std::string filename);
	SDLSurface &getMainSurface() { return mainsurface; }
	SDLSurface *newSurface(unsigned int width, unsigned int height);
	void freeSurface(SDLSurface *surf);
	bool keyDown(int key);
	int translateKey(int key);
};

#endif
