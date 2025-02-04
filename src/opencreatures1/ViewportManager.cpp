#include "ViewportManager.h"

#include "C1SoundManager.h"
#include "EngineContext.h"
#include "common/NumericCast.h"
#include "common/backend/Backend.h"
#include "common/backend/BackendEvent.h"
#include "common/backend/Keycodes.h"
#include "common/render/RenderSystem.h"

#include <math.h>

constexpr float VIEWPORT_SCALE = 1.0f;

void ViewportManager::handle_event(const BackendEvent& event) {
	if (!(event.type == eventrawkeyup || event.type == eventrawkeydown)) {
		return;
	}
	if (event.key == OPENC2E_KEY_LEFT) {
		scroll_left = (event.type == eventrawkeydown);
	} else if (event.key == OPENC2E_KEY_RIGHT) {
		scroll_right = (event.type == eventrawkeydown);
	} else if (event.key == OPENC2E_KEY_UP) {
		scroll_up = (event.type == eventrawkeydown);
	} else if (event.key == OPENC2E_KEY_DOWN) {
		scroll_down = (event.type == eventrawkeydown);
	}
}

void ViewportManager::tick() {
	// scroll left-right
	if (scroll_left) {
		scroll_velx -= SCROLL_ACCEL;
	}
	if (scroll_right) {
		scroll_velx += SCROLL_ACCEL;
	}
	if (!scroll_left && !scroll_right) {
		scroll_velx *= SCROLL_DECEL;
		if (-0.1 < scroll_velx && scroll_velx < 0.1) {
			scroll_velx = 0;
		}
	}
	// scroll up-down
	if (scroll_up) {
		scroll_vely -= SCROLL_ACCEL;
	}
	if (scroll_down) {
		scroll_vely += SCROLL_ACCEL;
	}
	if (!scroll_up && !scroll_down) {
		scroll_vely *= SCROLL_DECEL;
		if (-0.1 < scroll_vely && scroll_vely < 0.1) {
			scroll_vely = 0;
		}
	}
	// enforce scroll speed limits
	if (scroll_velx >= SCROLL_MAX) {
		scroll_velx = SCROLL_MAX;
	} else if (scroll_velx <= -SCROLL_MAX) {
		scroll_velx = -SCROLL_MAX;
	}
	if (scroll_vely >= SCROLL_MAX) {
		scroll_vely = SCROLL_MAX;
	} else if (scroll_vely <= -SCROLL_MAX) {
		scroll_vely = -SCROLL_MAX;
	}

	// do the actual movement
	scrollx += static_cast<int32_t>(scroll_velx);
	scrolly += static_cast<int32_t>(scroll_vely);

	// update based on new scroll
	update();
}

void ViewportManager::update() {
	const float width = get_backend()->getMainRenderTarget()->getWidth();
	const float height = get_backend()->getMainRenderTarget()->getHeight();
	const int32_t viewport_height = numeric_cast<int32_t>((height - margin_top - margin_bottom) / VIEWPORT_SCALE);

	// clamp scroll
	// can't go past top or bottom
	if (scrolly < 0) {
		scrolly = 0;
		scroll_vely = 0;
	} else if (CREATURES1_WORLD_HEIGHT - scrolly < viewport_height) {
		scrolly = CREATURES1_WORLD_HEIGHT - viewport_height;
		scroll_vely = 0;
	}
	// wraparound left and right
	if (scrollx < 0 || scrollx >= CREATURES1_WORLD_WIDTH) {
		scrollx = scrollx % CREATURES1_WORLD_WIDTH;
	}

	// update rendersystem and soundmanager
	// TODO: this doesn't feel like the best place for this
	Rect2i viewport{
		scrollx,
		scrolly,
		numeric_cast<int32_t>(width / VIEWPORT_SCALE),
		viewport_height};

	get_rendersystem()->main_camera_set_src_rect(viewport);
	get_rendersystem()->main_viewport_set_dest_rect({0, margin_top, width, height - margin_top - margin_bottom});
	g_engine_context.sounds->set_listener_position(viewport);
}

float ViewportManager::window_x_to_world_x(float winx) const {
	// TODO: move this to RenderSystem?
	Rect2i camera = get_rendersystem()->main_camera_get_src_rect();
	Rect2f viewport = get_rendersystem()->main_viewport_get_dest_rect();
	int32_t world_wrap = get_rendersystem()->world_get_wrap_width();

	float worldx = (winx - viewport.x) / viewport.width * camera.width + camera.x;
	if (world_wrap) {
		worldx = remainderf(worldx, numeric_cast<float>(world_wrap));
	}
	return worldx;
}

float ViewportManager::window_y_to_world_y(float winy) const {
	// TODO: move this to RenderSystem?
	Rect2i camera = get_rendersystem()->main_camera_get_src_rect();
	Rect2f viewport = get_rendersystem()->main_viewport_get_dest_rect();
	return (winy - viewport.y) / viewport.height * camera.height + camera.y;
}

void ViewportManager::set_scroll_position(int32_t scrollx_, int32_t scrolly_) {
	scrollx = scrollx_;
	scrolly = scrolly_;
}

void ViewportManager::set_margin_bottom(float margin_bottom_) {
	margin_bottom = margin_bottom_;
	update();
}