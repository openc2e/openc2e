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

#include "MetaRoom.h"
#include "SDLBackend.h"

class Camera {
protected:
	unsigned int x, y;
	MetaRoom *metaroom;

	bool panning;
	unsigned int destx, desty;
	float velx, vely;

	class Agent *trackedagent;
	
public:
	virtual unsigned int const getWidth() = 0;
	virtual unsigned int const getHeight() = 0;
	void setSize(int width, int height);

	unsigned int const getX() { return x; }
	unsigned int const getY() { return y; }
	unsigned int const getXCentre() { return x + (getWidth() / 2); }
	unsigned int const getYCentre() { return y + (getHeight() / 2); }
	void moveTo(int width, int height, bool pan);
	void moveToCentered(int width, int height, bool pan);

	void trackAgent(class Agent *a);

	void tick();
};

class MainCamera : public Camera {
protected:
	SDLBackend *backend;

public:
	MainCamera(SDLBackend *b);
	unsigned int const getWidth() { return backend->getWidth(); }
	unsigned int const getHeight() { return backend->getHeight(); }
};

