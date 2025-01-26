#pragma once

#include "SimpleObject.h"

#include <stdint.h>
#include <string>

namespace sfc {

struct ObjectV1;

struct BubbleV1 : SimpleObjectV1 {
	uint8_t life;
	ObjectV1* creator = nullptr;
	std::string text;

	template <typename Archive>
	void serialize(Archive& ar) {
		SimpleObjectV1::serialize(ar);

		ar(life);
		ar(creator); // ???
		ar.ascii_nullterminated(text, 25);
	}
};

} // namespace sfc