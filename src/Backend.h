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

#ifndef _BACKEND_H
#define _BACKEND_H

#include <memory>
#include <string>

#include "BackendEvent.h"
#include "imageformat.h"
#include "TextureAtlas.h"

using std::shared_ptr;

class creaturesImage;

class RenderTarget {
public:
	virtual void renderTexture(const TextureAtlasHandle& atlas, size_t i, int x, int y, uint8_t transparency = 0, bool mirror = false) = 0;
	virtual void renderLine(int x1, int y1, int x2, int y2, unsigned int colour) = 0;
	virtual void blitRenderTarget(RenderTarget *src, int x, int y, int w, int h) = 0;
	virtual unsigned int getWidth() const = 0;
	virtual unsigned int getHeight() const = 0;
	virtual void renderClear() = 0;
	virtual void renderDone() = 0;
	virtual ~RenderTarget() { }
};

class Backend {
public:
	virtual void init() = 0;
	virtual int networkInit() = 0;
	virtual void shutdown() = 0;

	virtual unsigned int ticks() = 0;	
	virtual bool pollEvent(BackendEvent &e) = 0;
	virtual void handleEvents() = 0;
	virtual bool keyDown(int key) = 0;

	virtual void resize(unsigned int width, unsigned int height) = 0;
	
	virtual RenderTarget *getMainRenderTarget() = 0;
	virtual RenderTarget *newRenderTarget(unsigned int width, unsigned int height) = 0;
	virtual void freeRenderTarget(RenderTarget *surf) = 0;

	virtual void setPalette(uint8_t *data) = 0;
	
	virtual TextureAtlasHandle createTextureAtlasFromCreaturesImage(const std::shared_ptr<creaturesImage>& image) = 0;
	
	virtual int run() = 0;
	virtual void delay(int msec) = 0;
	virtual ~Backend() { }
};

#endif
