#include "Renderable.h"

#include "EngineContext.h"
#include "ViewportManager.h"
#include "common/NumericCast.h"
#include "common/render/RenderSystem.h"

void Renderable::Renderable::set_position(float x_, float y_) {
	// TODO: better way of handling world wrap?
	if (x_ >= CREATURES1_WORLD_WIDTH) {
		x_ -= CREATURES1_WORLD_WIDTH;
	} else if (x_ < 0) {
		x_ += CREATURES1_WORLD_WIDTH;
	}

	x = x_;
	y = y_;
	update_renderitem();
}

void Renderable::set_z_order(int32_t z_) {
	z = z_;
	update_renderitem();
}

float Renderable::get_x() const {
	return x;
}
float Renderable::get_y() const {
	return y;
}

int32_t Renderable::get_z_order() const {
	return z;
}

void Renderable::set_base(int part_sprite_base_) {
	base = part_sprite_base_;
}

void Renderable::set_gallery(const ImageGallery& gallery_) {
	gallery = gallery_;
	update_renderitem();
}

const ImageGallery& Renderable::get_gallery() const {
	return gallery;
}

Rect Renderable::get_bbox() const {
	Rect r;
	// TODO: should we actually trunc these high-precision coordinates? This only
	// matters for vehicles.
	r.x = numeric_cast<int32_t>(x);
	r.width = width();
	r.y = numeric_cast<int32_t>(y);
	r.height = height();
	return r;
}

int32_t Renderable::frame() const {
	return gallery.absolute_base + base + pose;
}

int32_t Renderable::width() const {
	return gallery.width(frame());
}

int32_t Renderable::height() const {
	return gallery.height(frame());
}

void Renderable::set_animation(unsigned int animation_frame_, std::string animation_string_) {
	has_animation_ = true;
	animation_frame = animation_frame_;
	animation_string = animation_string_;

	// Change the sprite index immediately, because some scripts are written poorly and will
	// stop their animations on their next tick anyways. We assume that this has been called
	// from a CAOS macro, and that macros are run after object updates (e.g. if we don't set
	// the sprite index now, it won't get set until the next tick when the object updates).
	//
	// For example, the Creatures 1 Harp runs something like this when clicked:
	//
	//     anim [303030], snde hrp2, endi, else, endi, stim writ ..., stim shou ..., pose 0
	//
	// Whoops! The ANIM command will consume a tick, but ENDI, ELSE, SNDE, and STIM all run
	// instantly. So, the Harp gets a single frame to animate before resetting it's pose. This
	// has been confirmed on the official engine.
	update_animation();
}

bool Renderable::has_animation() const {
	return has_animation_;
}

void Renderable::clear_animation() {
	has_animation_ = false;
	animation_frame = 0;
	animation_string = {};
}

void Renderable::update_animation() {
	if (!has_animation_) {
		return;
	}

	if (animation_frame >= animation_string.size()) {
		// already done
		// TODO: are we on the correct frame already?
		clear_animation();
		return;
	}

	// some objects in Eden.sfc start at the 'R' character, so set frame
	// before incrementing.
	// TODO: assert isdigit
	if (animation_string[animation_frame] == 'R') {
		animation_frame = 0;
	}
	set_pose(animation_string[animation_frame] - '0');
	animation_frame += 1;
}

void Renderable::set_pose(int sprite_index_) {
	pose = sprite_index_;
	update_renderitem();
}

int32_t Renderable::get_pose() const {
	return pose;
}

void Renderable::update_renderitem() {
	if (!gallery) {
		renderitem = {};
		return;
	}
	if (!renderitem) {
		renderitem = get_rendersystem()->render_item_create(LAYER_OBJECTS);
	}

	get_rendersystem()->render_item_set_texture(renderitem,
		gallery.texture,
		gallery.texture_locations[numeric_cast<size_t>(frame())]);
	get_rendersystem()->render_item_set_position(renderitem, static_cast<float>(x), static_cast<float>(y), z);
}
