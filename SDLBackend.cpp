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
#include "World.h"

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
	mainsurface.width = _w;
	mainsurface.height = _h;
	mainsurface.surface = SDL_SetVideoMode(_w, _h, 0, SDL_RESIZABLE);
	assert(mainsurface.surface != 0);
}

void SDLBackend::init(bool enable_sound) {
	int init = SDL_INIT_VIDEO;
	if (enable_sound) init |= SDL_INIT_AUDIO;

	if (SDL_Init(init) < 0) {
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

	resizeNotify(800, 600);
	
	SDL_WM_SetCaption("openc2e (development build)", "openc2e");
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
		std::string fname = world.findFile(std::string("/Sounds/") + filename + ".wav");
		if (fname.size() == 0) return 0;
		//std::cout << "trying to load " << fname << std::endl;
		sounddata[i].sound = Mix_LoadWAV(fname.c_str());
		if (!sounddata[i].sound) return 0; // TODO: spout error
		soundcache[filename] = sounddata[i].sound;
	}

	return &sounddata[i];
}

void SDLSurface::renderLine(int x1, int y1, int x2, int y2, unsigned int colour) {
	aalineColor(surface, x1, y1, x2, y2, colour);
}

//*** code to mirror 16bpp surface - slow, we should cache this!

Uint16 *pixelPtr(SDL_Surface *surf, int x, int y) {
	return (Uint16 *)((Uint8 *)surf->pixels + (y * surf->pitch) + (x * 2));
}

SDL_Surface *MirrorSurface(SDL_Surface *surf) {
	SDL_Surface* newsurf = SDL_CreateRGBSurface(SDL_HWSURFACE, surf->w, surf->h, surf->format->BitsPerPixel, surf->format->Rmask, surf->format->Gmask, surf->format->Bmask, surf->format->Amask);
	SDL_BlitSurface(surf, 0, newsurf, 0);

	if (SDL_MUSTLOCK(newsurf))
		if (SDL_LockSurface(newsurf) == -1) {
			SDL_FreeSurface(newsurf);
			throw creaturesException("SDLBackend failed to lock surface for mirroring");
		}

	for (int y = 0; y < newsurf->h; y++) {
		for (int x = 0; x < (newsurf->w / 2); x++) {
			Uint16 *one = pixelPtr(newsurf, x, y);
			Uint16 *two = pixelPtr(newsurf, (newsurf->w - 1) - x, y);
			Uint16 temp = *one;
			*one = *two;
			*two = temp;
		}
	}
	
	if (SDL_MUSTLOCK(newsurf))
		SDL_UnlockSurface(newsurf);

	return newsurf;
}

//*** end mirror code

void SDLSurface::render(creaturesImage *image, unsigned int frame, int x, int y, bool trans, unsigned char transparency, bool mirror) {
	// don't bother rendering off-screen stuff
	if (x >= width) return; if (y >= height) return;
	if ((x + image->width(frame)) <= 0) return;
	if ((y + image->height(frame)) <= 0) return;

	// create surface
	unsigned int rmask, gmask, bmask;
	if (image->is565()) {
		rmask = 0xF800; gmask = 0x07E0; bmask = 0x001F;
	} else {
		rmask = 0x7C00; gmask = 0x03E0; bmask = 0x001F;
	}
	SDL_Surface *surf = SDL_CreateRGBSurfaceFrom(image->data(frame),
							image->width(frame), image->height(frame),
							16, // depth
							image->width(frame) * 2, // pitch
							rmask, gmask, bmask, 0); // RGBA mask
	
	// try mirroring, if necessary
	try {
		if (mirror) {
			SDL_Surface *newsurf = MirrorSurface(surf);
			SDL_FreeSurface(surf);
			surf = newsurf;
		}
	} catch (std::exception &e) {
		SDL_FreeSurface(surf);
		throw;
	}
	
	// set colour-keying/alpha
	// TODO: presumably there's a nicer way of doing this than dynamic_cast :P
	if (!dynamic_cast<blkImage *>(image)) SDL_SetColorKey(surf, SDL_SRCCOLORKEY, 0);
	if (trans) SDL_SetAlpha(surf, SDL_SRCALPHA, 255 - transparency);
	
	// do actual blit
	SDL_Rect destrect;
	destrect.x = x; destrect.y = y;
	SDL_BlitSurface(surf, 0, surface, &destrect);

	// free surface
	SDL_FreeSurface(surf);
}

