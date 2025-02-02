#pragma once

#include "DullPart.h"
#include "Object.h"

namespace sfc {
struct SceneryV1;
}

struct Scenery : Object {
	DullPart part;

	const DullPart* get_part(int32_t) const override;
	void handle_left_click(float, float) override;
	void serialize(SFCContext&, sfc::SceneryV1*);
};
