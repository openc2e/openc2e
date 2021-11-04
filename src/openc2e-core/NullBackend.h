/*
 *  NullBackend.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Fri Dec 15 2006.
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

#ifndef _NULLBACKEND_H
#define _NULLBACKEND_H

#include "Backend.h"

#include <memory>

class NullRenderTarget : public RenderTarget {
  public:
	virtual void renderCreaturesImage(creaturesImage&, unsigned int, int, int, RenderOptions) {}
	virtual void renderLine(int, int, int, int, unsigned int) {}
	virtual void blitRenderTarget(RenderTarget*, int, int, int, int) {}
	virtual unsigned int getWidth() const { return 800; }
	virtual unsigned int getHeight() const { return 600; }
	virtual void renderClear() {}
	virtual void renderDone() {}
};

class NullBackend : public Backend {
  protected:
	NullRenderTarget surface;

  public:
	virtual void init() {}
	virtual int run() { return 1; }
	virtual void shutdown() {}

	virtual void resize(unsigned int, unsigned int) {}

	virtual unsigned int ticks() { return 0; }
	virtual bool pollEvent(BackendEvent&) { return false; }
	virtual bool keyDown(int) { return false; }

	virtual bool selfRender() { return false; }
	virtual void requestRender() {}

	virtual RenderTarget* getMainRenderTarget() { return &surface; }
	virtual RenderTarget* newRenderTarget(unsigned int, unsigned int) { return nullptr; }
	virtual void freeRenderTarget(RenderTarget*) {}

	virtual Texture createTexture(const Image&) { return {}; }
	virtual Texture createTextureWithTransparentColor(const Image&, Color) { return {}; }

	virtual void setDefaultPalette(span<Color>) {}
	virtual unsigned int textWidth(std::string) { return 0; }
	virtual void delay(int) {}
};

#endif
