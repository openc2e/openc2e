#pragma once

#include "CompoundObject.h"

#include <array>
#include <stdint.h>
#include <string>

namespace sfc {

struct BlackboardV1 : CompoundObjectV1 {
	struct BlackboardWord {
		// not CArchive serialized
		uint32_t value;
		std::string text;
	};

	uint8_t background_color;
	uint8_t chalk_color;
	uint8_t alias_color;
	int8_t text_x_position;
	int8_t text_y_position;
	std::array<BlackboardWord, 16> words;

	template <typename Archive>
	void serialize(Archive& ar) {
		CompoundObjectV1::serialize(ar);
		ar(background_color);
		ar(chalk_color);
		ar(alias_color);
		ar(text_x_position);
		ar(text_y_position);
		for (auto& w : words) {
			ar(w.value);
			ar.ascii_nullterminated(w.text, 11);
		}
	}
};

} // namespace sfc