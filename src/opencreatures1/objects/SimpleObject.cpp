#include "SimpleObject.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/SimpleObject.h"

void SimpleObject::serialize(SFCContext& ctx, sfc::SimpleObjectV1* simp) {
	if (ctx.is_storing()) {
		simp->part = sfc_dump_renderable(part);
		static_cast<sfc::ObjectV1*>(simp)->gallery = simp->part->gallery;
		simp->z_order = z_order;
		simp->click_bhvr = click_bhvr;
		simp->touch_bhvr = touch_bhvr;
	} else {
		part = sfc_load_renderable(simp->part.get());
		z_order = simp->z_order;
		click_bhvr = simp->click_bhvr;
		touch_bhvr = simp->touch_bhvr;
	}
	Object::serialize(ctx, simp);
}