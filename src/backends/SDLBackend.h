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

#include <SDL.h>
#include <array>
#include <memory>
#include "Backend.h"

class SDLRenderTarget : public RenderTarget {
	friend class SDLBackend;

protected:
	class SDLBackend *parent;
	SDL_Texture *texture;
	int drawablewidth, drawableheight;
	float scale = 1.0;
	int viewport_offset_top = 0;
	int viewport_offset_bottom = 0;
	
	SDLRenderTarget(SDLBackend *p) { parent = p; }

public:
	void renderCreaturesImage(const creaturesImage& tex, unsigned int frame, int x, int y, uint8_t transparency = 0, bool mirror = false);
	void renderCreaturesImage(const std::shared_ptr<creaturesImage>& tex, unsigned int frame, int x, int y, uint8_t transparency = 0, bool mirror = false);
	void renderLine(int x1, int y1, int x2, int y2, unsigned int colour);
	void blitRenderTarget(RenderTarget *src, int x, int y, int w, int h);
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	void renderClear();
	void renderDone();
};

class SDLBackend : public Backend {
	friend class SDLRenderTarget;

protected:
	SDL_Window *window = nullptr;
	int windowwidth, windowheight;
	SDL_Renderer *renderer = nullptr;
	SDLRenderTarget mainrendertarget;
	std::array<SDL_Color, 256> default_palette;
	float userscale = 1.0;

	void resizeNotify(int _w, int _h);

	SDL_Surface *getMainSDLSurface() { return SDL_GetWindowSurface(window); }

public:
	SDLBackend();
	void init();
	void initFrom(void *window_id);
	int run();
	void shutdown();
	void setUserScale(float scale);

	void resize(unsigned int w, unsigned int h) { resizeNotify(w, h); }

	bool pollEvent(BackendEvent &e);
	
	unsigned int ticks() { return SDL_GetTicks(); }
	
	Texture createTexture(const Image& image);
	Texture createTextureWithTransparentColor(const Image& image, Color transparent);

	RenderTarget *getMainRenderTarget() { return &mainrendertarget; }
	RenderTarget *newRenderTarget(unsigned int width, unsigned int height);
	void freeRenderTarget(RenderTarget *surf);
		
	bool keyDown(int key);
	int translateScancode(int key);
	
	void setDefaultPalette(span<Color> palette);
	void delay(int msec);
};

#endif
