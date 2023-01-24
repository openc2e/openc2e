/*
 *  Backend.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Dec 2 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

#pragma once

#include "BackendEvent.h"
#include "BackendTexture.h"
#include "common/Image.h"
#include "common/math/Rect.h"
#include "common/math/RectF.h"
#include "common/span.h"

#include <memory>
#include <string>

// reasonable defaults
constexpr int OPENC2E_DEFAULT_WIDTH = 800;
constexpr int OPENC2E_DEFAULT_HEIGHT = 600;

class creaturesImage;

struct RenderOptions {
	uint8_t alpha = 255;
	bool mirror = false;
	float scale = 1.0;
	bool override_drawsize = false;
	int overridden_drawwidth;
	int overridden_drawheight;
};

class RenderTarget {
  public:
	virtual void renderTexture(const Texture& tex, Rect src, RectF dest, RenderOptions options = {}) = 0;
	virtual void renderCreaturesImage(creaturesImage& tex, unsigned int frame, int x, int y, RenderOptions options = {}) = 0;
	void renderCreaturesImage(const std::shared_ptr<creaturesImage>& tex, unsigned int frame, int x, int y, RenderOptions options = {}) {
		assert(tex.get() != nullptr);
		renderCreaturesImage(*tex.get(), frame, x, y, options);
	}
	virtual void renderLine(float x1, float y1, float x2, float y2, unsigned int color) = 0;
	virtual void blitRenderTarget(RenderTarget* src, int x, int y, int w, int h) = 0;
	virtual unsigned int getWidth() const = 0;
	virtual unsigned int getHeight() const = 0;
	virtual void renderClear() = 0;
	virtual void setViewportOffsetTop(int offset_top) = 0;
	virtual void setViewportOffsetBottom(int offset_bottom) = 0;
	virtual ~RenderTarget() {}
};

class Backend {
  public:
	virtual void init(const std::string& name, int width, int height) = 0;
	virtual void shutdown() = 0;

	virtual unsigned int ticks() = 0;
	virtual bool pollEvent(BackendEvent& e) = 0;
	virtual bool keyDown(int key) = 0;

	virtual std::shared_ptr<RenderTarget> getMainRenderTarget() = 0;
	virtual std::shared_ptr<RenderTarget> newRenderTarget(unsigned int width, unsigned int height) = 0;

	virtual Texture createTexture(unsigned int width, unsigned int height) = 0;
	virtual void updateTexture(Texture& tex, Rect location, const Image& image) = 0;
	Texture createTextureFromImage(const Image& image) {
		Texture tex = createTexture(image.width, image.height);
		updateTexture(tex, Rect{}, image);
		return tex;
	}

	virtual void waitForNextDraw() = 0;
	virtual void drawDone() = 0;
	virtual ~Backend() {}
};

// global instance
Backend* get_backend();
void set_backend(Backend*);