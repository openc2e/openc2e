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
	unsigned int firstimg, imagecount;

public:
	creaturesImage *getSprite() { return sprite; }
	unsigned int x, y, zorder, id;
	virtual void render(SDLBackend *renderer, int xoffset, int yoffset);

	bool operator < (const CompoundPart *b) const {
		return zorder < b->zorder;
	}
	
	CompoundPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int imgcnt, unsigned int _x, unsigned int _y,
				 unsigned int _z);
};

class ButtonPart : public CompoundPart {
protected:
	bool hittransparentpixelsonly;

public:
	ButtonPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			   unsigned int _z, std::string animhover, int msgid, int option);
};

class CameraPart : public CompoundPart {
public:
	CameraPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			   unsigned int _z, unsigned int viewwidth, unsigned int viewheight,
			   unsigned int camerawidth, unsigned int cameraheight);
};

class DullPart : public CompoundPart {
public:
	DullPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int imgcnt, unsigned int _x, unsigned int _y,
			 unsigned int _z);
};

class FixedTextPart : public CompoundPart {
public:
	FixedTextPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
				  unsigned int _z, std::string fontsprite);
};

class GraphPart : public CompoundPart {
public:
	GraphPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			  unsigned int _z, unsigned int novalues);
};

class TextEntryPart : public CompoundPart {
public:
	TextEntryPart(unsigned int _id, std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
				  unsigned int _z, unsigned int msgid, std::string fontsprite);
};

class CompoundAgent : public Agent {
protected:
    std::vector<CompoundPart *> parts;

public:
	// TODO: we share all these with SimpleAgent
	bool carryable, mouseable, activateable, invisible, floatable;
	bool suffercollisions, sufferphysics, camerashy, rotatable, presence;
	
	CompoundAgent(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
								std::string spritefile, unsigned int firstimage, unsigned int imagecount);
		
	unsigned int partCount() { return parts.size(); }
	CompoundPart *part(unsigned int id);
	void addPart(CompoundPart *);
	void delPart(unsigned int);
	virtual void setAttributes(unsigned int attr);
	virtual unsigned int getAttributes();
	virtual void tick();

	virtual void render(SDLBackend *renderer, int xoffset, int yoffset);
};
