#pragma once

#include "Entity.h"

#include <stdint.h>

namespace sfc {

struct BodyPartV1 : EntityV1 {
	int32_t angle;
	int32_t view;

	template <typename Archive>
	void serialize(Archive& ar) {
		EntityV1::serialize(ar);
		ar(angle);
		ar(view);
	}
};

} // namespace sfc