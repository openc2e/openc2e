#include "ViewportManager.h"

#include "common/render/RenderSystem.h"


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
	int32_t window_height = numeric_cast<int32_t>(g_engine_context.backend->getMainRenderTarget()->getHeight());
	// can't go past top or bottom
	if (scrolly < 0) {
		scrolly = 0;
		scroll_vely = 0;
	} else if (CREATURES1_WORLD_HEIGHT - scrolly < window_height) {
		scrolly = CREATURES1_WORLD_HEIGHT - window_height;
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
	g_engine_context.rendersystem->set_layer_position(LAYER_BACKGROUND, scrollx, scrolly);
	g_engine_context.rendersystem->set_layer_position(LAYER_OBJECTS, scrollx, scrolly);
	g_engine_context.rendersystem->set_layer_position(LAYER_ROOMS, scrollx, scrolly);
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
