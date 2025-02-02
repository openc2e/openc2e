#pragma once

#include "DullPart.h"
#include "Object.h"

#include <array>
#include <stdint.h>

namespace sfc {
struct SimpleObjectV1;
}

struct SimpleObject : Object {
	DullPart part;
	int32_t z_order;
	std::array<int8_t, 3> click_bhvr;
	uint8_t touch_bhvr;

	const DullPart* get_part(int32_t) const override;
	void handle_left_click(float, float) override;
	void serialize(SFCContext&, sfc::SimpleObjectV1*);
};
