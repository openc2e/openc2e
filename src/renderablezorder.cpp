#include "renderablezorder.h"
#include "renderable.h"
#include "CompoundPart.h"
#include "Engine.h"

bool renderablezorder::operator ()(const renderable *s1, const renderable *s2) const {
	// TODO: There's got to be a less stupid way to do this. - fuzzie
	if (s1->getZOrder() == s2->getZOrder()) {
		const CompoundPart *p1 = dynamic_cast<const CompoundPart *>(s1);
		const CompoundPart *p2 = dynamic_cast<const CompoundPart *>(s2);
		if (p1 && p2)
			return partzorder()(p2, p1);
	}
	// TODO: we shouldn't be checking engine.bmprenderer for this, but it's a cheap/easy way to check for seamonkeys
	if (engine.bmprenderer)
		return (int)s1->getZOrder() < (int)s2->getZOrder();
	else
		return s1->getZOrder() < s2->getZOrder();

}