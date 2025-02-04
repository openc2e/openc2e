#include "Vehicle.h"

#include "SFCSerialization.h"
#include "fileformats/sfc/Vehicle.h"

#include <fmt/format.h>

void Vehicle::serialize(SFCContext& ctx, sfc::VehicleV1* veh) {
	// we need to make sure CompoundObject::serialize has already been called before the
	// following load logic, since it loads the Entity positions as integer values but then
	// we adjust those based on the vehicle's fixed point 24.8 position.
	CompoundObject::serialize(ctx, veh);

	if (ctx.is_storing()) {
		// TODO: round or truncate velocity? does it matter?
		veh->x_times_256 = numeric_cast<int32_t>(parts[0].x() * 256);
		veh->y_times_256 = numeric_cast<int32_t>(parts[0].y() * 256);
		veh->xvel_times_256 = numeric_cast<int32_t>(xvel * 256);
		veh->yvel_times_256 = numeric_cast<int32_t>(yvel * 256);
		veh->cabin = cabin;
		veh->bump = bump;
	} else {
		xvel = veh->xvel_times_256 / 256.f;
		yvel = veh->yvel_times_256 / 256.f;
		if (parts.empty()) {
			throw Exception("Expected compound parts to be non-null by this point, they should have"
							" been loaded by the CompoundObject serialization logic");
		}
		auto veh_x = veh->x_times_256 / 256.f;
		auto veh_y = veh->y_times_256 / 256.f;
		// as opposed to the official engine, we just store all Part positions as floats.
		for (auto& cp : parts) {
			cp.set_position(veh_x + cp.relx, veh_y + cp.rely);
		}

		cabin = veh->cabin;
		bump = veh->bump;
	}
}