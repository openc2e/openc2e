#pragma once

#include "ObjectHandle.h"
#include "SimpleObject.h"

#include <stdint.h>
#include <string>

namespace sfc {
struct PointerToolV1;
}

struct PointerTool : SimpleObject {
	int32_t relx;
	int32_t rely;
	ObjectHandle bubble;
	std::string text;

	void serialize(SFCContext&, sfc::PointerToolV1*);
};
