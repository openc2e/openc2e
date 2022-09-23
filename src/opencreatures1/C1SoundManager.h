#pragma once

#include "C1Sound.h"
#include "common/SlotMap.h"
#include "common/audio/AudioBackend.h"
#include "common/audio/AudioState.h"

#include <chrono>
#include <memory>
#include <string>
#include <vector>

using fmilliseconds = std::chrono::duration<float, std::milli>;

class AudioBackend;
class PathManager;
class ViewportManager;

class C1SoundManager {
  public:
	C1SoundManager(std::shared_ptr<AudioBackend> audio, std::shared_ptr<PathManager> paths, std::shared_ptr<ViewportManager> viewport);
	~C1SoundManager();

	void tick();
	C1Sound play_sound(std::string filename, bool loop = false);

	bool is_muted();
	void set_muted(bool);

  private:
	friend class C1Sound;
	struct SoundData {
		SoundData() = default;
		SoundData(SoundData&&) = default;
		SoundData& operator=(SoundData&&) = default;
		SoundData(const SoundData&) = delete;
		SoundData& operator=(const SoundData&) = delete;

		AudioChannel handle{};

		bool positioned = false;
		float x = 0;
		float y = 0;
		float width = 0;
		float height = 0;

		std::chrono::time_point<std::chrono::steady_clock> fade_start{};
		fmilliseconds fade_length{};
	};

	DenseSlotMap<SoundData> sources;
	using SoundId = decltype(sources)::Key;

	bool is_alive(SoundData& source);
	void stop(SoundData& source);
	AudioState get_channel_state(SoundData& source);
	void update_volume(SoundData& source);
	void update_volumes();
	SoundData* get_sound_data(C1Sound& source);
	C1Sound get_new_sound(AudioChannel handle);

	std::shared_ptr<AudioBackend> audio;
	std::shared_ptr<PathManager> paths;
	std::shared_ptr<ViewportManager> viewport;
	bool muted = false;
};