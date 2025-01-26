#pragma once

#include "Entity.h"
#include "Object.h"

#include <array>
#include <stdint.h>

namespace sfc {

struct SimpleObjectV1 : ObjectV1 {
	std::shared_ptr<EntityV1> part;
	int32_t z_order;
	std::array<int8_t, 3> click_bhvr;
	uint8_t touch_bhvr;

	template <typename Archive>
	void serialize(Archive& ar) {
		ObjectV1::serialize(ar);

		ar(part);
		ar(z_order);
		ar(click_bhvr[0]);
		ar(click_bhvr[1]);
		ar(click_bhvr[2]);
		ar(touch_bhvr);
	}
};

} // namespace sfc