#pragma once

/*

____                _           ______   __   _
|  _ \ ___ _ __   __| | ___ _ __/ ___\ \ / /__| |_ ___ _ __ ___
| |_) / _ \ '_ \ / _` |/ _ \ '__\___ \\ V / __| __/ _ \ '_ ` _ \
|  _ <  __/ | | | (_| |  __/ |   ___) || |\__ \ ||  __/ | | | | |
|_| \_\___|_| |_|\__,_|\___|_|  |____/ |_||___/\__\___|_| |_| |_|

Simple retained-mode renderer for openc2e.

Three concepts (so far): RenderItems, RenderLayers, and the Main Camera

* RenderItems: Objects which wish to draw on the screen should call render_item_create()
  and then set the various properties, including e.g. the texture, a rect,
  position, z-order, and layer.

* RenderLayers are referred to by numeric index and are a coarse way to both set
  high-level z-order (e.g. background should always be behind everything, UI
  should always be above agents).

* MainCamera is the position of the camera looking at the world.

Future improvements:

* Cameras: support C2E's camera system. Add additional camera items and track
  position, viewport size, and drawn size for each camera. Keep track of which
  ones need to be rendered or not.

* SortingGroup: support multi-sprite objects like Creatures and Blackboards. Render
  all sprites after another to prevent z-fighting with other objects. Sort RenderItems
  first on SortingGroup z-index rather than their own z-index, and then on their own
  z-index against other items w/in the group.

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

	Backend* m_backend = nullptr;
	int32_t m_world_wrap_width = 0;
	int32_t m_main_camera_x = 0;
	int32_t m_main_camera_y = 0;
	DenseSlotMap<RenderItem> m_render_items;

  public:
	RenderSystem(Backend* backend);

	void world_set_wrap_width(int32_t wrap_width);
	void main_camera_set_position(int32_t x, int32_t y);

	RenderItemHandle render_item_create(int layer = 0);

	void render_item_set_position(const RenderItemHandle& key, int32_t x, int32_t y, int32_t z);
	void render_item_set_texture(const RenderItemHandle& key, const Texture& tex);
	void render_item_set_unfilled_rect(const RenderItemHandle& key, int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);

	void render_item_set_layer(const RenderItemHandle& key, int32_t layer);
	void render_item_set_position(const RenderItemHandle& key, int32_t x, int32_t y);
	void render_item_set_texture_size(const RenderItemHandle& key, int32_t w, int32_t h); // allows negative values, handles MIRA, SCLE, STRC. hmm.. how hard/easy to make this?
	void render_item_set_z_order(const RenderItemHandle& key, int32_t z);
	void render_item_set_shows_on_camera(const RenderItemHandle& key, bool photogenic); // to implement ATTR 256 Camera Shy

	void draw();
};