/*
 *  CompoundPart.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
 *  Copyright (c) 2004-2008 Alyssa Milburn. All rights reserved.
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

#include <cassert>
#include <memory>

#include "Agent.h"
#include "Backend.h"
#include "CompoundPart.h"
#include "CameraPart.h"
#include "Camera.h"
#include "CompoundAgent.h"
#include "fileformats/creaturesImage.h"
#include "encoding.h"
#include "Engine.h"
#include "imageManager.h"
#include "keycodes.h"
#include "World.h"

void CompoundPart::render(RenderTarget *renderer, int xoffset, int yoffset) {
	if (parent->visible) {
		partRender(renderer, xoffset + (int)parent->x, yoffset + (int)parent->y);
		if (parent->displaycore /*&& (id == 0)*/) {
			// TODO: tsk, this should be drawn along with the other craziness on the line plane, i expect
			int xoff = xoffset + (int)parent->x + x;
			int yoff = yoffset + (int)parent->y + y;
			renderer->renderLine(xoff + (getWidth() / 2), yoff, xoff + getWidth(), yoff + (getHeight() / 2), 0xFF0000CC);
			renderer->renderLine(xoff + getWidth(), yoff + (getHeight() / 2), xoff + (getWidth() / 2), yoff + getHeight(), 0xFF0000CC);
			renderer->renderLine(xoff + (getWidth() / 2), yoff + getHeight(), xoff, yoff + (getHeight() / 2), 0xFF0000CC);
			renderer->renderLine(xoff, yoff + (getHeight() / 2), xoff + (getWidth() / 2), yoff, 0xFF0000CC);
		}	
	}
}

bool CompoundPart::showOnRemoteCameras() {
	return !parent->camerashy();
}

bool CompoundPart::canClick() {
	return parent->activateable();
}

void CompoundPart::gainFocus() {
	assert(false);
}

void CompoundPart::loseFocus() {
	throw creaturesException("impossible loseFocus() call");
}

void CompoundPart::handleTranslatedChar(unsigned char c) {
	throw creaturesException("impossible handleTranslatedChar() call");
}

void CompoundPart::handleRawKey(uint8_t c) {
	throw creaturesException("impossible handleRawKey() call");
}

int CompoundPart::handleClick(float clickx, float clicky) {
	return parent->handleClick(clickx + x + parent->x, clicky + y + parent->y);
}

CompoundPart::CompoundPart(Agent *p, unsigned int _id, int _x, int _y, int _z) : parent(p), zorder(_z), id(_id) {
	auto compound_parent = dynamic_cast<CompoundAgent*>(parent);
	part_sequence_number = compound_parent ? compound_parent->nextPartSequenceNumber() : 0;

	addZOrder();	
	x = _x;
	y = _y;
	
	has_alpha = false;
}

CompoundPart::~CompoundPart() {
	world.zorder.erase(zorder_iter);
}

unsigned int CompoundPart::getZOrder() const {
	return parent->getZOrder() + zorder;
}

void CompoundPart::zapZOrder() {
	renderable::zapZOrder();
	world.zorder.erase(zorder_iter);
}

void CompoundPart::addZOrder() {
	renderable::addZOrder();
	zorder_iter = world.zorder.insert(this);	
}
