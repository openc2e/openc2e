/*
 *  CameraPart.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon Feb 13 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

#ifndef _CAMERAPART_H
#define _CAMERAPART_H

#include "SpritePart.h"
#include <memory>

class Camera;
class RenderTarget;

class CameraPart : public SpritePart {
protected:
	unsigned int viewheight, viewwidth, cameraheight, camerawidth;
	std::shared_ptr<Camera> camera;
	std::unique_ptr<RenderTarget, void(*)(RenderTarget*)> rendertarget;
	
public:
	CameraPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
			   unsigned int _z, unsigned int viewwidth, unsigned int viewheight,
			   unsigned int camerawidth, unsigned int cameraheight);

	unsigned int cameraWidth() const { return viewwidth; }
	unsigned int cameraHeight() const { return viewheight; }
	std::shared_ptr<Camera> &getCamera() { return camera; }
	void partRender(class RenderTarget *renderer, int xoffset, int yoffset);
	void tick();
};

#endif

/* vim: set noet: */
