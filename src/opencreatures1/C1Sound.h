#pragma once

#include "openc2e-audiobackend/AudioState.h"

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
	uint32_t id = ~0;
};