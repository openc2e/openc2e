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

#include "SimpleAgent.h"
#include "creaturesImage.h"
#include <map>
#include <string>

class CompoundPart {
protected:
	creaturesImage *sprite;
	unsigned int firstimg, pose, frameno;
	CompoundPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
				 unsigned int _z);

public:
	std::vector<unsigned int> animation;
	bool is_transparent;
	unsigned char framerate;
	unsigned int framedelay;
	unsigned char transparency;
	creaturesImage *getSprite() { return sprite; }
	unsigned int x, y, zorder, id;
	virtual void render(SDLBackend *renderer, int xoffset, int yoffset);
	virtual void tick();
	unsigned int getWidth() { return sprite->width(firstimg); }
	unsigned int getHeight() { return sprite->height(firstimg); }
	unsigned int getPose() { return pose; }
	unsigned int getCurrentSprite() { return firstimg + pose; }
	unsigned int getFrameNo() { return frameno; }
	void setFrameNo(unsigned int f) { frameno = f; pose = animation[f]; } // todo: assert it's in the range
	void setPose(unsigned int p) { animation.clear(); pose = p; }
	void setFramerate(unsigned char f) { framerate = f; framedelay = 0; }
				
	bool operator < (const CompoundPart *b) const {
		return zorder < b->zorder;
	}
	
	virtual ~CompoundPart() { }
};

class ButtonPart : public CompoundPart {
protected:
	bool hittransparentpixelsonly;

public:
	ButtonPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			   unsigned int _z, const bytestring &animhover, int msgid, int option);
};

class CameraPart : public CompoundPart {
public:
	CameraPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			   unsigned int _z, unsigned int viewwidth, unsigned int viewheight,
			   unsigned int camerawidth, unsigned int cameraheight);
};

class DullPart : public CompoundPart {
public:
	DullPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y, unsigned int _z);
};

class TextPart : public CompoundPart {
protected:
	std::string text;
	TextPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y, unsigned int _z)
		: CompoundPart(_id, spritefile, fimg, _x, _y, _z) { }

public:
	void setText(std::string t) { text = t; }
	std::string getText() { return text; }
};

class FixedTextPart : public TextPart {
public:
	FixedTextPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
				  unsigned int _z, std::string fontsprite);
};

class GraphPart : public CompoundPart {
public:
	GraphPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			  unsigned int _z, unsigned int novalues);
};

class TextEntryPart : public TextPart {
public:
	TextEntryPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
				  unsigned int _z, unsigned int msgid, std::string fontsprite);
};

class CompoundAgent : public Agent {
protected:
	std::vector<CompoundPart *> parts;

	unsigned int width, height;

public:
	CompoundAgent(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
								std::string spritefile, unsigned int firstimage, unsigned int imagecount);
	virtual ~CompoundAgent();
		
	unsigned int partCount() { return parts.size(); }
	CompoundPart *part(unsigned int id);
	void addPart(CompoundPart *);
	void delPart(unsigned int);
	virtual void setAttributes(unsigned int attr);
	virtual unsigned int getAttributes();
	virtual void tick();
	virtual unsigned int getWidth() { return width; }
	virtual unsigned int getHeight() { return height; }

	virtual void render(SDLBackend *renderer, int xoffset, int yoffset);
};
/* vim: set noet: */
