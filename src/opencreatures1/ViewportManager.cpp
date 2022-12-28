#include "ViewportManager.h"

#include "common/render/RenderSystem.h"

constexpr int32_t VIEWPORT_MARGIN_TOP = 20;
constexpr int32_t VIEWPORT_MARGIN_BOTTOM = 20;
constexpr float VIEWPORT_SCALE = 1.f;

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
	int32_t viewport_height = numeric_cast<int32_t>((numeric_cast<int32_t>(g_engine_context.backend->getMainRenderTarget()->getHeight()) - VIEWPORT_MARGIN_TOP - VIEWPORT_MARGIN_BOTTOM) / VIEWPORT_SCALE);
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


	// update rendersystem
	// TODO: this doesn't feel like the best place for this
	g_engine_context.rendersystem->main_camera_set_src_rect({scrollx, scrolly, numeric_cast<int32_t>(width() / VIEWPORT_SCALE), numeric_cast<int32_t>((height() - VIEWPORT_MARGIN_TOP - VIEWPORT_MARGIN_BOTTOM) / VIEWPORT_SCALE)});
	g_engine_context.rendersystem->main_camera_set_dest_rect({0, VIEWPORT_MARGIN_TOP, width(), height() - VIEWPORT_MARGIN_TOP - VIEWPORT_MARGIN_BOTTOM});
}

int32_t ViewportManager::window_x_to_world_x(int32_t winx) const {
	int32_t worldx = numeric_cast<int32_t>(winx / VIEWPORT_SCALE) + scrollx;
	// TODO: better way to handle world wrap?
	if (worldx >= CREATURES1_WORLD_WIDTH) {
		worldx -= CREATURES1_WORLD_WIDTH;
	}
	if (worldx < 0) {
		worldx += CREATURES1_WORLD_WIDTH;
	}
	return worldx;
}

int32_t ViewportManager::window_y_to_world_y(int32_t winy) const {
	return numeric_cast<int32_t>(winy / VIEWPORT_SCALE) + scrolly - VIEWPORT_MARGIN_TOP;
}

int32_t ViewportManager::width() const {
	return numeric_cast<int32_t>(g_engine_context.backend->getMainRenderTarget()->getWidth());
}

int32_t ViewportManager::height() const {
	return numeric_cast<int32_t>(g_engine_context.backend->getMainRenderTarget()->getHeight());
}

int32_t ViewportManager::centerx() const {
	return static_cast<int>(std::remainder(scrollx + width() / 2, CREATURES1_WORLD_WIDTH));
}

int32_t ViewportManager::centery() const {
	return scrolly + height() / 2;
}
