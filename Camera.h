/*
 *  Camera.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#ifndef _OPENC2E_CAMERA_H
#define _OPENC2E_CAMERA_H

#include "MetaRoom.h"
#include "SDLBackend.h"

enum cameratransition { none = 0, fliphorz = 1, burst = 2 };
enum trackstyle { brittle = 0, flexible = 1, hard = 2 };
enum panstyle { jump = 0, smoothscroll = 1, smoothscrollifvisible = 2 };

class Camera {
protected:
	unsigned int x, y;
	unsigned int metaroom;

	bool panning;
	unsigned int destx, desty;
	float velx, vely;

	class Agent *trackedagent;
	
public:
	Camera();
	virtual unsigned int const getWidth() = 0;
	virtual unsigned int const getHeight() = 0;

	unsigned int const getX() { return x; }
	unsigned int const getY() { return y; }
	unsigned int const getXCentre() { return x + (getWidth() / 2); }
	unsigned int const getYCentre() { return y + (getHeight() / 2); }

	MetaRoom *getMetaRoom();
	void goToMetaRoom(unsigned int m);
	void goToMetaRoom(unsigned int m, int x, int y, cameratransition transition);
	void moveTo(int _x, int _y, panstyle pan);
	void trackAgent(class Agent *a, int xp, int yp, trackstyle s, cameratransition transition);

	void tick();
};

class MainCamera : public Camera {
protected:
	SDLBackend *backend;

public:
	MainCamera() { backend = 0; }
	void setBackend(SDLBackend *b) { backend = b; }
	unsigned int const getWidth() { return backend->getWidth(); }
	unsigned int const getHeight() { return backend->getHeight(); }
};

#endif

