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

#include "common/backend/Backend.h"

#include <SDL.h>
#include <array>
#include <memory>
#include <string>

class SDLBackend;

class SDLRenderTarget : public RenderTarget {
  public:
	SDLBackend* parent;
	SDL_Texture* texture = nullptr;
	int drawablewidth = 0;
	int drawableheight = 0;
	float scale = 1.0;
	int viewport_offset_top = 0;
	int viewport_offset_bottom = 0;

	SDLRenderTarget(SDLBackend* parent);
	SDLRenderTarget(const SDLRenderTarget&) = delete;
	SDLRenderTarget(SDLRenderTarget&&) = delete;
	SDLRenderTarget& operator=(const SDLRenderTarget&) = delete;
	SDLRenderTarget& operator=(SDLRenderTarget&&) = delete;
	~SDLRenderTarget();

	void renderTexture(const Texture& tex, Rect src, RectF dest, RenderOptions options) override;
	void renderCreaturesImage(creaturesImage& tex, unsigned int frame, int x, int y, RenderOptions options) override;
	void renderLine(float x1, float y1, float x2, float y2, unsigned int color) override;
	void blitRenderTarget(RenderTarget* src, int x, int y, int w, int h) override;
	unsigned int getWidth() const override;
	unsigned int getHeight() const override;
	void renderClear() override;
	void setViewportOffsetTop(int offset_top) override;
	void setViewportOffsetBottom(int offset_bottom) override;
};

class SDLBackend : public Backend {
	friend class SDLRenderTarget;

  protected:
	int windowwidth, windowheight;
	SDL_Renderer* renderer = nullptr;
	std::shared_ptr<SDLRenderTarget> mainrendertarget;
	Uint32 last_frame_end = 0;

	SDLBackend();
	SDLBackend(const SDLBackend&) = delete;
	SDLBackend(SDLBackend&&) = delete;
	SDLBackend& operator=(const SDLBackend&) = delete;
	SDLBackend& operator=(SDLBackend&&) = delete;

	SDL_Surface* getMainSDLSurface() { return SDL_GetWindowSurface(window); }
	void resizeNotify(int _w, int _h);
	int translateScancode(int key);

  public:
	SDL_Window* window = nullptr;

	static Backend* get_instance();
	void init(const std::string& name, int width, int height) override;
	void waitForNextDraw() override;
	void drawDone() override;
	void shutdown() override;

	void resize(int w, int h) override { resizeNotify(w, h); }

	bool pollEvent(BackendEvent& e) override;

	unsigned int ticks() override { return SDL_GetTicks(); }

	Texture createTextureFromImage(const Image& image) override;
	Texture createTextureWithTransparentColor(const Image& image, Color transparent) override;

	std::shared_ptr<RenderTarget> getMainRenderTarget() override;
	std::shared_ptr<RenderTarget> newRenderTarget(unsigned int width, unsigned int height) override;

	bool keyDown(int key) override;
};

#endif
