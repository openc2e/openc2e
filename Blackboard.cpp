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
}

Blackboard::Blackboard(std::string spritefile, unsigned int firstimage, unsigned int imagecount, 
		unsigned int tx, unsigned int ty, unsigned int bgcolour, unsigned int ckcolour,
		unsigned int alcolour) : CompoundAgent(spritefile, firstimage, imagecount) {
	textx = tx; texty = ty;
	backgroundcolour = bgcolour; chalkcolour = ckcolour; aliascolour = alcolour;
}

void Blackboard::addBlackboardString(unsigned int n, unsigned int id, std::string text) {
	strings[n] = std::pair<unsigned int, std::string>(id, text);
}

/* vim: set noet: */