void SDLSurface::renderDone() {
	SDL_Flip(surface);
}

void SDLSurface::blitSurface(SDLSurface *src, int x, int y, int w, int h) {
	// TODO: evil use of internal SDL api
	SDL_Rect r; r.x = x; r.y = y; r.w = w; r.h = h;
	SDL_SoftStretch(src->surface, 0, surface, &r);
}

SDLSurface *SDLBackend::newSurface(unsigned int w, unsigned int h) {
	SDL_Surface *surf = mainsurface.surface;
	SDL_Surface* underlyingsurf = SDL_CreateRGBSurface(SDL_HWSURFACE, w, h, surf->format->BitsPerPixel, surf->format->Rmask, surf->format->Gmask, surf->format->Bmask, surf->format->Amask);
	assert(underlyingsurf);
	SDLSurface *newsurf = new SDLSurface();
	newsurf->surface = underlyingsurf;
	newsurf->width = w;
	newsurf->height = h;
	return newsurf;
}

void SDLBackend::freeSurface(SDLSurface *surf) {
	SDL_FreeSurface(surf->surface);
	delete surf;
}

// left out: menu, select, execute, snapshot, numeric keypad, f keys
#define keytrans_size 25
struct _keytrans { int sdl, windows; } keytrans[keytrans_size] = {
	{ SDLK_BACKSPACE, 8 },
	{ SDLK_TAB, 9 },
	{ SDLK_CLEAR, 12 },
	{ SDLK_RETURN, 13 },
	{ SDLK_RSHIFT, 16 },
	{ SDLK_LSHIFT, 16 },
	{ SDLK_RCTRL, 17 },
	{ SDLK_LCTRL, 17 },
	{ SDLK_PAUSE, 19 },
	{ SDLK_CAPSLOCK, 20 },
	{ SDLK_ESCAPE, 27 },
	{ SDLK_SPACE, 32 },
	{ SDLK_PAGEUP, 33 },
	{ SDLK_PAGEDOWN, 34 },
	{ SDLK_END, 35 },
	{ SDLK_HOME, 36 },
	{ SDLK_LEFT, 37 },
	{ SDLK_UP, 38 },
	{ SDLK_RIGHT, 39 },
	{ SDLK_DOWN, 40 },
	{ SDLK_PRINT, 42 },
	{ SDLK_INSERT, 45 },
	{ SDLK_DELETE, 46 },
	{ SDLK_NUMLOCK, 144 }
};

// TODO: handle f keys (112-123 under windows, SDLK_F1 = 282 under sdl)
 
// TODO: this is possibly not a great idea, we should maybe maintain our own state table
bool SDLBackend::keyDown(int key) {
	Uint8 *keystate = SDL_GetKeyState(NULL);
	
	for (unsigned int i = 0; i < keytrans_size; i++) {
		if (keytrans[i].windows == key)
			if (keystate[keytrans[i].sdl])
				return true;
	}

	return false;
}

int SDLBackend::translateKey(int key) {
	if (key >= 97 && key <= 122) { // lowercase letters
		return key - 32; // capitalise
	}
	if (key >= 48 && key <= 57) { // numbers
		return key;
	}

	for (unsigned int i = 0; i < keytrans_size; i++) {
		if (keytrans[i].sdl == key)
			return keytrans[i].windows;
	}

	return -1;
}

