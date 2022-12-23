#pragma once

#include "EngineContext.h"
#include "common/NumericCast.h"
#include "common/backend/Backend.h"
#include "common/backend/Keycodes.h"

#include <cmath>
#include <memory>

static constexpr int CREATURES1_WORLD_WIDTH = 8352;
static constexpr int CREATURES1_WORLD_HEIGHT = 1200;

class ViewportManager {
  public:
	ViewportManager() = default;
	void handle_event(const BackendEvent& event);
	void tick();

	int32_t width() const;

	int32_t height() const;

	int32_t centerx() const;

	int32_t centery() const;

	int32_t scrollx = 0;
	int32_t scrolly = 0;

  private:
	static constexpr float SCROLL_ACCEL = 8;
	static constexpr float SCROLL_DECEL = 0.5;
	static constexpr float SCROLL_MAX = 64;

	bool scroll_left = false;
	bool scroll_right = false;
	bool scroll_up = false;
	bool scroll_down = false;

	float scroll_velx = 0;
	float scroll_vely = 0;
};
