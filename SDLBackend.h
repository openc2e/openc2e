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

class SDLBackend {
protected:
	bool soundenabled;
	static const unsigned int nosounds = 12;
	SoundSlot sounddata[12];

	std::map<std::string, Mix_Chunk *> soundcache;
	
	int width, height;

public:
	SDL_Surface *screen;
	
public:
	SDLBackend() { }
	unsigned int ticks() { return SDL_GetTicks(); }
	void init(bool enable_sound);
	void resizeNotify(int _w, int _h);
	SoundSlot *getAudioSlot(std::string filename);
	void render(creaturesImage *image, unsigned int frame, unsigned int x, unsigned int y, bool trans, unsigned char transparency);
	void renderLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int colour);
	int getWidth() const { return width; }
	int getHeight() const { return height; }
	bool keyDown(int key);
	int translateKey(int key);
};

#endif
