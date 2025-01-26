#pragma once

#include "Vehicle.h"

#include <array>
#include <stdint.h>

namespace sfc {

struct CallButtonV1;

struct LiftV1 : VehicleV1 {
	struct LiftFloor {
		int32_t y;
		CallButtonV1* call_button = nullptr;
	};

	int32_t num_floors = 0;
	int32_t next_or_current_floor = 0;
	int32_t current_call_button = -1;
	uint8_t delay_ticks_divided_by_32 = 0;
	std::array<int32_t, 8> floors{};
	std::array<CallButtonV1*, 8> activated_call_buttons{};

	template <typename Archive>
	void serialize(Archive& ar) {
		VehicleV1::serialize(ar);
		ar(num_floors);
		ar(next_or_current_floor);
		ar(current_call_button);
		ar(delay_ticks_divided_by_32);
		for (size_t i = 0; i < 8; ++i) {
			ar(floors[i]);
			ar(activated_call_buttons[i]);
		}
	}
};

} // namespace sfc