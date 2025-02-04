#pragma once

#include "CGallery.h"
#include "fileformats/MFCObject.h"

#include <memory>
#include <stdint.h>
#include <string>

namespace sfc {

struct EntityV1 : MFCObject {
	std::shared_ptr<CGalleryV1> gallery;
	uint8_t sprite_pose_plus_base;
	uint8_t sprite_base;
	int32_t z_order; // TODO: should be unsigned? // absolute plane in world
	int32_t x; // absolute position in world
	int32_t y; // absolute position in world
	uint8_t has_animation;
	uint8_t animation_frame; // only if has_animation is true
	std::string animation_string; // only if has_animation is true

	template <typename Archive>
	void serialize(Archive& ar) {
		ar(gallery);
		ar(sprite_pose_plus_base);
		ar(sprite_base);
		ar(z_order);
		ar(x);
		ar(y);
		ar(has_animation);
		if (has_animation) {
			ar(animation_frame);
			// TODO: Assert real animation string?
			ar.ascii_nullterminated(animation_string, 32);
		}
	}
};

} // namespace sfc