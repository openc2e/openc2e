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
#include <memory>
#include "Backend.h"

class SDLRenderTarget : public RenderTarget {
	friend class SDLBackend;

protected:
	class SDLBackend *parent;
	SDL_Surface *surface;
	unsigned int width, height;
	SDL_Color palette[256];
	
	SDLRenderTarget(SDLBackend *p) { parent = p; }

public:
	void render(shared_ptr<creaturesImage> image, unsigned int frame, int x, int y, bool trans = false, unsigned char transparency = 0, bool mirror = false, bool is_background = false);
	void renderLine(int x1, int y1, int x2, int y2, unsigned int colour);
	void blitRenderTarget(RenderTarget *src, int x, int y, int w, int h);
	unsigned int getWidth() const { return width; }
	unsigned int getHeight() const { return height; }
	void renderClear();
	void renderDone();
};

class SDLBackend : public Backend {
	friend class SDLRenderTarget;

protected:
	bool networkingup;

	SDL_Window *window = nullptr;
	SDLRenderTarget mainrendertarget;
	TCPsocket listensocket;

	void handleNetworking();
	void resizeNotify(int _w, int _h);

	SDL_Surface *getMainSDLSurface() { return mainrendertarget.surface; }

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

	RenderTarget *getMainRenderTarget() { return &mainrendertarget; }
	RenderTarget *newRenderTarget(unsigned int width, unsigned int height);
	void freeRenderTarget(RenderTarget *surf);
		
	bool keyDown(int key);
	int translateScancode(int key);
	
	void setPalette(uint8_t *data);
	void delay(int msec);
};

#endif
