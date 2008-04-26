/*
 *  Bubble.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Apr 26 2008.
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

#include "Bubble.h"
#include "World.h"
#include "Engine.h"
#include "Backend.h"

// class BubblePart *ourPart;

Bubble::Bubble(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
		std::string spritefile, unsigned int firstimage, unsigned int imagecount, 
		unsigned int tx, unsigned int ty, unsigned int twidth, unsigned int theight,
		unsigned int bgcolour, unsigned int tcolour)
		: CompoundAgent(family, genus, species, plane, spritefile, firstimage, imagecount) {
	ourPart = new BubblePart(this, 1, tx, ty);
	addPart(ourPart);
	ourPart->textwidth = twidth;
	ourPart->textheight = theight;
	ourPart->backgroundcolour = bgcolour;
	ourPart->textcolour = tcolour;

	ourPart->editable = false;
}

void Bubble::setText(std::string s) {
	ourPart->setText(s);
}

std::string Bubble::getText() {
	return ourPart->text;
}

void Bubble::setEditing(bool e) {
	ourPart->editable = e;
	if (e)
		world.setFocus(ourPart); // gain focus
	else if (world.focusagent == AgentRef(this) && world.focuspart == ourPart->id)
		world.setFocus(0); // lose focus
}

void Bubble::setTimeout(unsigned int t) {
	timeout = t;
}

void Bubble::tick() {
	CompoundAgent::tick();

	if (!paused && timeout) {
		timeout--;
		if (timeout == 0) kill();
	}
}

#include "PointerAgent.h"
void Bubble::turnIntoSpeech() {
	// TODO: this should really really really be handled elsewhere, not in Bubble!!!!

	// TODO: C2 support
	assert(engine.version == 1);

	bool leftside = false;
	// TODO: cope with wrap
	if (world.hand()->x - world.camera.getX() < world.camera.getWidth() / 2) leftside = true;

	// TODO: are 1/0 good colours?
	Bubble *ourSpeechBubble = new Bubble(2, 1, 2, 9000, "syst", leftside ? 10 : 9, 1, 6, 3, 144, 12, 1, 0);
	ourSpeechBubble->finishInit();

	ourSpeechBubble->attr = 32; // floating
	ourSpeechBubble->floatTo(world.hand());

	// TODO: fix positioning
	if (leftside)
		ourSpeechBubble->moveTo(world.hand()->x + world.hand()->getWidth() - 2, world.hand()->y - getHeight());
	else
		ourSpeechBubble->moveTo(world.hand()->x - getWidth() + 2, world.hand()->y - getHeight());

	ourSpeechBubble->setText(getText());
	
	ourSpeechBubble->setTimeout(2 * 10); // TODO: 2s is probably not right
	// TODO: announce via shou
	// TODO: add to speech history

	kill();
}

/*
  class BubblePart : public CompoundPart {
	bool editable;
	std::string text;
	unsigned int textwidth, textheight;
	unsigned int backgroundcolour, textcolour;
*/

BubblePart::BubblePart(Bubble *p, unsigned int _id, int x, int y) : CompoundPart(p, _id, x, y, 1) {
	editable = false;
	textwidth = 0;
	textheight = 0;
	textoffset = 0; // doesn't matter when text is empty
}

void BubblePart::partRender(class Surface *renderer, int xoffset, int yoffset) {
	renderer->renderText(xoffset + x + textoffset, yoffset + y, text, textcolour, backgroundcolour);
}

void BubblePart::gainFocus() {
	assert(editable);
}

void BubblePart::loseFocus() {
	parent->kill();
}

void BubblePart::handleKey(char c) {
	// TODO: reject invalid chars

	setText(text + c);
}

void BubblePart::handleSpecialKey(char c) {
	switch (c) {
		case 8: // backspace
			if (text.size() == 0) { loseFocus(); return; }
			{ std::string s = text;
			s.erase(s.begin() + (s.size() - 1));
			setText(s); }
			if (text.size() == 0) { loseFocus(); return; }
			break;

		case 13: // return
			((Bubble *)parent)->turnIntoSpeech(); // TODO: omg hax
			break;
	}
}

void BubblePart::setText(std::string s) {
	unsigned int twidth = engine.backend->textWidth(s);
	if (twidth > textwidth) return;

	text = s;
	textoffset = (textwidth - twidth) / 2;
}

/* vim: set noet: */
