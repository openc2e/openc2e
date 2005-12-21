/*
 *  CompoundAgent.cpp
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

#include "CompoundPart.h"
#include "openc2e.h"
#include "c16Image.h"
#include "SDLBackend.h"

void CompoundPart::render(SDLBackend *renderer, int xoffset, int yoffset) {
	renderer->render(getSprite(), getCurrentSprite(), xoffset + x, yoffset + y, is_transparent, transparency);
}

CompoundPart::CompoundPart(unsigned int _id, std::string spritefile, unsigned int fimg,
						int _x, int _y, unsigned int _z) {
	id = _id;
	firstimg = fimg;
	x = _x;
	y = _y;
	zorder = _z;
	sprite = gallery.getImage(spritefile);
	caos_assert(sprite);
	pose = 0;
	base = 0;
	is_transparent = false;
	framerate = 1;
	framedelay = 0;
}

DullPart::DullPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
			 unsigned int _z) : CompoundPart(_id, spritefile, fimg, _x, _y, _z) {
}

ButtonPart::ButtonPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
	unsigned int _z, const bytestring &animhover, int msgid, int option) : CompoundPart(_id, spritefile, fimg, _x, _y, _z) {
	messageid = msgid;
	hitopaquepixelsonly = (option == 1);
	hoveranimation = animhover;
}

FixedTextPart::FixedTextPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		                                  unsigned int _z, std::string fontsprite) : TextPart(_id, spritefile, fimg, _x, _y, _z) {
	// TODO: load fontsprite
}

TextEntryPart::TextEntryPart(unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
		                                  unsigned int _z, unsigned int msgid, std::string fontsprite) : TextPart(_id, spritefile, fimg, _x, _y, _z) {
	// TODO: load fontsprite, msgid
}

void CompoundPart::tick() {
	if (!animation.empty()) {
                if (framerate > 1) {
			framedelay++;
			if (framedelay == framerate + 1)
				framedelay = 0;
		}
		
		if (framedelay == 0) {
			unsigned int f = frameno + 1;
			if (f == animation.size()) return;
			if (animation[f] == 255) {
				if (f == (animation.size() - 1)) f = 0;
				else f = animation[f + 1];
			}
			setFrameNo(f);
		}
	}
}

/* vim: set noet: */

