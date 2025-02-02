#pragma once

#include "SimpleObject.h"

namespace sfc {
struct BubbleV1;
}

struct Bubble : SimpleObject {
	// TODO: implement me
	void serialize(SFCContext&, sfc::BubbleV1*);
};