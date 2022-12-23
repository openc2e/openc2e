#pragma once

/*

____                _           ______   __   _
|  _ \ ___ _ __   __| | ___ _ __/ ___\ \ / /__| |_ ___ _ __ ___
| |_) / _ \ '_ \ / _` |/ _ \ '__\___ \\ V / __| __/ _ \ '_ ` _ \
|  _ <  __/ | | | (_| |  __/ |   ___) || |\__ \ ||  __/ | | | | |
|_| \_\___|_| |_|\__,_|\___|_|  |____/ |_||___/\__\___|_| |_| |_|

Simple retained-mode renderer for openc2e.

Two concepts (so far): RenderItems and RenderLayers.

* RenderItems: Objects which wish to draw on the screen should call create_render_item()
  and then set the various properties, including e.g. the texture, a rect,
  position, z-order, and layer.
  
* RenderLayers are referred to by numeric index and are a coarse way to both set
  high-level z-order (e.g. background should always be behind everything, UI
  should always be above agents) as well as group RenderItems for purposes of
  positioning.
  
Might add a RenderCamera concept in the future to support C2E's cameras. This
concept might take over some responsibilities from RenderLayer, such as position.

*/

#include "RenderItemHandle.h"
#include "common/SlotMap.h"
#include "common/backend/Backend.h"
#include "common/backend/BackendTexture.h"

#include <map>
#include <stdint.h>

// These layer indexes aren't enforced, but are useful so keep them here.
enum LayerIndex : int32_t {
	LAYER_BACKGROUND,
	LAYER_OBJECTS,
	LAYER_ROOMS,
	LAYER_UI
};

class RenderSystem {
	friend RenderItemHandle;
	enum RenderItemType {
		RENDER_NONE,
		RENDER_TEXTURE,
		RENDER_RECT
	};
	struct RenderItem {
		int32_t layer = 0;
		int32_t x = 0;
		int32_t y = 0;
		int32_t width = 0;
		int32_t height = 0;
		int32_t z = 0;
		RenderItemType type = RENDER_NONE;
		uint32_t color = 0;
		Texture tex;
	};
	struct RenderLayer {
		int32_t x = 0;
		int32_t y = 0;
		int32_t wrap_width = 0;
	};

	Backend* m_backend = nullptr;
	DenseSlotMap<RenderItem> m_render_items;
	// TODO: optimize this first by not accessing as often (see `draw()`),
	// secondly by replacing with a faster data structure (sorted vector?).
	std::map<int, RenderLayer> m_render_layers;

  public:
	RenderSystem(Backend* backend);

	void set_layer_position(int layer, int32_t x, int32_t y);
	void set_layer_wrap_width(int layer, int32_t wrap_width);

	RenderItemHandle create_render_item(int layer = 0);

	void set_render_item_position(const RenderItemHandle& key, int32_t x, int32_t y, int32_t z);

	void set_render_item_texture(const RenderItemHandle& key, const Texture& tex);

	void set_render_item_unfilled_rect(const RenderItemHandle& key, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);

	void set_render_item_layer(const RenderItemHandle& key, int32_t layer);
	void set_render_item_position(const RenderItemHandle& key, int32_t x, int32_t y);
	void set_render_item_texture_size(const RenderItemHandle& key, int32_t w, int32_t h); // allows negative values, handles MIRA, SCLE, STRC. hmm.. how hard/easy to make this?
	void set_render_item_z_order(const RenderItemHandle& key, int32_t z);
	void set_render_item_shows_on_camera(const RenderItemHandle& key, bool photogenic); // to implement ATTR 256 Camera Shy

	void draw();
};