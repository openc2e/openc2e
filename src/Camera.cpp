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
#include "CameraPart.h"
#include "Engine.h"
#include "World.h"
#include "Backend.h"
#include "Map.h"
#include "MetaRoom.h"
#include "Agent.h"
#include <cassert>

Camera::Camera() {
	metaroom = 0;
	panning = false;
	x = 0;
	y = 0;
}

MetaRoom * Camera::getMetaRoom() const {
	return world.map->getMetaRoom(metaroom);
}

void Camera::goToMetaRoom(unsigned int m) {
	metaroom = m;
	moveTo(getMetaRoom()->x(), getMetaRoom()->y());
}

void Camera::goToMetaRoom(unsigned int m, int _x, int _y, cameratransition transition) {
	metaroom = m;
	moveTo(_x, _y);
	// TODO: transition
	
	checkBounds();
}

void Camera::moveTo(int _x, int _y, panstyle pan) {
	// TODO: we don't break tracking according to trackingstyle.. atm updateTracking() calls this, also
	x = _x;
	y = _y;

	// TODO: panning
	
	checkBounds();
}

void Camera::moveToGlobal(int _x, int _y, panstyle pan) {
	MetaRoom *m = world.map->metaRoomAt(_x, _y);
	if (m) {
		if (m->id != metaroom) pan = jump; // inter-metaroom panning is always jump
		metaroom = m->id;
	}

	moveTo(_x, _y, pan);
}

void MainCamera::moveTo(int _x, int _y, panstyle pan) {
	int xoffset = _x - x;
	int yoffset = _y - y;
	Camera::moveTo(_x, _y, pan);

	for (auto & i : floated) {
		assert(*i);
		i->moveTo(i->x + xoffset, i->y + yoffset);
	}
}

void MainCamera::addFloated(AgentRef a) {
	assert(a);
	floated.push_back(a);
}

void MainCamera::delFloated(AgentRef a) {
	assert(a);
	std::vector<AgentRef>::iterator i = std::find(floated.begin(), floated.end(), a);
	if (i == floated.end()) return;
	floated.erase(i);
}

void Camera::trackAgent(AgentRef a, int xp, int yp, trackstyle s, cameratransition transition) {
	trackedagent = a;
	trackingstyle = s;
	updateTracking();
}

void Camera::checkBounds() {
	MetaRoom *m = getMetaRoom();
	if (!m) return;
	
	if (m->wraparound()) {
		// handle wrapping around, if necessary
		if (x < (int)m->x()) {
			moveTo(x + m->width(), y);
		} else if (x > (int)m->x() + (int)m->width()) {
			moveTo(x - m->width(), y);
		}
	} else {
		// refuse to move beyond the boundaries
		if (x < (int)m->x()) {
			moveTo(m->x(), y);
		} else if (x + getWidth() > m->x() + m->width()) {
			moveTo(m->x() + m->width() - getWidth(), y);
		}
	}

	// refuse to move beyond the boundaries
	if (y < (int)m->y()) {
		moveTo(x, m->y());
	} else if (y + getHeight() > m->y() + m->height()) {
		moveTo(x, m->y() + m->height() - getHeight());
	}
}

void Camera::tick() {
	updateTracking();
}

void Camera::updateTracking() {
	if (!trackedagent) return;

	// TODO: not very intelligent :) also, are int casts correct?
	int trackx = (int)trackedagent->x + ((int)trackedagent->getWidth() / 2) - (int)(getWidth() / 2);
	int tracky = (int)trackedagent->y + ((int)trackedagent->getHeight() / 2) - (int)(getHeight() / 2);
	moveToGlobal(trackx, tracky);
}

unsigned int MainCamera::getWidth() const {
	if ((!getMetaRoom()) || (engine.backend->getMainRenderTarget()->getWidth() < getMetaRoom()->width()))
		return engine.backend->getMainRenderTarget()->getWidth();
	else
		return getMetaRoom()->width();
}

unsigned int MainCamera::getHeight() const {
	if ((!getMetaRoom()) || (engine.backend->getMainRenderTarget()->getHeight() < getMetaRoom()->height()))
		return engine.backend->getMainRenderTarget()->getHeight();
	else
		return getMetaRoom()->height();
}

unsigned int PartCamera::getWidth() const {
	// TODO: update from ZOOM values
	return part->cameraWidth();
}

unsigned int PartCamera::getHeight() const {
	// TODO: update from ZOOM values
	return part->cameraHeight();
}

/* vim: set noet: */
