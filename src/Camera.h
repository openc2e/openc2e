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

#include <vector>
#include "AgentRef.h"

enum cameratransition { none = 0, fliphorz = 1, burst = 2 };
enum trackstyle { brittle = 0, flexible = 1, hard = 2 };
enum panstyle { jump = 0, smoothscroll = 1, smoothscrollifvisible = 2 };

class Camera {
protected:
	int x, y;
	unsigned int metaroom;

	bool panning;
	unsigned int destx, desty;
	float velx, vely;

	AgentRef trackedagent;
	trackstyle trackingstyle;
	
public:
	Camera();
	virtual unsigned int getWidth() const = 0;
	virtual unsigned int getHeight() const = 0;

	unsigned int getX() const { return x; }
	unsigned int getY() const { return y; }
	unsigned int getXCentre() const { return x + (getWidth() / 2); }
	unsigned int getYCentre() const { return y + (getHeight() / 2); }
	
	class MetaRoom * getMetaRoom() const;
	void goToMetaRoom(unsigned int m);
	void goToMetaRoom(unsigned int m, int x, int y, cameratransition transition);
	virtual void moveTo(int _x, int _y, panstyle pan = jump);
	void moveToGlobal(int _x, int _y, panstyle pan = jump);
	void trackAgent(AgentRef a, int xp, int yp, trackstyle s, cameratransition transition);
	AgentRef trackedAgent() { return trackedagent; }
	void checkBounds();

	void tick();
	void updateTracking();

	virtual ~Camera() {}
};

class PartCamera : public Camera {
protected:
	class CameraPart *part;

public:
	PartCamera(class CameraPart *p) { part = p; }
	unsigned int getWidth() const;
	unsigned int getHeight() const;

	void setZoom(int pixels, int _x, int _y);
};

class Backend;

class MainCamera : public Camera {
protected:
	std::shared_ptr<Backend> backend;
	std::vector<AgentRef> floated;

public:
	MainCamera() { }
	void setBackend(std::shared_ptr<Backend> b) { backend = b; }
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	void moveTo(int _x, int _y, panstyle pan = jump);
	
	void addFloated(AgentRef);
	void delFloated(AgentRef);
};

#endif

/* vim: set noet: */
