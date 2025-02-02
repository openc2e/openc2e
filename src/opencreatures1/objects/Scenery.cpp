#include "Scenery.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/Scenery.h"

const DullPart* Scenery::get_part(int32_t partnum) const {
	if (partnum == 0) {
		return &part;
	}
	return nullptr;
}

void Scenery::serialize(SFCContext& ctx, sfc::SceneryV1* scen) {
	if (ctx.is_storing()) {
		scen->part = sfc_dump_entity(part);
		static_cast<sfc::ObjectV1*>(scen)->gallery = scen->part->gallery;
	} else {
		part = sfc_load_entity(scen->part.get());
	}
	Object::serialize(ctx, scen);
}