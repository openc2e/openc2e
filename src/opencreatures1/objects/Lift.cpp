#include "Lift.h"

#include "CallButton.h"
#include "ObjectManager.h"
#include "SFCSerialization.h"
#include "common/Ranges.h"
#include "fileformats/sfc/CallButton.h"
#include "fileformats/sfc/Lift.h"

void Lift::serialize(SFCContext& ctx, sfc::LiftV1* lift) {
	if (ctx.is_storing()) {
		lift->num_floors = numeric_cast<int32_t>(floors.size());
		lift->next_or_current_floor = next_or_current_floor;

		// Would like to do this based on index w/in the serialized array, but
		// we're not guaranteed to have the CallButtons serialized by this point...
		// Since CallButtons have a pointer to their parent Lift, we might start
		// serializing a CallButton and immediately switch to serializing the Lift,
		// without fleshing out the CallButton struct until the Lift is finished.
		lift->current_call_button = index_if(activated_call_buttons, [&](auto& handle) {
			auto* cb = g_engine_context.objects->try_get(handle);
			return cb && cb->as_call_button()->floor == lift->next_or_current_floor;
		});

		lift->delay_ticks_divided_by_32 = 0; // TODO
		for (size_t i = 0; i < floors.size(); ++i) {
			lift->floors[i] = floors[i];
		}
		for (auto it : enumerate(activated_call_buttons)) {
			lift->activated_call_buttons[it.first] = dynamic_cast<sfc::CallButtonV1*>(
				ctx.dump_object(g_engine_context.objects->try_get(it.second)).get());
		}
	} else {
		next_or_current_floor = lift->next_or_current_floor;
		// unneeded
		// current_call_button = lift->current_call_button;
		// TODO
		// delay_ticks_divided_by_32 = lift->delay_ticks_divided_by_32;
		if (lift->delay_ticks_divided_by_32 != 0) {
			fmt::print("WARN [SFCLoader] Unsupported: LiftData.delay_ticks_divided_by_32 = {}\n", lift->delay_ticks_divided_by_32);
		}
		for (size_t i = 0; i < numeric_cast<size_t>(lift->num_floors); ++i) {
			// printf("lift->floors[i] y %i call_button %p\n", lift->floors[i].y, lift->floors[i].call_button);
			floors.push_back(lift->floors[i]);
		}
		for (auto* cb : lift->activated_call_buttons) {
			if (auto&& handle = ctx.load_object(cb)) {
				activated_call_buttons.insert(handle);
			}
		}
	}
	Vehicle::serialize(ctx, lift);
}