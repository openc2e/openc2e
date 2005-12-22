/*
 *  CompoundAgent.h
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

#ifndef _COMPOUNDPART_H
#define _COMPOUNDPART_H

#include "openc2e.h"
#include "creaturesImage.h"
#include <map>
#include <string>
#include <vector>

class CompoundPart {
protected:
	creaturesImage *sprite;
	unsigned int firstimg, pose, frameno, base;
	CompoundPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
				 unsigned int _z);

public:
	std::vector<unsigned int> animation;
	bool is_transparent;
	unsigned char framerate;
	unsigned int framedelay;
	unsigned char transparency;
	creaturesImage *getSprite() { return sprite; }
	int x, y;
	unsigned int zorder, id;
	virtual void render(class SDLBackend *renderer, int xoffset, int yoffset);
	virtual void tick();
	unsigned int getWidth() { return sprite->width(firstimg); }
	unsigned int getHeight() { return sprite->height(firstimg); }
	unsigned int getPose() { return pose; }
	unsigned int getBase() { return base; }
	unsigned int getCurrentSprite() { return firstimg + base + pose; }
	unsigned int getFrameNo() { return frameno; }
	unsigned int getFirstImg() { return firstimg; }
	void setFrameNo(unsigned int f) { frameno = f; pose = animation[f]; } // todo: assert it's in the range
	void setPose(unsigned int p) { animation.clear(); pose = p; }
	void setFramerate(unsigned char f) { framerate = f; framedelay = 0; }
	void setBase(unsigned int b) { base = b; }
				
	bool operator < (const CompoundPart &b) const {
		return zorder < b.zorder;
	}
	
	virtual ~CompoundPart() { }
};

class ButtonPart : public CompoundPart {
protected:
	bool hitopaquepixelsonly;
	int messageid;
	bytestring hoveranimation;

	friend class CompoundAgent;

public:
	ButtonPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
			   unsigned int _z, const bytestring &animhover, int msgid, int option);
};

class CameraPart : public CompoundPart {
public:
	CameraPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
			   unsigned int _z, unsigned int viewwidth, unsigned int viewheight,
			   unsigned int camerawidth, unsigned int cameraheight);
};

class DullPart : public CompoundPart {
public:
	DullPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y, unsigned int _z);
};

struct linedata {
	std::string text;
	unsigned int width;

	void reset() { text = ""; width = 0; }
	linedata() { reset(); }
};

class TextPart : public CompoundPart {
protected:
	std::vector<linedata> lines;
	std::vector<unsigned int> pages;
	unsigned int currpage;
	std::string text;
	creaturesImage *textsprite;
	int leftmargin, topmargin, rightmargin, bottommargin;
	int linespacing, charspacing;
	bool left_align, center_align, bottom_align, middle_align, last_page_scroll;
	TextPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y, unsigned int _z, std::string fontsprite);
	void recalculateData();
	unsigned int calculateWordWidth(std::string word);

public:
	void setText(std::string t);
	std::string getText() { return text; }
	unsigned int noPages() { return pages.size(); }
	void setPage(unsigned int p) { currpage = p; }
	unsigned int getPage() { return currpage; }
	void render(class SDLBackend *renderer, int xoffset, int yoffset, class TextEntryPart *caretdata);
	void render(class SDLBackend *renderer, int xoffset, int yoffset) { render(renderer, xoffset, yoffset, 0); }
	void setFormat(int left, int top, int right, int bottom, int line, int _char, bool lefta, bool centera, bool bottoma, bool middlea, bool lastpagescroll);
};

class FixedTextPart : public TextPart {
public:
	FixedTextPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
				  unsigned int _z, std::string fontsprite);
};

class GraphPart : public CompoundPart {
public:
	GraphPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
			  unsigned int _z, unsigned int novalues);
};

class TextEntryPart : public TextPart {
private:
	static creaturesImage *caretsprite;
	unsigned int caretpose;
	bool focused;
	unsigned int caretline, caretchar;
	void renderCaret(class SDLBackend *renderer, int xoffset, int yoffset);

	friend class TextPart;

public:
	TextEntryPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
				  unsigned int _z, unsigned int msgid, std::string fontsprite);
	void gainFocus() { focused = true; caretpose = 0; }
	void loseFocus() { focused = false; }
	void tick();
	virtual void render(class SDLBackend *renderer, int xoffset, int yoffset);
};

#endif

/* vim: set noet: */
