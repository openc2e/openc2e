#pragma once

/*

____                _           ______   __   _
|  _ \ ___ _ __   __| | ___ _ __/ ___\ \ / /__| |_ ___ _ __ ___
| |_) / _ \ '_ \ / _` |/ _ \ '__\___ \\ V / __| __/ _ \ '_ ` _ \
|  _ <  __/ | | | (_| |  __/ |   ___) || |\__ \ ||  __/ | | | | |
|_| \_\___|_| |_|\__,_|\___|_|  |____/ |_||___/\__\___|_| |_| |_|

Simple retained-mode renderer for openc2e.

Five concepts (so far): RenderItems, RenderLayers, WorldWrap, the Main Camera, and
the Main Viewport.

* RenderItems: Objects which wish to draw on the screen should call render_item_create()
  and then set the various properties, including e.g. the texture, a rect,
  position, z-order, and layer.

* RenderLayers are referred to by numeric index and are a coarse way to both set
  high-level z-order (e.g. background should always be behind everything, UI
  should always be above agents).

* WorldWrap is the width of the world, after which RenderItems start wrapping back
  around (needed for Creatures 1).

* MainCamera is the position of the camera looking at the world.

* MainViewport is the where the MainCamera is drawn onto the actual screen.

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
#include "common/Color.h"
#include "common/SlotMap.h"
#include "common/backend/BackendTexture.h"
#include "common/math/Rect.h"

#include <stdint.h>

// These layer indexes aren't enforced, but are useful so keep them here.
enum LayerIndex : int32_t {
	LAYER_BACKGROUND,
	LAYER_OBJECTS,
	LAYER_DEBUG
};

class RenderSystem {
	friend RenderItemHandle;
	enum RenderItemType {
		RENDER_NONE,
		RENDER_TEXTURE,
		RENDER_RECT,
		RENDER_LINE
	};
	struct RenderItem {
		int32_t layer = 0;
		Rect2i src{};
		Rect2f dest{};
		int32_t z = 0;
		RenderItemType type = RENDER_NONE;
		Color color;
		Texture tex;
	};

	int32_t m_world_wrap_width = 0;
	Rect2i m_main_camera_src_rect;
	Rect2f m_main_viewport_dest_rect;
	DenseSlotMap<RenderItem> m_render_items;

  public:
	RenderSystem(const struct RenderSystemConstructTag&);
	RenderSystem(const RenderSystem&) = delete;
	RenderSystem(RenderSystem&&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;
	RenderSystem& operator=(RenderSystem&&) = delete;

	// MainCamera
	void main_camera_set_src_rect(Rect2i);
	const Rect2i& main_camera_get_src_rect() const;

	// MainViewport
	void main_viewport_set_dest_rect(Rect2f);
	const Rect2f& main_viewport_get_dest_rect() const;

	// WorldWrap
	void world_set_wrap_width(int32_t wrap_width);
	int32_t world_get_wrap_width() const;

	// RenderItems
	RenderItemHandle render_item_create(int layer = 0);

	void render_item_set_position(const RenderItemHandle& key, float x, float y, int32_t z);
	void render_item_set_texture(const RenderItemHandle& key, const Texture& tex, Rect2i location);
	void render_item_set_unfilled_rect(const RenderItemHandle& key, float x, float y, float w, float h, Color color);
	void render_item_set_line(const RenderItemHandle& key, float xstart, float ystart, float xend, float yend, Color color);

	void render_item_set_layer(const RenderItemHandle& key, int32_t layer);
	void render_item_set_position(const RenderItemHandle& key, float x, float y);
	void render_item_set_texture_size(const RenderItemHandle& key, int32_t w, int32_t h); // allows negative values, handles MIRA, SCLE, STRC. hmm.. how hard/easy to make this?
	void render_item_set_z_order(const RenderItemHandle& key, int32_t z);
	void render_item_set_shows_on_camera(const RenderItemHandle& key, bool photogenic); // to implement ATTR 256 Camera Shy

	void draw();
};

// global instance
RenderSystem* get_rendersystem();