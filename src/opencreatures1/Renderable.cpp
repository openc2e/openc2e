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

void Renderable::set_object_sprite_base(int object_sprite_base_) {
	object_sprite_base = object_sprite_base_;
}
void Renderable::set_part_sprite_base(int part_sprite_base_) {
	part_sprite_base = part_sprite_base_;
}
void Renderable::set_sprite(creaturesImage sprite_) {
	sprite = sprite_;
	update_renderitem();
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
	return object_sprite_base + part_sprite_base + sprite_index;
}

int32_t Renderable::width() const {
	return numeric_cast<int32_t>(sprite.width(numeric_cast<uint32_t>(frame())));
}

int32_t Renderable::height() const {
	return numeric_cast<int32_t>(sprite.height(numeric_cast<uint32_t>(frame())));
}

void Renderable::set_animation(unsigned int animation_frame_, std::string animation_string_) {
	has_animation_ = true;
	animation_frame = animation_frame_;
	animation_string = animation_string_;
	// TODO: should this change sprite index immediately?
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
	set_sprite_index(animation_string[animation_frame] - '0');
	animation_frame += 1;
}

void Renderable::set_sprite_index(int sprite_index_) {
	sprite_index = sprite_index_;
	update_renderitem();
}

int32_t Renderable::get_sprite_index() const {
	return sprite_index;
}

std::string Renderable::get_sprite_name() const {
	return sprite.getName();
}


void Renderable::update_renderitem() {
	if (!renderitem) {
		renderitem = get_rendersystem()->render_item_create(LAYER_OBJECTS);
	}
	get_rendersystem()->render_item_set_texture(renderitem, sprite.getTextureForFrame(numeric_cast<unsigned int>(frame())));
	get_rendersystem()->render_item_set_position(renderitem, static_cast<float>(x), static_cast<float>(y), z);
}
