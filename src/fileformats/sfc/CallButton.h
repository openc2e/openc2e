#pragma once

#include "SimpleObject.h"

#include <stdint.h>

namespace sfc {

struct LiftV1;

struct CallButtonV1 : SimpleObjectV1 {
	LiftV1* lift = nullptr;
	uint8_t floor;

	template <typename Archive>
	void serialize(Archive& ar) {
		SimpleObjectV1::serialize(ar);
		ar(lift);
		ar(floor);
	}
};

} // namespace sfc