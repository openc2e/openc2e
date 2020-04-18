/*
 *  Bubble.h
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

#include "CompoundAgent.h"

#ifndef _C2E_BUBBLE_H
#define _C2E_BUBBLE_H

class Bubble : public CompoundAgent {
	friend class BubblePart;

protected:
	class BubblePart *ourPart;
	unsigned int timeout;
	bool leftside;

	void turnIntoSpeech();

public:
	Bubble(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
		std::string spritefile, unsigned int firstimage, unsigned int imagecount, 
		unsigned int tx, unsigned int ty, unsigned int twidth, unsigned int theight,
		unsigned int bgcolour, unsigned int tcolour);

	void setText(std::string s);
	std::string getText();

	void setEditing(bool e);

	void setTimeout(unsigned int i);
	void tick();

	static Bubble *newBubble(Agent *parent, bool speech, std::string text);
};

class BubblePart : public CompoundPart {
	friend class Bubble;

protected:
	BubblePart(Bubble *p, unsigned int _id, int x, int y);

	bool editable;
	std::string text;
	unsigned int textwidth, textheight;
	unsigned int textoffset;
	unsigned int backgroundcolour, textcolour;

	void setText(std::string s);
	unsigned int poseForWidth(unsigned int width);

public:
	void partRender(class RenderTarget *renderer, int xoffset, int yoffset);
	unsigned int getWidth() { return textwidth; }
	unsigned int getHeight() { return textheight; }

	bool canGainFocus() { return editable; }
	void gainFocus();
	void loseFocus();
	void handleKey(char c);
	void handleSpecialKey(char c);
};

#endif
/* vim: set noet: */
