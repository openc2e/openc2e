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

#include "creaturesImage.h"
#include "endianlove.h"
#include <map>

struct SoundSlot {
	class Agent *agent;

	virtual void play() = 0;
	virtual void playLooped() = 0;
	virtual void adjustPanning(int angle, int distance) = 0;
	virtual void fadeOut() = 0;
	virtual void stop() = 0;
	virtual ~SoundSlot() { }
};

enum eventtype { eventquit, eventkeydown, eventspecialkeydown, eventmousebuttondown, eventmousebuttonup, eventmousemove, eventresizewindow };
enum eventbuttons { buttonleft, buttonright, buttonmiddle, buttonwheeldown, buttonwheelup };

struct SomeEvent {
	eventtype type;
	int x, y, xrel, yrel;
	int key;
	eventbuttons button;
};

class Surface {
public:
	virtual void render(creaturesImage *image, unsigned int frame, int x, int y, bool trans = false, unsigned char transparency = 0, bool mirror = false, bool is_background = false) = 0;
	virtual void renderLine(int x1, int y1, int x2, int y2, unsigned int colour) = 0;
	virtual void blitSurface(Surface *src, int x, int y, int w, int h) = 0;
	virtual int getWidth() const = 0;
	virtual int getHeight() const = 0;
	virtual void renderDone() = 0;
	virtual ~Surface() { }
};

class Backend {
public:
	virtual void init() = 0;
	virtual void soundInit() = 0;
	virtual int networkInit() = 0;
	virtual void shutdown() = 0;

	virtual unsigned int ticks() = 0;	
	virtual bool pollEvent(SomeEvent &e) = 0;
	virtual void handleEvents() = 0;
	virtual bool keyDown(int key) = 0;
	
	virtual Surface *getMainSurface() = 0;
	virtual Surface *newSurface(unsigned int width, unsigned int height) = 0;
	virtual void freeSurface(Surface *surf) = 0;
			
	virtual void setPalette(uint8 *data) = 0;
	virtual SoundSlot *getAudioSlot(std::string filename) = 0;
	virtual ~Backend() { }
};

#endif
