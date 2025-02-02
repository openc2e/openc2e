#pragma once


#include "common/math/Rect.h"
#include "common/render/RenderItemHandle.h"
#include "opencreatures1/ImageGallery.h"

#include <fmt/format.h>
#include <string>

class Renderable {
  public:
	void set_gallery(const ImageGallery& gallery);
	const ImageGallery& get_gallery() const;

	void set_base(int base);
	int32_t get_base() const { return base; }

	void set_pose(int pose);
	int32_t get_pose() const;

	void set_position(float x, float y);
	float get_x() const;
	float get_y() const;
	void set_z_order(int32_t z);
	int32_t get_z_order() const;

	bool has_animation() const;
	void set_animation(uint8_t animation_frame_, std::string animation_string_);
	uint8_t get_animation_frame() const { return animation_frame; }
	const std::string& get_animation_string() const { return animation_string; }
	void update_animation();
	void clear_animation();

	int32_t width() const;
	int32_t height() const;

  private:
	friend std::string format_as(const Renderable& r);

	int32_t frame() const;
	void update_renderitem();

	ImageGallery gallery;
	int32_t base = 0;
	int32_t pose = 0;
	float x = 0;
	float y = 0;
	int32_t z = 0;
	bool has_animation_ = false;
	uint8_t animation_frame = 0; // only if has_animation is true
	std::string animation_string; // only if has_animation is true

	RenderItemHandle renderitem;
};

inline std::string format_as(const Renderable& r) {
	if (r.has_animation()) {
		return fmt::format(
			"<Renderable x={} y={} z={} abba={} base={} pose={} gallery={} animation={} anim_index={}>",
			r.x, r.y, r.z, r.gallery.absolute_base, r.base, r.pose, r.gallery.name, r.animation_string, r.animation_frame);
	}
	return fmt::format(
		"<Renderable x={} y={} z={} abba={} base={} pose={} gallery={} animation=false>",
		r.x, r.y, r.z, r.gallery.absolute_base, r.base, r.pose, r.gallery.name);
}