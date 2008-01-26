/*
 *  SDLBackend.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Oct 24 2004.
 *  Copyright (c) 2004-2006 Alyssa Milburn. All rights reserved.
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

#include "SDL.h"
#include <SDL_net.h>
#include "Backend.h"

class SDLSurface : public Surface {
	friend class SDLBackend;

protected:
	SDL_Surface *surface;
	unsigned int width, height;
	SDL_Color palette[256];

public:
	void render(shared_ptr<creaturesImage> image, unsigned int frame, int x, int y, bool trans = false, unsigned char transparency = 0, bool mirror = false, bool is_background = false);
	void renderLine(int x1, int y1, int x2, int y2, unsigned int colour);
	void blitSurface(Surface *src, int x, int y, int w, int h);
	unsigned int getWidth() const { return width; }
	unsigned int getHeight() const { return height; }
	void renderDone();
};

class SDLBackend : public Backend {
protected:
	bool networkingup;

	SDLSurface mainsurface;
	TCPsocket listensocket;

	void handleNetworking();
	void resizeNotify(int _w, int _h);
	int translateKey(int key);

	SDL_Surface *getMainSDLSurface() { return mainsurface.surface; }

	virtual int idealBpp();

public:
	SDLBackend();	
	void init();
	int networkInit();
	void shutdown();

	void resize(unsigned int w, unsigned int h) { resizeNotify(w, h); }

	bool pollEvent(SomeEvent &e);
	
	unsigned int ticks() { return SDL_GetTicks(); }
	
	void handleEvents();

	bool selfRender() { return false; }
	void requestRender() { }

	Surface *getMainSurface() { return &mainsurface; }
	Surface *newSurface(unsigned int width, unsigned int height);
	void freeSurface(Surface *surf);
		
	bool keyDown(int key);
	
	void setPalette(uint8 *data);
};

#endif
