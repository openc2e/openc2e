/*
 *  SimpleAgent.h
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

#include "Agent.h"
#include <string>
#include <vector>

#ifndef _C2E_SIMPLEAGENT_H
#define _C2E_SIMPLEAGENT_H

class SimpleAgent : public Agent {
protected:
	creaturesImage *sprite;
	unsigned int first_image, image_count;
	unsigned int pose, frameno;

public:
	std::vector<unsigned int> animation;

	SimpleAgent(unsigned char family, unsigned char genus, unsigned short species, unsigned int plane,
							unsigned int firstimage, unsigned int imagecount);
	unsigned int getFirstImage() { return first_image; }
	unsigned int getImageCount() { return image_count; }
	creaturesImage *getSprite() { return sprite; }
	void setImage(std::string img);
	virtual void setAttributes(unsigned int attr);
	virtual unsigned int getAttributes();
	virtual void render(SDLBackend *renderer, int xoffset, int yoffset);
	virtual unsigned int getWidth() { return sprite->width(first_image); }
	virtual unsigned int getHeight() { return sprite->height(first_image); }

	void setFrameNo(unsigned int);
	void setPose(unsigned int p) { pose = p; }
	unsigned int getPose() { return pose; }
	unsigned int getCurrentSprite() { return first_image + pose; }
	unsigned int getFrameNo() { return frameno; }

	virtual void tick();
};

#endif
