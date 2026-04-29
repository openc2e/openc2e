#pragma once

#include "common/Color.h"
#include "common/backend/BackendTexture.h"
#include "common/math/Rect.h"

#include <stdint.h>

// These layer indexes aren't enforced, but are useful so keep them here.
enum LayerIndex : int32_t {
	LAYER_BACKGROUND = -1,
	LAYER_OBJECTS = 0,
	LAYER_DEBUG = 1,
};

struct RenderItem {
	enum RenderItemType {
		RENDER_NONE,
		RENDER_TEXTURE,
		RENDER_RECT,
		RENDER_LINE
	};
	RenderItemType type = RENDER_NONE;
	int32_t layer = 0;
	Rect2i src{};
	Rect2f dest{};
	int32_t z = 0;
	Color color;
	Texture tex;
};

RenderItem create_render_item_texture(Texture texture, Rect2i src, float x, float y, int32_t z);
RenderItem create_render_item_rect(Rect2f dest, Color color);
RenderItem create_render_item_line(float xstart, float xend, float ystart, float yend, Color color);