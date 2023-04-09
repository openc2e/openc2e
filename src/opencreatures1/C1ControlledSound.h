#pragma once

#include "common/audio/AudioChannel.h"
#include "common/audio/AudioState.h"

#include <string>

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
	void set_volume(float);
	AudioState get_state();
	std::string get_name() const;
	bool get_looping() const;

  private:
	friend C1SoundManager;
	C1ControlledSound(AudioChannel channel_)
		: channel(channel_) {}
	AudioChannel channel;
};
