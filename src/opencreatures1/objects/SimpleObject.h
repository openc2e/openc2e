#pragma once

#include "Object.h"
#include "Renderable.h"

#include <array>
#include <stdint.h>

struct SimpleObject : Object {
	Renderable part;
	int32_t z_order;
	std::array<int8_t, 3> click_bhvr;
	uint8_t touch_bhvr;
};
