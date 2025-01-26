#pragma once

#include "BodyPart.h"

#include <array>
#include <memory>
#include <stdint.h>

namespace sfc {

struct LimbData {
	uint8_t startx;
	uint8_t starty;
	uint8_t endx;
	uint8_t endy;
};

struct LimbV1 : BodyPartV1 {
	std::array<LimbData, 10> limb_data;
	std::shared_ptr<LimbV1> next;

	template <typename Archive>
	void serialize(Archive& ar) {
		BodyPartV1::serialize(ar);
		for (auto& l : limb_data) {
			ar(l.startx);
			ar(l.starty);
			ar(l.endx);
			ar(l.endy);
		}
		ar(next);
	}
};

} // namespace sfc