#pragma once

#include "common/audio/AudioState.h"

#include <limits>
#include <stdint.h>

class C1SoundManager;

class C1Sound {
  public:
	explicit operator bool();
	void fade_out();
	void stop();
	void set_position(float x, float y, float width, float height);
	AudioState get_state();

  private:
	friend class C1SoundManager;
	C1SoundManager* soundmanager = nullptr;
	uint32_t id = std::numeric_limits<uint32_t>::max();
};
