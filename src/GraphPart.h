#pragma once

#include "SpritePart.h"

class GraphPart : public SpritePart {
public:
	GraphPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
			  unsigned int _z, unsigned int novalues);
};