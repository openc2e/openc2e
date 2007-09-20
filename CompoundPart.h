/*
 *  CompoundPart.h
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
#include "renderable.h"
#include <map>
#include <string>
#include <vector>

class Agent;

struct partzorder {
	bool operator()(const class CompoundPart *s1, const class CompoundPart *s2) const;
};

class CompoundPart : public renderable {
protected:
	std::multiset<CompoundPart *, partzorder>::iterator zorder_iter;
	Agent *parent;

	CompoundPart(Agent *p, unsigned int _id, int _x, int _y, int _z);

public:
	int x, y;
	unsigned int zorder, id;

	bool has_alpha;
	unsigned char alpha;

	virtual void render(class Surface *renderer, int xoffset, int yoffset);
	virtual void partRender(class Surface *renderer, int xoffset, int yoffset) = 0;
	virtual void tick() { }
	virtual void handleClick(float, float);
	virtual unsigned int getWidth() = 0;
	virtual unsigned int getHeight() = 0;

	virtual bool showOnRemoteCameras();
	
	Agent *getParent() const { return parent; }
	unsigned int getZOrder() const;
	void zapZOrder();
	void addZOrder();

	bool operator < (const CompoundPart &b) const {
		return zorder < b.zorder;
	}

	virtual ~CompoundPart();
};

class SpritePart : public CompoundPart {
protected:
	shared_ptr<creaturesImage> origsprite, sprite;
	unsigned int firstimg, pose, frameno, base, spriteno;
	SpritePart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
				 unsigned int _z);

public:
	bytestring_t animation;
	bool is_transparent;
	bool draw_mirrored;
	unsigned char framerate;
	unsigned int framedelay;
	shared_ptr<creaturesImage> getSprite() { return sprite; }
	virtual void partRender(class Surface *renderer, int xoffset, int yoffset);
	virtual void tick();
	unsigned int getPose() { return pose; }
	unsigned int getBase() { return base; }
	unsigned int getCurrentSprite() { return spriteno; }
	unsigned int getFrameNo() { return frameno; }
	unsigned int getFirstImg() { return firstimg; }
	unsigned int getWidth() { return sprite->width(getCurrentSprite()); }
	unsigned int getHeight() { return sprite->height(getCurrentSprite()); }
	void setFrameNo(unsigned int f);
	void setPose(unsigned int p);
	void setFramerate(unsigned char f) { framerate = f; framedelay = 0; }
	void setBase(unsigned int b);
	void changeSprite(std::string spritefile, unsigned int fimg);
	void tint(unsigned char r, unsigned char g, unsigned char b, unsigned char rotation, unsigned char swap);
	virtual bool isTransparent() { return is_transparent; }
	bool transparentAt(unsigned int x, unsigned int y);

	virtual ~SpritePart();
};

class ButtonPart : public SpritePart {
protected:
	bool hitopaquepixelsonly;
	int messageid;
	bytestring_t hoveranimation;

public:
	ButtonPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
			   unsigned int _z, const bytestring_t &animhover, int msgid, int option);
	void handleClick(float, float);
	bool isTransparent() { return hitopaquepixelsonly; }
};

class DullPart : public SpritePart {
public:
	DullPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y, unsigned int _z);
};

struct linedata {
	std::string text;
	unsigned int offset, width;

	void reset(unsigned int o) { offset = o; text = ""; width = 0; }
	linedata() { reset(0); }
};

struct texttintinfo {
	shared_ptr<creaturesImage> sprite;
	unsigned int offset;
};

enum horizontalalign { leftalign, centeralign, rightalign };
enum verticalalign { top, middle, bottom };

class TextPart : public SpritePart {
protected:
	std::vector<texttintinfo> tints;
	std::vector<linedata> lines;
	std::vector<unsigned int> pages;
	std::vector<unsigned int> pageheights;
	unsigned int currpage;
	std::string text;
	
	shared_ptr<creaturesImage> textsprite;
	
	int leftmargin, topmargin, rightmargin, bottommargin;
	int linespacing, charspacing;
	horizontalalign horz_align;
	verticalalign vert_align;	
	bool last_page_scroll;

	TextPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y, unsigned int _z, std::string fontsprite);
	~TextPart();
	void recalculateData();
	unsigned int calculateWordWidth(std::string word);
	void addTint(std::string tintinfo);

public:
	virtual void setText(std::string t);
	std::string getText() { return text; }
	unsigned int noPages() { return pages.size(); }
	void setPage(unsigned int p) { currpage = p; }
	unsigned int getPage() { return currpage; }
	void partRender(class Surface *renderer, int xoffset, int yoffset, class TextEntryPart *caretdata);
	void partRender(class Surface *renderer, int xoffset, int yoffset) { partRender(renderer, xoffset, yoffset, 0); }
	void setFormat(int left, int top, int right, int bottom, int line, int _char, horizontalalign horza, verticalalign verta, bool lastpagescroll);
};

class FixedTextPart : public TextPart {
public:
	FixedTextPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
				  unsigned int _z, std::string fontsprite);
};

class GraphPart : public SpritePart {
public:
	GraphPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
			  unsigned int _z, unsigned int novalues);
};

class TextEntryPart : public TextPart {
private:
	static shared_ptr<creaturesImage> caretsprite;
	unsigned int caretpose;
	bool focused;
	unsigned int caretpos;
	unsigned int messageid;
	void renderCaret(class Surface *renderer, int xoffset, int yoffset);

	friend class TextPart;

public:
	TextEntryPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
				  unsigned int _z, unsigned int msgid, std::string fontsprite);
	void setText(std::string t);
	void gainFocus() { focused = true; caretpose = 0; }
	void loseFocus() { focused = false; }
	void handleClick(float, float);
	void handleKey(char c);
	void handleSpecialKey(char c);
	void tick();
	virtual void partRender(class Surface *renderer, int xoffset, int yoffset);
	bool isTransparent() { return false; }
};

#endif

/* vim: set noet: */
