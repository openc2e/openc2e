#pragma once

#include "ObjectHandle.h"
#include "SimpleObject.h"

#include <stdint.h>

namespace sfc {
struct CallButtonV1;
}

struct CallButton : SimpleObject {
	ObjectHandle lift;
	uint8_t floor;

	void serialize(SFCContext&, sfc::CallButtonV1*);
};