#pragma once

#include "CompoundObject.h"
#include "WinRect.h"

#include <stdint.h>

namespace sfc {

struct VehicleV1 : CompoundObjectV1 {
	int32_t xvel_times_256;
	int32_t yvel_times_256;
	int32_t x_times_256;
	int32_t y_times_256;
	WinRect cabin;
	uint32_t bump;

	template <typename Archive>
	void serialize(Archive& ar) {
		CompoundObjectV1::serialize(ar);
		ar(xvel_times_256);
		ar(yvel_times_256);
		ar(x_times_256);
		ar(y_times_256);
		ar(cabin.left);
		ar(cabin.top);
		ar(cabin.right);
		ar(cabin.bottom);
		ar(bump);
	}
};

} // namespace sfc