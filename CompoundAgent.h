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
	unsigned int firstimg;

public:
	creaturesImage *getSprite() { return sprite; }
	unsigned int x, y, zorder;
	
	CompoundPart(std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
				 unsigned int _z);
};

class ButtonPart : public CompoundPart {
protected:
	bool hittransparentpixelsonly;

public:
	ButtonPart(std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			   unsigned int _z, std::string animhover, int msgid, int option);
};

class CameraPart : public CompoundPart {
public:
	CameraPart(std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			   unsigned int _z, unsigned int viewwidth, unsigned int viewheight,
			   unsigned int camerawidth, unsigned int cameraheight);
};

class DullPart : public CompoundPart {
public:
	DullPart(std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			 unsigned int _z);
};

class FixedTextPart : public CompoundPart {
public:
	FixedTextPart(std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
				  unsigned int _z, std::string fontsprite);
};

class GraphPart : public CompoundPart {
public:
	GraphPart(std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
			  unsigned int _z, unsigned int novalues);
};

class TextEntryPart : public CompoundPart {
public:
	TextEntryPart(std::string spritefile, unsigned int fimg, unsigned int _x, unsigned int _y,
				  unsigned int _z, unsigned int msgid, std::string fontsprite);
};

class CompoundAgent : public SimpleAgent {
protected:
	std::map<unsigned int, CompoundPart *> parts;

public:
	CompoundAgent(unsigned int family, unsigned int genus, unsigned int species, unsigned int plane,
								unsigned int firstimage, unsigned int imagecount) :
		SimpleAgent(family, genus, species, plane, firstimage, imagecount) { }
		
	unsigned int partCount() { return parts.size(); }
	CompoundPart &part(int id) { return *(parts[id]); }
	void addPart(int, CompoundPart *);
	void delPart(int);
};

