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

#include <cassert>

#include "Backend.h"
#include "Bubble.h"
#include "Camera.h"
#include "creaturesImage.h"
#include "Engine.h"
#include "imageManager.h"
#include "keycodes.h"
#include "World.h"

Bubble::Bubble(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
		std::string spritefile, unsigned int firstimage, unsigned int imagecount, 
		unsigned int tx, unsigned int ty, unsigned int twidth, unsigned int theight,
		unsigned int tcolour, unsigned int bgcolour)
		: CompoundAgent(family, genus, species, plane, spritefile, firstimage, imagecount) {
	ourPart = new BubblePart(this, 1, tx, ty, tcolour, bgcolour);
	addPart(ourPart);
	ourPart->textwidth = twidth;
	ourPart->textheight = theight;

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

void Bubble::turnIntoSpeech() {
	newBubble((Agent *)world.hand(), true, getText());

	// TODO: announce via shou
	// TODO: add to speech history

	kill();
}

Bubble *Bubble::newBubble(Agent *parent, bool speech, std::string text) {
	assert(engine.version < 3);

	assert(parent);

	bool leftside = false;
	// TODO: cope with wrap
	if (parent->x - engine.camera->getX() < engine.camera->getWidth() / 2) leftside = true;

	int pose;
	if (engine.version == 1) {
		// C1 poses
		if (speech)
			pose = leftside ? 10 : 9;
		else
			pose = leftside ? 12 : 11;
	} else {
		// C2 poses
		// pose adjusted on setText() if necessary
		if (speech)
			pose = leftside ? 21 : 18;
		else
			pose = leftside ? 27 : 24;
	}

	int plane;
	if (engine.version == 1) {
		plane = 9000;
	} else {
		// C2
		if (parent == (Agent *)world.hand()) {
			if (speech)
				plane = 9996;
			else
				plane = 9999;
		} else
			plane = 9995;
	}

	int xoffset = (engine.version == 1) ? 6 : 8;
	int yoffset = (engine.version == 1) ? 3 : 8;
	int twidth = (engine.version == 1) ? 144 : 95; // extended to fit text upon setText()

	unsigned int textcolor = (engine.version == 1) ? 0xb : 0x0821; // TODO
	unsigned int bgcolor = (engine.version == 1) ? 0x1 : 0xFFFF; // TODO

	Bubble *ourBubble = new Bubble(2, 1, 2, plane, "syst", pose, engine.version == 1 ? 1 : 3, xoffset, yoffset, twidth, 12, textcolor, bgcolor);
	ourBubble->finishInit();
	ourBubble->leftside = leftside;

	ourBubble->attr = 32; // floating
	ourBubble->floatTo(parent);

	// TODO: fix positioning
	if (leftside)
		ourBubble->moveTo(parent->x + parent->getWidth() - 2, parent->y - ourBubble->getHeight());
	else
		ourBubble->moveTo(parent->x - ourBubble->getWidth() + 2, parent->y - ourBubble->getHeight());

	ourBubble->setText(text);

	if (speech)
		ourBubble->setTimeout(2 * 10); // TODO: 2s is probably not right
	else
		ourBubble->setEditing(true);

	return ourBubble;
}

BubblePart::BubblePart(Bubble *p, unsigned int _id, int x, int y, unsigned int tcolour, unsigned int bgcolour)
	: CompoundPart(p, _id, x, y, 0)
{
	editable = false;
	textwidth = 0;
	textheight = 0;
	textoffset = 0; // doesn't matter when text is empty
	textcolour = tcolour;
	backgroundcolour = bgcolour;

	if (engine.version == 1) {
		charsetsprite = world.gallery->getCharsetDta(if_paletted, tcolour, bgcolour);
	} else {
		charsetsprite = world.gallery->getCharsetDta(if_16bit_565, tcolour, bgcolour);
	}
}

void BubblePart::partRender(RenderTarget *renderer, int xoffset, int yoffset) {
	unsigned int charpos = 0;
	for (unsigned char c : text) {
		assert(c < charsetsprite->numframes()); // handled in setText
		renderer->render(charsetsprite, c, xoffset + x + textoffset + charpos, yoffset + y);
		charpos += charsetsprite->width(c) + 1;
	}
}

void BubblePart::gainFocus() {
	assert(editable);
}

void BubblePart::loseFocus() {
	parent->kill();
}

void BubblePart::handleTranslatedChar(unsigned char c) {
	setText(text + (char)c);
}

void BubblePart::handleRawKey(uint8_t key) {
	switch (key) {
		case OPENC2E_KEY_BACKSPACE:
			if (text.size() == 0) { loseFocus(); return; }
			{
				std::string s = text;
				s.erase(s.begin() + (s.size() - 1));
				setText(s);
			}
			if (text.size() == 0) { loseFocus(); return; }
			break;

		case OPENC2E_KEY_RETURN:
			((Bubble *)parent)->turnIntoSpeech(); // TODO: omg hax
			break;
	}
}

unsigned int BubblePart::poseForWidth(unsigned int width) {
	if (engine.version == 1) return 0;
	SpritePart *s;
	if (!(s = dynamic_cast<SpritePart *>(parent->part(0)))) return 0;
	unsigned int sprite = s->getFirstImg();
	while (sprite < s->getFirstImg()+2 && width > s->getSprite()->width(sprite) - 17)
		sprite++;
	return sprite - s->getFirstImg();
}

void BubblePart::setText(std::string str) {
	unsigned int twidth = 0;
	std::string newtext;
	for (unsigned char c : str) {
		if (c >= charsetsprite->numframes()) {
			// skip accented characters when we only have CHARSET.DTA
			// TODO: convert to non-accented characters (CP1252->ASCII)
			continue;
		}
		twidth += charsetsprite->width(c) + 1;
		newtext.push_back(c);
	}
	if (twidth > 0) {
		twidth -= 1;
	}

	if (engine.version == 2) {
		unsigned int pose = poseForWidth(twidth);
		SpritePart *s;
		if ((s = dynamic_cast<SpritePart *>(parent->part(0)))) {
			if (pose != s->getPose()) {
				s->setPose(pose);
				textwidth = s->getWidth() - 17;
				Bubble* p = (Bubble*)parent; // TODO: omg hax
				if (!p->leftside)
					p->moveTo(p->floatingagent->x - p->getWidth() + 2, p->floatingagent->y - p->getHeight());
			}
		}
	}
	if (twidth > textwidth) return;

	text = newtext;
	textoffset = (textwidth - twidth) / 2;
}

/* vim: set noet: */
