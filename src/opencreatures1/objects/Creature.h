#pragma once

#include "Object.h"

namespace sfc {
struct CreatureV1;
}

struct Creature : Object {
	// TODO: implement me
	void serialize(SFCContext&, sfc::CreatureV1*);
};
