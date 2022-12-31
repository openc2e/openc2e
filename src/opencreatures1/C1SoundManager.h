#pragma once

#include "common/SlotMap.h"
#include "common/audio/AudioChannel.h"
#include "common/audio/AudioState.h"
#include "common/math/RectF.h"

#include <chrono>
#include <string>
#include <vector>

using fmilliseconds = std::chrono::duration<float, std::milli>;

class C1ControlledSound;

class C1SoundManager {
  public:
	C1SoundManager();
	~C1SoundManager();

	void set_listener_position(RectF);
	void set_listener_world_wrap_width(int32_t wrap_width);

	// play a sound effect tied to the returned C1ControlledSound handle. if the
	// handle is destroyed the sound is stopped.
	[[nodiscard]] C1ControlledSound play_controlled_sound(
		std::string filename, RectF initial_position = {}, bool loop = false);
	// play a non-looping sound effect from a world position. will play until the sound
	// has finished.
	void play_uncontrolled_sound(std::string filename, RectF initial_position);

	bool is_muted();
	void set_muted(bool);

  private:
	struct SoundData {
		AudioChannel channel;
		RectF position;
		std::string name;
		bool looping = false;
		float volume = 1.0;
	};

	std::vector<SoundData> data;

	RectF listener;
	int32_t world_wrap_width = 0;

	friend C1ControlledSound;

	bool is_alive(AudioChannel);
	void stop(AudioChannel);
	AudioState get_channel_state(AudioChannel);
	void update_volume(SoundData&);
	void update_volumes();
	SoundData* get_sound_data(AudioChannel source);
	AudioChannel play_sound_helper(std::string filename, RectF initial_position, bool loop);

	bool muted = false;
};