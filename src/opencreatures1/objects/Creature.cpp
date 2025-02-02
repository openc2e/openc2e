#include "Creature.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/Creature.h"

const DullPart* Creature::get_part(int32_t) const {
	// TODO
	return nullptr;
}

void Creature::handle_left_click(float, float) {
	// When an object is the subject of a left click event, we queue up a message
	// to ACTIVATE1, ACTIVATE2, or DEACTIVATE. But how do we know which message
	// to send?

	throw Exception("handle_left_click not implemented for Creature");
}

void Creature::serialize(SFCContext& ctx, sfc::CreatureV1* crea) {
	if (ctx.is_storing()) {
	} else {
		fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type Creature\n", family, genus, species);
	}
	Object::serialize(ctx, crea);
}