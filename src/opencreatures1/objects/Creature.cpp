#include "Creature.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/Creature.h"

const DullPart* Creature::get_part(int32_t) const {
	// TODO
	return nullptr;
}

void Creature::serialize(SFCContext& ctx, sfc::CreatureV1* crea) {
	if (ctx.is_storing()) {
	} else {
		fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type Creature\n", family, genus, species);
	}
	Object::serialize(ctx, crea);
}