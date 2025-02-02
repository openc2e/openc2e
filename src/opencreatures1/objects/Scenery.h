#pragma once

#include "Object.h"
#include "Renderable.h"

namespace sfc {
struct SceneryV1;
}

struct Scenery : Object {
	Renderable part;

	void serialize(SFCContext&, sfc::SceneryV1*);
};
