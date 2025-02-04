#pragma once


#include "common/math/Rect.h"
#include "common/render/RenderItemHandle.h"
#include "opencreatures1/ImageGallery.h"

#include <fmt/format.h>
#include <string>

class Renderable {
  public:
	Renderable(); // TODO: null part? shouldn't allow this

	// the order of arguments here matches the CAOS command PAT: DULL, except that
	// coordinates are expected as world-absolute instead of relative to the object.
	Renderable(const ImageGallery& gallery, int32_t sprite_base, float x, float y, int32_t z);
	// Renderable(Renderable&&) = default;

	const ImageGallery& gallery() const;

	int32_t base() const;
	void set_base(int32_t base);

	int32_t pose() const;
	void set_pose(int32_t pose);

	float x() const;
	float y() const;
	int32_t z_order() const;
	void set_position(float x, float y);

	int32_t width() const;
	int32_t height() const;

	bool has_animation() const;
	void set_animation(uint8_t animation_frame_, std::string animation_string_);
	uint8_t animation_frame() const;
	const std::string& animation_string() const;
	void update_animation();
	void clear_animation();

  private:
	friend std::string format_as(const Renderable& r);

	int32_t frame() const;
	void update_renderitem();

	ImageGallery gallery_;
	int32_t base_ = 0;
	int32_t pose_ = 0;
	float x_ = 0;
	float y_ = 0;
	int32_t z_ = 0;
	bool has_animation_ = false;
	uint8_t animation_frame_ = 0; // only if has_animation is true
	std::string animation_string_; // only if has_animation is true

	RenderItemHandle renderitem_;
};