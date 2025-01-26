#pragma once

#include "fileformats/MFCObject.h"

#include <array>
#include <stdint.h>

namespace sfc {

struct InstinctDendriteV1 {
	// not CArchive serialized
	uint32_t lobe;
	uint32_t cell;
};

struct CInstinctV1 : MFCObject {
	std::array<InstinctDendriteV1, 3> dendrites;
	uint32_t motor_decision;
	uint32_t reinforcement_chemical;
	uint32_t reinforcement_amount;
	uint32_t phase;

	template <typename Archive>
	void serialize(Archive& ar) {
		for (auto& d : dendrites) {
			ar(d.lobe);
			ar(d.cell);
		}
		ar(motor_decision);
		ar(reinforcement_chemical);
		ar(reinforcement_amount);
		ar(phase);
	}
};

} // namespace sfc