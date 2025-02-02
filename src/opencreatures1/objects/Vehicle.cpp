#include "Vehicle.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/Vehicle.h"

void Vehicle::serialize(SFCContext& ctx, sfc::VehicleV1* veh) {
	// recurse before the Vehicle logic (unlike the other functions) because
	// we need to adjust the parts' position based on the vehicle's fixed point 24.8 position.
	CompoundObject::serialize(ctx, veh);

	if (ctx.is_storing()) {
		// TODO: round or truncate velocity? does it matter?
		veh->x_times_256 = numeric_cast<int32_t>(parts[0].renderable.get_x() * 256);
		veh->y_times_256 = numeric_cast<int32_t>(parts[0].renderable.get_y() * 256);
		veh->xvel_times_256 = numeric_cast<int32_t>(xvel * 256);
		veh->yvel_times_256 = numeric_cast<int32_t>(yvel * 256);
		veh->cabin = cabin;
		veh->bump = bump;
	} else {
		// need the compound parts to already be defined
		xvel = veh->xvel_times_256 / 256.f;
		yvel = veh->yvel_times_256 / 256.f;
		if (parts.empty()) {
			throw Exception("Expected compound parts to be non-null by this point, they should have"
							" been loaded by the CompoundObject serialization logic");
		}
		auto obj_x = parts[0].renderable.get_x();
		auto obj_y = parts[0].renderable.get_y();
		auto veh_x = veh->x_times_256 / 256.f;
		auto veh_y = veh->y_times_256 / 256.f;
		if (obj_x != veh_x || obj_y != veh_y) {
			fmt::print("INFO [SFCLoader] Object {} {} {} position {}, {} adjusted for VehicleData {}, {}\n", family, genus, species, obj_x, obj_y, veh_x, veh_y);
			auto diff_x = veh_x - obj_x;
			auto diff_y = veh_y - obj_y;
			// All parts need to be adjusted, not just the main one. If only the main one
			// changes, things start to get weird — look at the cable car buttons after
			// multiple load/save cycles whilst activating the buttons. They seem to "shimmy"
			// up and down (classic floating point issue) and eventually drift after enough
			// serialization cycles.
			for (auto& cp : parts) {
				cp.renderable.set_position(cp.renderable.get_x() + diff_x, cp.renderable.get_y() + diff_y);
			}
		}
		cabin = veh->cabin;
		bump = veh->bump;
	}
}