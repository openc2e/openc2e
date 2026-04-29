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
	const float width = numeric_cast<float>(get_backend()->getMainRenderTarget()->getWidth());
	const float height = numeric_cast<float>(get_backend()->getMainRenderTarget()->getHeight());
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
	scrollx = scrollx % CREATURES1_WORLD_WIDTH;

	// update rects and soundmanager
	// TODO: this doesn't feel like the best place for this
	main_camera = Rect2i{
		scrollx,
		scrolly,
		numeric_cast<int32_t>(width / VIEWPORT_SCALE),
		viewport_height};
	screen_dest = Rect2f{0, margin_top, width, height - margin_top - margin_bottom};
	g_engine_context.sounds->set_listener_position(main_camera);
}

const Rect2i& ViewportManager::get_main_camera_rect() const {
	return main_camera;
}

const Rect2f& ViewportManager::get_screen_dest_rect() const {
	return screen_dest;
}

float ViewportManager::window_x_to_world_x(float winx) const {
	// TODO: move this to RenderSystem?
	float worldx = (winx - screen_dest.x) / screen_dest.width * numeric_cast<float>(main_camera.width) + numeric_cast<float>(main_camera.x);
	worldx = remainderf(worldx, numeric_cast<float>(CREATURES1_WORLD_WIDTH));
	return worldx;
}

float ViewportManager::window_y_to_world_y(float winy) const {
	// TODO: move this to RenderSystem?
	return (winy - screen_dest.y) / screen_dest.height * numeric_cast<float>(main_camera.height) + numeric_cast<float>(main_camera.y);
}

void ViewportManager::set_scroll_position(int32_t scrollx_, int32_t scrolly_) {
	scrollx = scrollx_;
	scrolly = scrolly_;
}

void ViewportManager::set_margin_bottom(float margin_bottom_) {
	margin_bottom = margin_bottom_;
	update();
}