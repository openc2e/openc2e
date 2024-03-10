#pragma once

#include "CompoundObject.h"
#include "common/math/Rect.h"

struct Vehicle : CompoundObject {
	float xvel;
	float yvel;
	Rect2i cabin;
	uint32_t bump;
};