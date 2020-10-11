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
	virtual void renderCreaturesImage(const creaturesImage& tex, unsigned int frame, int x, int y, uint8_t transparency = 0, bool mirror = false) { }
	virtual void renderCreaturesImage(const std::shared_ptr<creaturesImage>& tex, unsigned int frame, int x, int y, uint8_t transparency = 0, bool mirror = false) { }
	virtual void renderLine(int x1, int y1, int x2, int y2, unsigned int colour) { }
	virtual void blitRenderTarget(RenderTarget *src, int x, int y, int w, int h)  { }
	virtual unsigned int getWidth() const { return 800; }
	virtual unsigned int getHeight() const { return 600; }
	virtual void renderClear() {}
	virtual void renderDone() { }
};

class NullBackend : public Backend {
protected:
	NullRenderTarget surface;

public:
	virtual void init() { }
	virtual int networkInit() { return -1; }
	virtual int run() { return 1; }
	virtual void shutdown() { }

	virtual void resize(unsigned int width, unsigned int height) { }

	virtual unsigned int ticks() { return 0; }
	virtual bool pollEvent(BackendEvent &e) { return false; }
	virtual void handleEvents() { }
	virtual bool keyDown(int key) { return false; }

	virtual bool selfRender() { return false; }
	virtual void requestRender() { }
	
	virtual RenderTarget *getMainRenderTarget() { return &surface; }
	virtual RenderTarget *newRenderTarget(unsigned int width, unsigned int height) { return 0; }
	virtual void freeRenderTarget(RenderTarget *surf) { }
	
	virtual Texture createTexture(const Image& image) { return {}; }
	virtual Texture createTextureWithTransparentColor(const Image& image, Color transparent) { return {}; }
			
	virtual void setDefaultPalette(span<Color> data) { }
	virtual unsigned int textWidth(std::string text) { return 0; }
	virtual void delay(int msec) { }
};

#endif
