#pragma once


#include "common/math/Rect.h"
#include "common/render/RenderItemHandle.h"
#include "opencreatures1/ImageGallery.h"

#include <fmt/format.h>
#include <string>

class Renderable {
  public:
	void set_gallery(const ImageGallery& gallery);
	const ImageGallery& gallery() const;

	void set_base(int base);
	int32_t base() const;

	void set_pose(int pose);
	int32_t pose() const;

	void set_position(float x, float y);
	float x() const;
	float y() const;
	void set_z_order(int32_t z);
	int32_t z_order() const;

	bool has_animation() const;
	void set_animation(uint8_t animation_frame_, std::string animation_string_);
	uint8_t animation_frame() const;
	const std::string& animation_string() const;
	void update_animation();
	void clear_animation();

	int32_t width() const;
	int32_t height() const;

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