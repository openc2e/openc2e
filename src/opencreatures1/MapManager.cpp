#include "MapManager.h"

#include "common/NumericCast.h"
#include "common/render/RenderSystem.h"

void MapManager::add_room(Room&& room) {
	if (!room.renderitem) {
		Color color;
		if (room.type == 0) {
			color = {0xff, 0xff, 0, 0xcc};
		} else if (room.type == 1) {
			color = {0, 0xff, 0xff, 0xcc};
		} else {
			color = {0xff, 0, 0xff, 0xcc};
		}

		room.renderitem = get_rendersystem()->render_item_create(LAYER_DEBUG);
		get_rendersystem()->render_item_set_unfilled_rect(room.renderitem, room.left, room.top, room.width(), room.height(), color);
	}

	rooms.emplace_back(std::move(room));
}

void MapManager::set_background(ImageGallery background_) {
	background = background_;
	background_renderitem = get_rendersystem()->render_item_create(LAYER_BACKGROUND);
	get_rendersystem()->render_item_set_texture(background_renderitem, background.texture, background.texture_locations[0]);
}

void MapManager::set_groundlevel(const std::array<uint32_t, 261>& groundlevel_) {
	groundlevel = groundlevel_;
	for (size_t i = 0; i < 261; ++i) {
		groundlevel_renderitems[i] = get_rendersystem()->render_item_create(LAYER_DEBUG);
		int32_t xstart = numeric_cast<int32_t>(i * 32); // TODO: i * CREATURES1_WORLD_WIDTH / 261;
		int32_t xend = numeric_cast<int32_t>((i + 1) * 32);
		int32_t ystart = numeric_cast<int32_t>(groundlevel[i]);
		int32_t yend = numeric_cast<int32_t>(i == 260 ? groundlevel[0] : groundlevel[i + 1]);
		Color color{0xff, 0xff, 0xff, 0xcc};
		get_rendersystem()->render_item_set_line(groundlevel_renderitems[i], xstart, ystart, xend, yend, color);
	}
}