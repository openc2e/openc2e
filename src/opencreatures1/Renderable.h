#pragma once


#include "common/NumericCast.h"
#include "common/math/Rect.h"
#include "common/render/RenderSystem.h"
#include "opencreatures1/SpriteGallery.h"

#include <fmt/core.h>
#include <string>
#include <vector>

class Renderable {
  public:
	void set_position(float x, float y);
	void set_z_order(int32_t z);
	float get_x() const;
	float get_y() const;
	int32_t get_z_order() const;

	int32_t width() const;
	int32_t height() const;
	Rect get_bbox() const;

	void set_object_sprite_base(int object_sprite_base);
	void set_part_sprite_base(int part_sprite_base);
	void set_sprite_index(int sprite_index);
	int32_t get_sprite_index() const;
	int32_t frame() const;
	void set_sprite(const SpriteGallery& sprite);
	std::string get_sprite_name() const;

	bool has_animation() const;
	void set_animation(unsigned int animation_frame_, std::string animation_string_);
	void update_animation();
	void clear_animation();

  private:
	friend struct fmt::formatter<Renderable>;

	void update_renderitem();

	float x = 0;
	float y = 0;
	int32_t z = 0;
	int object_sprite_base = 0;
	int part_sprite_base = 0;
	int sprite_index = 0;
	SpriteGallery sprite;
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
			r.x, r.y, r.z, r.object_sprite_base, r.part_sprite_base, r.sprite_index, r.sprite.name);
		if (r.has_animation()) {
			return format_to(out, " animation={} anim_index={}>", r.animation_string, r.animation_frame);
		} else {
			return format_to(out, " animation=false>");
		}
	}
};