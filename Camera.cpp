/*
 *  Camera.cpp
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

#include "Camera.h"
#include "World.h"

Camera::Camera() {
	x = 0;
	y = 0;
	metaroom = 0;
	panning = false;
	trackedagent = 0;
}

MetaRoom *Camera::getMetaRoom() {
	return world.map.getMetaRoom(metaroom);
}

void Camera::goToMetaRoom(unsigned int m) {
	metaroom = m;
	x = getMetaRoom()->x();
	y = getMetaRoom()->y();
}

void Camera::goToMetaRoom(unsigned int m, int _x, int _y, cameratransition transition) {
	metaroom = m;
	x = _x;
	y = _y;
	// TODO: transition
}

void Camera::moveTo(int _x, int _y, panstyle pan) {
	x = _x;
	y = _y;
	// TODO: panning
}

void Camera::trackAgent(class Agent *a, int xp, int yp, trackstyle s, cameratransition transition) {
	// TODO
}

void Camera::tick() {
	// TODO
}
					
/* vim: set noet: */
