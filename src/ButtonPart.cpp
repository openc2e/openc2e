#include "ButtonPart.h"

ButtonPart::ButtonPart(Agent *p, unsigned int _id, std::string spritefile, unsigned int fimg, int _x, int _y,
	unsigned int _z, const bytestring_t &animhover, int msgid, int option) : SpritePart(p, _id, spritefile, fimg, _x, _y, _z) {
	messageid = msgid;
	hitopaquepixelsonly = (option == 1);
	hoveranimation = animhover;
}

unsigned int calculateScriptId(unsigned int message_id); // from caosVM_agent.cpp, TODO: move into shared file

int ButtonPart::handleClick(float, float) {
	return calculateScriptId(messageid);
}

void ButtonPart::mouseIn() {
	// TODO: what if ANIM is called during mouse hover?
	if (hoveranimation.size()) {
		oldanim = animation;
		animation = hoveranimation;
		setFrameNo(0);
	}
}

void ButtonPart::mouseOut() {
	// TODO: what if ANIM is called during mouse hover?
	if (hoveranimation.size()) {
		animation = oldanim;
		if (animation.size())
			setFrameNo(0);
	}
}