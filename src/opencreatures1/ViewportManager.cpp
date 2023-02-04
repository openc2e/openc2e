#include "ViewportManager.h"

#include "C1SoundManager.h"
#include "common/render/RenderSystem.h"

constexpr int32_t VIEWPORT_MARGIN_TOP = 20;
constexpr int32_t VIEWPORT_MARGIN_BOTTOM = 20;
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

	// fix scroll
	const float width = get_backend()->getMainRenderTarget()->getWidth();
	const float height = get_backend()->getMainRenderTarget()->getHeight();
	const int32_t viewport_height = numeric_cast<int32_t>((height - VIEWPORT_MARGIN_TOP - VIEWPORT_MARGIN_BOTTOM) / VIEWPORT_SCALE);
	// can't go past top or bottom
	if (scrolly < 0) {
		scrolly = 0;
		scroll_vely = 0;
	} else if (CREATURES1_WORLD_HEIGHT - scrolly < viewport_height) {
		scrolly = CREATURES1_WORLD_HEIGHT - viewport_height;
		scroll_vely = 0;
	}
	// wraparound left and right
	if (scrollx < 0) {
		scrollx = CREATURES1_WORLD_WIDTH + scrollx;
	} else if (scrollx >= CREATURES1_WORLD_WIDTH) {
		scrollx -= CREATURES1_WORLD_WIDTH;
	}

	// update rendersystem and soundmanager
	// TODO: this doesn't feel like the best place for this
	Rect viewport{
		scrollx,
		scrolly,
		numeric_cast<int32_t>(width / VIEWPORT_SCALE),
		viewport_height};

	get_rendersystem()->main_camera_set_src_rect(viewport);
	get_rendersystem()->main_viewport_set_dest_rect({0, VIEWPORT_MARGIN_TOP, width, height - VIEWPORT_MARGIN_TOP - VIEWPORT_MARGIN_BOTTOM});
	g_engine_context.sounds->set_listener_position(viewport);
}

int32_t ViewportManager::window_x_to_world_x(float winx) const {
	// TODO: move this to RenderSystem?
	Rect camera = get_rendersystem()->main_camera_get_src_rect();
	RectF viewport = get_rendersystem()->main_viewport_get_dest_rect();
	int32_t world_wrap = get_rendersystem()->world_get_wrap_width();

	int32_t worldx = int32_t((winx - viewport.x) / viewport.width * camera.width + camera.x);
	if (world_wrap) {
		if (worldx >= world_wrap) {
			worldx -= world_wrap;
		} else if (worldx < 0) {
			worldx += world_wrap;
		}
	}
	return worldx;
}

int32_t ViewportManager::window_y_to_world_y(float winy) const {
	// TODO: move this to RenderSystem?
	Rect camera = get_rendersystem()->main_camera_get_src_rect();
	RectF viewport = get_rendersystem()->main_viewport_get_dest_rect();
	return int32_t((winy - viewport.y) / viewport.height * camera.height + camera.y);
}

void ViewportManager::set_scroll_position(int32_t scrollx_, int32_t scrolly_) {
	scrollx = scrollx_;
	scrolly = scrolly_;
}