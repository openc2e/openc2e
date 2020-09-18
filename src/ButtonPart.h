#pragma once

#include "SpritePart.h"

class ButtonPart : public SpritePart {
protected:
	bool hitopaquepixelsonly;
	int messageid;
	bytestring_t hoveranimation;
	bytestring_t oldanim;

public:
	ButtonPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
			   unsigned int _z, const bytestring_t &animhover, int msgid, int option);
	int handleClick(float, float);
	bool isTransparent() { return hitopaquepixelsonly; }
	void mouseIn();
	void mouseOut();

	bool canClick() { return true; }
};