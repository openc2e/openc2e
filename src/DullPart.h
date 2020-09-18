#pragma once

#include "SpritePart.h"

class DullPart : public SpritePart {
public:
	DullPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y, unsigned int _z);
};