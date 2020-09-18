#pragma once

#include "TextPart.h"

class FixedTextPart : public TextPart {
public:
	FixedTextPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
				  unsigned int _z, std::string fontsprite);
};