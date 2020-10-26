#pragma once

#include "bytestring.h"
#include "CompoundPart.h"

class AnimatablePart : public CompoundPart {
protected:
	unsigned int frameno;
	AnimatablePart(Agent *p, unsigned int _id, int _x, int _y, int _z) : CompoundPart(p, _id, _x, _y, _z) { frameno = 0; }

	void updateAnimation();

public:
	bytestring_t animation;
	virtual void setPose(unsigned int p) = 0;
	virtual void setFrameNo(unsigned int f) = 0;
	unsigned int getFrameNo() { return frameno; }
};