#pragma once

#include "Entity.h"
#include "Object.h"

namespace sfc {

struct SceneryV1 : ObjectV1 {
	std::shared_ptr<EntityV1> part;

	template <typename Archive>
	void serialize(Archive& ar) {
		ObjectV1::serialize(ar);
		ar(part);
	}
};

} // namespace sfc