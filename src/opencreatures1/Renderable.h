#pragma once

#include "common/FixedPoint.h"
#include "common/NumericCast.h"
#include "common/creaturesImage.h"
#include "common/math/Rect.h"
#include "common/render/RenderSystem.h"

#include <fmt/core.h>
#include <string>
#include <vector>

class Renderable {
  public:
	void set_position(fixed24_8_t x, fixed24_8_t y);
	void set_z_order(int z);
	fixed24_8_t get_x() const;
	fixed24_8_t get_y() const;

	int32_t width() const;
	int32_t height() const;
	Rect get_bbox() const;

	void set_object_sprite_base(int object_sprite_base);
	void set_part_sprite_base(int part_sprite_base);
	void set_sprite_index(int sprite_index);
	int32_t frame() const;
	void set_sprite(creaturesImage sprite);
	std::string get_sprite_name() const;

	bool has_animation() const;
	void set_animation(unsigned int animation_frame_, std::string animation_string_);
	void update_animation();
	void clear_animation();

  private:
	friend struct fmt::formatter<Renderable>;

	void update_renderitem();

	fixed24_8_t x = 0;
	fixed24_8_t y = 0;
	int z = 0;
	int object_sprite_base = 0;
	int part_sprite_base = 0;
	int sprite_index = 0;
	creaturesImage sprite;
	bool has_animation_ = false;
	unsigned int animation_frame = 0; // only if has_animation is true
	std::string animation_string; // only if has_animation is true

	RenderItemHandle renderitem;
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
		if (r.has_animation()) {
			return format_to(out, " animation={} anim_index={}>", r.animation_string, r.animation_frame);
		} else {
			return format_to(out, " animation=false>");
		}
	}
};