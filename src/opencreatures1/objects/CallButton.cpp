#include "CallButton.h"

#include "ObjectManager.h"
#include "SFCSerialization.h"
#include "fileformats/sfc/CallButton.h"
#include "fileformats/sfc/Lift.h"

void CallButton::serialize(SFCContext& ctx, sfc::CallButtonV1* cbtn) {
	if (ctx.is_storing()) {
		// CallButtonV1
		cbtn->lift = dynamic_cast<sfc::LiftV1*>(
			ctx.dump_object(g_engine_context.objects->try_get(lift)).get());
		cbtn->floor = floor;
	} else {
		lift = ctx.load_object(cbtn->lift);
		floor = cbtn->floor;
	}
	SimpleObject::serialize(ctx, cbtn);
}