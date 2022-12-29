#pragma once

#include "common/audio/AudioChannel.h"
#include "common/audio/AudioState.h"

class C1SoundManager;

class C1Sound {
  public:
	C1Sound() {}
	C1Sound(const C1Sound&) = delete;
	C1Sound& operator=(const C1Sound&) = delete;
	C1Sound(C1Sound&& other);
	C1Sound& operator=(C1Sound&& other);
	~C1Sound();

	explicit operator bool();
	void fade_out();
	void stop();
	void set_position(float x, float y, float width, float height);
	AudioState get_state();
	void set_volume(float);

  private:
	friend class C1SoundManager;
	C1Sound(AudioChannel channel_)
		: channel(channel_) {}
	AudioChannel channel;
};
