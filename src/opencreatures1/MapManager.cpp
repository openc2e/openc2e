#include "MapManager.h"

#include "common/NumericCast.h"
#include "common/render/RenderSystem.h"

constexpr bool RENDER_ROOM_OUTLINES = true;
constexpr bool RENDER_GROUNDLEVEL = true;

void MapManager::add_room(Room&& room) {
	rooms.emplace_back(std::move(room));
}

void MapManager::set_background(ImageGallery background_) {
	background = background_;
}

void MapManager::set_groundlevel(const std::array<uint32_t, 261>& groundlevel_) {
	groundlevel = groundlevel_;
}

int32_t MapManager::get_world_wrap_width() const {
	// should always be 8352
	return background.width(0);
}

void MapManager::render(RenderSystem& rendersystem) const {
	rendersystem.add(
		create_render_item_texture(
			get_background().texture,
			get_background().texture_locations[0],
			0, 0, 0),
		LAYER_BACKGROUND);

	if (RENDER_ROOM_OUTLINES) {
		for (const auto& room : get_rooms()) {
			Color color;
			if (room.type == 0) { // indoors
				color = {0xff, 0xff, 0, 0xcc}; // yellow
			} else if (room.type == 1) { // surface
				color = {0, 0xff, 0xff, 0xcc}; // aqua
			} else if (room.type == 2) { // undersea
				color = {0xff, 0, 0xff, 0xcc}; // magenta
			} else {
				color = {0, 0, 0, 0xcc}; // black
			}
			rendersystem.add(
				create_render_item_rect(
					Rect2f(
						numeric_cast<float>(room.left),
						numeric_cast<float>(room.top),
						numeric_cast<float>(room.width()),
						numeric_cast<float>(room.height())),
					color),
				LAYER_DEBUG);
		}
	}

	if (RENDER_GROUNDLEVEL) {
		for (size_t i = 0; i < 261; ++i) {
			rendersystem.add(
				create_render_item_line(
					numeric_cast<float>(i * 32), // TODO: i * CREATURES1_WORLD_WIDTH / 261 ?
					numeric_cast<float>((i + 1) * 32),
					numeric_cast<float>(get_groundlevel()[i]),
					numeric_cast<float>(get_groundlevel()[(i + 1) % 261]),
					Color{0xff, 0xff, 0xff, 0xcc}), // white
				LAYER_DEBUG);
		}
	}
}