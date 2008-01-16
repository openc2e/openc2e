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

class NullSurface : public Surface {
public:
	virtual void render(shared_ptr<creaturesImage> image, unsigned int frame, int x, int y, bool trans = false, unsigned char transparency = 0, bool mirror = false, bool is_background = false) { }
	virtual void renderLine(int x1, int y1, int x2, int y2, unsigned int colour) { }
	virtual void blitSurface(Surface *src, int x, int y, int w, int h)  { }
	virtual unsigned int getWidth() const { return 800; }
	virtual unsigned int getHeight() const { return 600; }
	virtual void renderDone() { }
};

class NullBackend : public Backend {
protected:
	NullSurface surface;

public:
	virtual void init() { }
	virtual int networkInit() { return -1; }
	virtual void shutdown() { }

	virtual void resize(unsigned int width, unsigned int height) { }

	virtual unsigned int ticks() { return 0; }
	virtual bool pollEvent(SomeEvent &e) { return false; }
	virtual void handleEvents() { }
	virtual bool keyDown(int key) { return false; }
	
	virtual Surface *getMainSurface() { return &surface; }
	virtual Surface *newSurface(unsigned int width, unsigned int height) { return 0; }
	virtual void freeSurface(Surface *surf) { }
			
	virtual void setPalette(uint8 *data) { }
};

#endif
