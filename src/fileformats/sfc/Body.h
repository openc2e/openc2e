#pragma once

#include "BodyPart.h"

#include <array>
#include <stdint.h>

namespace sfc {

struct Vector2i8 {
	int8_t x;
	int8_t y;
};

struct BodyV1 : BodyPartV1 {
	std::array<std::array<Vector2i8, 10>, 6> body_data;

	template <typename Archive>
	void serialize(Archive& ar) {
		BodyPartV1::serialize(ar);

		// stored in column-major order, unlike ATT files which
		// are in row-major order
		for (auto& part : body_data) {
			for (auto& pose : part) {
				ar(pose.x);
				ar(pose.y);
			}
		}
	}
};

} // namespace sfc