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

#include <cassert>

#include "Backend.h"
#include "Blackboard.h"
#include "Engine.h"
#include "keycodes.h"
#include "World.h" // setFocus

Blackboard::Blackboard(std::string spritefile, unsigned int firstimage, unsigned int imagecount, 
		unsigned int tx, unsigned int ty, unsigned int bgcolour, unsigned int ckcolour,
		unsigned int alcolour) : CompoundAgent(spritefile, firstimage, imagecount) {
	textx = tx; texty = ty;
	backgroundcolour = bgcolour; chalkcolour = ckcolour; aliascolour = alcolour;
	ourPart = 0;
	editing = false;

	if (engine.version == 1)
		strings.resize(16, std::pair<unsigned int, std::string>(0, std::string()));
	else
		strings.resize(48, std::pair<unsigned int, std::string>(0, std::string()));
}

void Blackboard::addPart(CompoundPart *p) {
	CompoundAgent::addPart(p);

	// if we're adding the first part..
	if (parts.size() == 1) {
		// add the part responsible for text; id #10 keeps it safely out of the way
		ourPart = new BlackboardPart(this, 10);
		addPart(ourPart);
	}
}

std::string Blackboard::getText() {
	if (var[0].hasInt() && var[0].getInt() >= 0 && (unsigned int)var[0].getInt() < strings.size())
		return strings[var[0].getInt()].second;
	else
		return std::string();
}

void Blackboard::showText(bool show) {
	if (editing) stopEditing(false);

	if (show) {
		currenttext = getText();
	} else {
		currenttext.clear();
	}
}

void Blackboard::addBlackboardString(unsigned int n, unsigned int id, std::string text) {
	strings[n] = std::pair<unsigned int, std::string>(id, text);
}

void Blackboard::renderText(RenderTarget *renderer, int xoffset, int yoffset) {
	std::string ourtext = currenttext;
	if (editing) ourtext += "_"; // TODO: should this be rendered in aliascolour?

	// TODO: is +1 really the right fix here?
	renderer->renderText(xoffset + textx + 1, yoffset + texty + 1, ourtext, chalkcolour, backgroundcolour);
}

void Blackboard::startEditing() {
	assert(!editing);

	if (var[0].hasInt() && var[0].getInt() >= 0 && (unsigned int)var[0].getInt() < strings.size()) {
		editing = true;
		editingindex = var[0].getInt();
		strings[editingindex].second = currenttext = "";
	} else {
		// TODO: this will probably be thrown all the way to main() :-(
		throw creaturesException("tried to start editing a blackboard with invalid var0");
	}
}

void Blackboard::stopEditing(bool losingfocus) {
	assert(editing);

	if (!losingfocus && world.focusagent == AgentRef(this)) {
		world.setFocus(0); // this will call us again via loseFocus() on the part
		return;
	}

	editing = false;
}

#include "Bubble.h"
void Blackboard::broadcast(bool audible) {
	// TODO: blackboard broadcasts
	
	if (audible) {
		Bubble::newBubble(this, true, getText());
	}
}

BlackboardPart::BlackboardPart(Blackboard *p, unsigned int _id) : CompoundPart(p, _id, 0, 0, 1) {
	// TODO: think about plane
}

void BlackboardPart::partRender(RenderTarget *renderer, int xoffset, int yoffset) {
	Blackboard *bbd = dynamic_cast<Blackboard *>(parent);
	bbd->renderText(renderer, xoffset, yoffset);
}

void BlackboardPart::gainFocus() {
	Blackboard *bbd = dynamic_cast<Blackboard *>(parent);
	bbd->startEditing();
}

void BlackboardPart::loseFocus() {
	Blackboard *bbd = dynamic_cast<Blackboard *>(parent);
	bbd->stopEditing(true);
}

void BlackboardPart::handleTranslatedChar(unsigned char c) {
	Blackboard *bbd = dynamic_cast<Blackboard *>(parent);

	// strip non-alpha chars
	// TODO: internationalisation?
	if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) return;

	std::string &s = bbd->strings[bbd->editingindex].second;
	if (s.size() < 10) {
		s += (char)c;
		bbd->currenttext = s;
	}
}

void BlackboardPart::handleRawKey(uint8_t c) {
	Blackboard *bbd = dynamic_cast<Blackboard *>(parent);

	switch (c) {
		case OPENC2E_KEY_BACKSPACE:
			if (bbd->currenttext.size() == 0) return;
			{
				std::string &s = bbd->strings[bbd->editingindex].second;
				s.erase(s.begin() + (s.size() - 1));
				bbd->currenttext = s;
			}
			break;

		case OPENC2E_KEY_RETURN:
			bbd->stopEditing(false);
			break;
	}
}

/* vim: set noet: */
