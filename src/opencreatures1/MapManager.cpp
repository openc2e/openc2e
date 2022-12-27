#include "MapManager.h"

#include "EngineContext.h"
#include "common/render/RenderSystem.h"

void MapManager::add_room(Room&& room) {
	if (!room.renderitem) {
		uint32_t color;
		if (room.type == 0) {
			color = 0xFFFF00CC;
		} else if (room.type == 1) {
			color = 0x00FFFFCC;
		} else {
			color = 0xFF00FFCC;
		}

		room.renderitem = g_engine_context.rendersystem->render_item_create(LAYER_ROOMS);
		g_engine_context.rendersystem->render_item_set_unfilled_rect(room.renderitem, room.left, room.top, room.width(), room.height(), color);
	}

	rooms.emplace_back(std::move(room));
}

void MapManager::set_background(creaturesImage background_) {
	background = background_;
	background_renderitem = g_engine_context.rendersystem->render_item_create(LAYER_BACKGROUND);
	g_engine_context.rendersystem->render_item_set_texture(background_renderitem, background.getTextureForFrame(0));
}
