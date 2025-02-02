#include "Bubble.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/Bubble.h"
void Bubble::serialize(SFCContext& ctx, sfc::BubbleV1* bub) {
	if (ctx.is_storing()) {
	} else {
		fmt::print("WARN [SFCLoader] Object {} {} {} unsupported type: Bubble\n", family, genus, species);
	}
	SimpleObject::serialize(ctx, bub);
}