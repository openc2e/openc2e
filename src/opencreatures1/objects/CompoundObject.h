#pragma once

#include "DullPart.h"
#include "Object.h"
#include "common/math/Rect.h"

#include <array>
#include <stdint.h>
#include <vector>

namespace sfc {
struct CompoundObjectV1;
};

struct CompoundPart : DullPart {
	int32_t relx;
	int32_t rely;
};

enum HotspotFunction {
	HOTSPOT_CREATUREACTIVATE1,
	HOTSPOT_CREATUREACTIVATE2,
	HOTSPOT_CREATUREDEACTIVATE,
	HOTSPOT_MOUSEACTIVATE1,
	HOTSPOT_MOUSEACTIVATE2,
	HOTSPOT_MOUSEDEACTIVATE,
};

struct CompoundObject : Object {
	std::vector<CompoundPart> parts;
	std::array<Rect2i, 6> hotspots;
	std::array<int32_t, 6> functions_to_hotspots;

	void serialize(SFCContext&, sfc::CompoundObjectV1*);
};
