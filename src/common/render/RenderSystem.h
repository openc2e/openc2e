#pragma once

/*

____                _           ______   __   _
|  _ \ ___ _ __   __| | ___ _ __/ ___\ \ / /__| |_ ___ _ __ ___
| |_) / _ \ '_ \ / _` |/ _ \ '__\___ \\ V / __| __/ _ \ '_ ` _ \
|  _ <  __/ | | | (_| |  __/ |   ___) || |\__ \ ||  __/ | | | | |
|_| \_\___|_| |_|\__,_|\___|_|  |____/ |_||___/\__\___|_| |_| |_|

Simple renderer for openc2e.

Objects which wish to draw on the screen should create a RenderSystem::RenderItem
and then set the various properties, including e.g. the texture, a rect, position,
z-order, and layer.

*/

#include "common/math/Rect.h"
#include "common/render/RenderItem.h"

#include <stdint.h>
#include <vector>

class RenderSystem {
  public:
	struct DrawConfig {
		// The position of the camera looking at the world (world coordinates).
		Rect2i world_src;
		// The width of the world, after which RenderItems start wrapping back around (Creatures 1 and 2).
		int32_t world_wrap_width = INT32_MAX;
		// Where the MainCamera is drawn onto the actual screen (screen coordinates).
		Rect2f screen_dest;
		// Renderer to use to actually draw.
		class RenderTarget* renderer;
	};

  private:
	std::vector<RenderItem> m_render_items;

  public:
	RenderSystem();
	RenderSystem(const RenderSystem&) = delete;
	// RenderSystem(RenderSystem&&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;
	// RenderSystem& operator=(RenderSystem&&) = delete;

	void add(const RenderItem&);
	void add(const RenderItem&, LayerIndex layer);
	void draw(const DrawConfig&);
	void clear();
};