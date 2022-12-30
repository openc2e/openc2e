#pragma once

#include "C1Sound.h"
#include "common/SlotMap.h"
#include "common/audio/AudioChannel.h"
#include "common/audio/AudioState.h"
#include "common/math/RectF.h"

#include <chrono>
#include <string>
#include <vector>

using fmilliseconds = std::chrono::duration<float, std::milli>;

class C1SoundManager {
  public:
	C1SoundManager();
	~C1SoundManager();

	void set_listener_position(RectF);
	void set_listener_world_wrap_width(int32_t wrap_width);

	C1Sound play_sound(std::string filename, bool loop = false);
	C1Sound play_positioned_sound(std::string filename, RectF initial_position, bool loop = false);

	bool is_muted();
	void set_muted(bool);

  private:
	struct SoundData {
		AudioChannel channel;
		RectF position;
		std::string name;
		bool looping = false;
	};

	std::vector<SoundData> data;

	RectF listener;
	int32_t world_wrap_width = 0;

	friend class C1Sound;

	bool is_alive(AudioChannel);
	void stop(AudioChannel);
	AudioState get_channel_state(AudioChannel);
	void update_volume(SoundData&);
	void update_volumes();
	SoundData* get_sound_data(AudioChannel source);
	C1Sound get_new_sound(AudioChannel handle);

	bool muted = false;
};