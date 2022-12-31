#pragma once

#include "common/audio/AudioChannel.h"
#include "common/audio/AudioState.h"

class C1SoundManager;

class C1ControlledSound {
  public:
	C1ControlledSound() {}
	C1ControlledSound(const C1ControlledSound&) = delete;
	C1ControlledSound& operator=(const C1ControlledSound&) = delete;
	C1ControlledSound(C1ControlledSound&& other);
	C1ControlledSound& operator=(C1ControlledSound&& other);
	~C1ControlledSound();

	explicit operator bool();
	void fade_out();
	void stop();
	void set_position(float x, float y, float width, float height);
	AudioState get_state();
	void set_volume(float);

  private:
	friend C1SoundManager;
	C1ControlledSound(AudioChannel channel_)
		: channel(channel_) {}
	AudioChannel channel;
};
