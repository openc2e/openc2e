#pragma once

#include "DullPart.h"
#include "Object.h"

namespace sfc {
struct SceneryV1;
}

struct Scenery : Object {
	DullPart part;

	void serialize(SFCContext&, sfc::SceneryV1*);
};
