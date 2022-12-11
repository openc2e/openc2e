#pragma once

#include "common/FixedPoint.h"
#include "common/NumericCast.h"
#include "common/creaturesImage.h"

#include <fmt/core.h>
#include <string>
#include <vector>

class Renderable {
  public:
	fixed24_8_t x = 0;
	fixed24_8_t y = 0;
	int z = 0;
	int object_sprite_base = 0;
	int part_sprite_base = 0;
	int sprite_index = 0;
	creaturesImage sprite;
	bool has_animation = false;
	unsigned int animation_frame = 0; // only if has_animation is true
	std::string animation_string; // only if has_animation is true

	int32_t frame() const {
		return object_sprite_base + part_sprite_base + sprite_index;
	}

	int32_t width() const {
		return numeric_cast<int32_t>(sprite.width(numeric_cast<uint32_t>(frame())));
	}

	int32_t height() const {
		return numeric_cast<int32_t>(sprite.height(numeric_cast<uint32_t>(frame())));
	}

	void clear_animation() {
		has_animation = false;
		animation_frame = 0;
		animation_string = {};
	}
};

template <>
struct fmt::formatter<Renderable> {
	template <typename ParseContext>
	constexpr auto parse(ParseContext& ctx) {
		return ctx.begin();
	}

	template <typename FormatContext>
	auto format(const Renderable& r, FormatContext& ctx) {
		auto out = format_to(ctx.out(),
			"<Renderable x={} y={} z={} object_base={} part_base={} index={} sprite={}",
			r.x, r.y, r.z, r.object_sprite_base, r.part_sprite_base, r.sprite_index, r.sprite.getName());
		if (r.has_animation) {
			return format_to(out, " animation={} anim_index={}>", r.animation_string, r.animation_frame);
		} else {
			return format_to(out, " animation=false>");
		}
	}
};