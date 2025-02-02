#include "Scenery.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/Scenery.h"

void Scenery::serialize(SFCContext& ctx, sfc::SceneryV1* scen) {
	if (ctx.is_storing()) {
		scen->part = sfc_dump_renderable(part);
		static_cast<sfc::ObjectV1*>(scen)->gallery = scen->part->gallery;
	} else {
		part = sfc_load_renderable(scen->part.get());
	}
	Object::serialize(ctx, scen);
}