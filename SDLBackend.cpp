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
#include "Agent.h"
#include <strings.h> // bzero

SDLBackend *g_backend;

void SoundSlot::play() {
	soundchannel = Mix_PlayChannel(-1, sound, 0);
}

void SoundSlot::playLooped() {
	soundchannel = Mix_PlayChannel(-1, sound, -1);
}

void SoundSlot::adjustPanning(int angle, int distance) {
	Mix_SetPosition(soundchannel, angle, distance);
}

void SoundSlot::fadeOut() {
	Mix_FadeOutChannel(soundchannel, 500); // TODO: is 500 a good value?
}

void SoundSlot::stop() {
	Mix_HaltChannel(soundchannel);
	sound = 0;
}

void SDLBackend::resizeNotify(int _w, int _h) {
	width = _w;
	height = _h;
	screen = SDL_SetVideoMode(width, height, 0, SDL_SWSURFACE + SDL_RESIZABLE);
	assert(screen != 0);
}

void SDLBackend::init(bool enable_sound) {
	if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) < 0) {
		std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
		assert(false);
	}

	if (!enable_sound) {
		std::cerr << "Sound disabled per user request." << std::endl;
		soundenabled = false;
	} else if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096) < 0) {
		std::cerr << "SDL_mixer init failed, disabling sound: " << Mix_GetError() << std::endl;
		soundenabled = false;
	} else soundenabled = true;

	resizeNotify(640, 480);
	
	SDL_WM_SetCaption("openc2e - Creatures 3 (development build)", "openc2e");
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_ShowCursor(false);
}

SoundSlot *SDLBackend::getAudioSlot(std::string filename) {
	if (!soundenabled) return 0;

	unsigned int i = 0;

	while (i < nosounds) {
		if (sounddata[i].sound == 0) break;
		if (!Mix_Playing(sounddata[i].soundchannel)) {
			sounddata[i].sound = 0;
			if (sounddata[i].agent)
				sounddata[i].agent->soundslot = 0;
			break;
		}
		i++;
	}
	
	if (i == nosounds) return 0; // no free slots, so return

	sounddata[i].agent = 0;

	std::map<std::string, Mix_Chunk *>::iterator it = soundcache.find(filename);
	if (it != soundcache.end()) {
		sounddata[i].sound = (*it).second;
	} else {
		std::string fname = datapath + "/Sounds/" + filename + ".wav"; // TODO: case sensitivity stuff
		//std::cout << "trying to load " << fname << std::endl;
		sounddata[i].sound = Mix_LoadWAV(fname.c_str());
		if (!sounddata[i].sound) return 0; // TODO: spout error
		soundcache[filename] = sounddata[i].sound;
	}

	return &sounddata[i];
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

