/*
 *  CompoundAgent.cpp
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

#include "CompoundAgent.h"
#include "openc2e.h"
#include "c16Image.h"
#include "World.h"
#include <algorithm> // sort
#include <functional> // binary_function

// the list of parts is a list of pointers to CompoundPart, so we need a custom sort
struct less_part : public std::binary_function<CompoundPart *, CompoundPart *, bool> {
	bool operator()(CompoundPart *x, CompoundPart *y) { return *x < *y; }
};

void CompoundAgent::render(SDLBackend *renderer, int xoffset, int yoffset) {
	// todo: we're ignoring zorder here.. (it's handled by the sort in addPart)
	for (std::vector<CompoundPart *>::iterator i = parts.begin(); i != parts.end(); i++) {
		(*i)->render(renderer, xoffset + (int)x, yoffset + (int)y);
	}

	if (displaycore) {
		// draw core
		int xoff = xoffset + x;
		int yoff = yoffset + y;
		renderer->renderLine(xoff + (getWidth() / 2), yoff, xoff + getWidth(), yoff + (getHeight() / 2), 0xFF0000CC);
		renderer->renderLine(xoff + getWidth(), yoff + (getHeight() / 2), xoff + (getWidth() / 2), yoff + getHeight(), 0xFF0000CC);
		renderer->renderLine(xoff + (getWidth() / 2), yoff + getHeight(), xoff, yoff + (getHeight() / 2), 0xFF0000CC);
		renderer->renderLine(xoff, yoff + (getHeight() / 2), xoff + (getWidth() / 2), yoff, 0xFF0000CC);
	}
}

void CompoundAgent::addPart(CompoundPart *p) {
	assert(!part(p->id)); // todo: handle better

	// todo: we should prbly insert at the right place, not call sort
	parts.push_back(p);
	std::sort(parts.begin(), parts.end(), less_part());

	// this is an icky hack.. for instance, it doesn't take account of negative x/y (which we don't have any way of handling right now)
	if (p->x + (int)p->getWidth() > (int)fullwidth) fullwidth = p->x + p->getWidth();
	if (p->y + (int)p->getHeight() > (int)fullheight) fullheight = p->y + p->getHeight();
}

void CompoundAgent::delPart(unsigned int id) {
	caos_assert(id != 0);

	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		if ((*x)->id == id) { delete *x; parts.erase(x); return; }
	}
	
	throw caosException("delPart got a bad id"); // TODO: handle this exception properly
}

CompoundPart *CompoundAgent::part(unsigned int id) {
	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		if ((*x)->id == id) return *x;
	}
	return 0;
}

CompoundAgent::CompoundAgent(unsigned char _family, unsigned char _genus, unsigned short _species, unsigned int plane,
				std::string spritefile, unsigned int firstimage, unsigned int imagecount) :
				Agent(_family, _genus, _species, plane), width(0), height(0) {
	setAttributes(0);
	// TODO: we ignore image count acos it sucks
	CompoundPart *p = new DullPart(this, 0, spritefile, firstimage, 0, 0, 0);
	fullwidth = width = p->getWidth();
	fullheight = height = p->getHeight();
	addPart(p);
}

CompoundAgent::~CompoundAgent() {
	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		delete *x;
	}
}

unsigned int calculateScriptId(unsigned int message_id); // from caosVM_agent.cpp, TODO: move into shared file

#include <iostream>

void CompoundAgent::handleClick(float clickx, float clicky) {
	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		if ((clickx >= (*x)->x) && (clicky >= (*x)->y) &&
				(clickx <= (*x)->x + (*x)->getWidth()) && (clicky <= (*x)->y + (*x)->getHeight())) {
			ButtonPart *b = dynamic_cast<ButtonPart *>(*x);
			if (b) {
				// TODO: check button transparency if needed
				fireScript(calculateScriptId(b->messageid), (Agent *)world.hand());
				// TODO: do we need to return here, or should we always passthrough to Agent::handleClick?
				return;
			}
			TextEntryPart *t = dynamic_cast<TextEntryPart *>(*x);
			if (t) {
				world.setFocus(this, t);
				return;
			}
		}
	}

	// TODO: this check should possibly be in Agent::handleClick, along with the attribute
	if (activateable)
		Agent::handleClick(clickx, clicky);
}

void CompoundAgent::tick() {
	Agent::tick();

	for (std::vector<CompoundPart *>::iterator x = parts.begin(); x != parts.end(); x++) {
		(*x)->tick();
	}
}

/* vim: set noet: */
