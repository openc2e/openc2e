#pragma once

#include "Object.h"

namespace sfc {
struct CreatureV1;
}

struct Creature : Object {
	// TODO: implement me
	const DullPart* get_part(int32_t) const override;
	void serialize(SFCContext&, sfc::CreatureV1*);
};
