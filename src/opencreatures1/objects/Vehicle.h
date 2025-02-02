#pragma once

#include "CompoundObject.h"
#include "common/math/Rect.h"

namespace sfc {
struct VehicleV1;
}

struct Vehicle : CompoundObject {
	float xvel;
	float yvel;
	Rect2i cabin;
	uint32_t bump;
	void serialize(SFCContext&, sfc::VehicleV1*);
};