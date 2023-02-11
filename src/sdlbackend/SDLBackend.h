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
	void renderLine(float x1, float y1, float x2, float y2, Color color) override;
	void blitRenderTarget(RenderTarget* src, RectF dest) override;

	// dimensions in logical window points (device-independent pixels), not pixels
	int32_t getWidth() const override;
	int32_t getHeight() const override;
	void setClip(RectF dest) override;

	void renderClear() override;
	void setViewportOffsetTop(int offset_top) override;
	void setViewportOffsetBottom(int offset_bottom) override;
};

class SDLBackend : public Backend {
	friend class SDLRenderTarget;

  protected:
	SDL_Renderer* renderer = nullptr;
	std::shared_ptr<SDLRenderTarget> mainrendertarget;

	SDLBackend();
	SDLBackend(const SDLBackend&) = delete;
	SDLBackend(SDLBackend&&) = delete;
	SDLBackend& operator=(const SDLBackend&) = delete;
	SDLBackend& operator=(SDLBackend&&) = delete;

	int translateScancode(int key);

  public:
	SDL_Window* window = nullptr;

	static Backend* get_instance();
	void init(const std::string& name, int32_t width, int32_t height) override;
	void run(std::function<bool()> render_callback) override;
	void shutdown() override;

	bool pollEvent(BackendEvent& e) override;

	unsigned int ticks() override { return SDL_GetTicks(); }

	Texture createTexture(int32_t width, int32_t height) override;
	void updateTexture(Texture& tex, Rect location, const Image& image) override;

	std::shared_ptr<RenderTarget> getMainRenderTarget() override;
	std::shared_ptr<RenderTarget> newRenderTarget(int32_t width, int32_t height) override;

	bool keyDown(Openc2eKeycode key) override;
};

#endif
