#pragma once

#include "Bubble.h"
#include "SimpleObject.h"

#include <stdint.h>
#include <string>

namespace sfc {

struct BubbleV1;

struct PointerToolV1 : SimpleObjectV1 {
	int32_t relx;
	int32_t rely;
	BubbleV1* bubble = nullptr;
	std::string text;

	template <typename Archive>
	void serialize(Archive& ar) {
		SimpleObjectV1::serialize(ar);

		ar(relx);
		ar(rely);
		ar(bubble);
		ar.ascii_nullterminated(text, 25);
	}
};

} // namespace sfc