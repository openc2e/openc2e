/*
 *  Blackboard.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Jan 12 2008.
 *  Copyright (c) 2008 Alyssa Milburn. All rights reserved.
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

#include "Blackboard.h"
#include "Engine.h"
#include "Backend.h"

Blackboard::Blackboard(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
		std::string spritefile, unsigned int firstimage, unsigned int imagecount, unsigned int tx,
		unsigned int ty, unsigned int bgcolour, unsigned int ckcolour, unsigned int alcolour) :
		CompoundAgent(family, genus, species, plane, spritefile, firstimage, imagecount) {
	textx = tx; texty = ty;
	backgroundcolour = bgcolour; chalkcolour = ckcolour; aliascolour = alcolour;

	if (engine.version == 1)
		strings.resize(16, std::pair<unsigned int, std::string>(0, std::string()));
	else
		strings.resize(48, std::pair<unsigned int, std::string>(0, std::string()));

	// add the part responsible for text; id #10 keeps it safely out of the way
	BlackboardPart *p = new BlackboardPart(this, 10);
	addPart(p);
}

Blackboard::Blackboard(std::string spritefile, unsigned int firstimage, unsigned int imagecount, 
		unsigned int tx, unsigned int ty, unsigned int bgcolour, unsigned int ckcolour,
		unsigned int alcolour) : CompoundAgent(spritefile, firstimage, imagecount) {
	textx = tx; texty = ty;
	backgroundcolour = bgcolour; chalkcolour = ckcolour; aliascolour = alcolour;

	if (engine.version == 1)
		strings.resize(16, std::pair<unsigned int, std::string>(0, std::string()));
	else
		strings.resize(48, std::pair<unsigned int, std::string>(0, std::string()));

	BlackboardPart *p = new BlackboardPart(this, 10);
	addPart(p);
}

void Blackboard::addBlackboardString(unsigned int n, unsigned int id, std::string text) {
	strings[n] = std::pair<unsigned int, std::string>(id, text);
}

void Blackboard::renderText(class Surface *renderer, int xoffset, int yoffset) {
	// TODO: check var[0] is valid

	// TODO: is +1 really the right fix here?
	renderer->renderText(xoffset + textx + 1, yoffset + texty + 1, strings[var[0].getInt()].second, chalkcolour, backgroundcolour);
}

BlackboardPart::BlackboardPart(Blackboard *p, unsigned int _id) : CompoundPart(p, _id, 0, 0, 1) {
	// TODO: think about plane
}

void BlackboardPart::partRender(class Surface *renderer, int xoffset, int yoffset) {
	dynamic_cast<Blackboard *>(parent)->renderText(renderer, xoffset, yoffset);
}

/* vim: set noet: */
