#pragma once

#include "openc2e-audiobackend/AudioState.h"

#include <stdint.h>

class Sound {
  public:
	Sound();
	operator bool();
	void fadeOut();
	void stop();
	void setPosition(float x, float y, float width, float height);
	AudioState getState();

  private:
	friend class SoundManager;
	uint32_t id = ~0;
};