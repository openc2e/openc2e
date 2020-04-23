#include "partzorder.h"
#include "Agent.h"
#include "CompoundPart.h"
#include "Engine.h"

bool partzorder::operator ()(const CompoundPart *s1, const CompoundPart *s2) const {
	// TODO: unsure about all of this, needs a check (but seems to work)
	if (s1->getParent()->getZOrder() == s2->getParent()->getZOrder()) {
		// part 0 is often at the same plane as other parts..
		// TODO: is this correct fix? I suspect not, we should be sorting by reaction order, not id.
		if (s1->getParent() == s2->getParent()) {
			return s1->id > s2->id;
		} else
			return s1->getZOrder() > s2->getZOrder();
	}
	// TODO: we shouldn't be checking engine.bmprenderer for this, but it's a cheap/easy way to check for seamonkeys
	if (engine.bmprenderer)
		return (int)s1->getParent()->getZOrder() > (int)s2->getParent()->getZOrder();
	else
		return s1->getParent()->getZOrder() > s2->getParent()->getZOrder();
}