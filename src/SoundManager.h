#pragma once

#include "Sound.h"
#include "audiobackend/AudioBackend.h"

#include <chrono>
#include <memory>
#include <string>
#include <vector>

using fmilliseconds = std::chrono::duration<float, std::milli>;

class SoundManager {
  public:
	SoundManager();
	~SoundManager();

	void tick();
	Sound playSound(std::string filename, bool loop = false);
	Sound playVoice(std::string filename);

	float getVolume();
	void setVolume(float);
	bool isMuted();
	void setMuted(bool);
	bool areCreatureVoicesMuted();

  private:
	friend class Sound;
	struct SoundData {
		SoundData() {
			resetAndIncrementGeneration();
		}
		SoundData(SoundData&&) = default;
		SoundData& operator=(SoundData&&) = default;
		SoundData(const SoundData&) = delete;
		SoundData& operator=(const SoundData&) = delete;

		bool isAlive();
		void resetAndIncrementGeneration() {
			generation++;
			handle = {};
			is_creature_voice = false;
			positioned = false;
			x = 0;
			y = 0;
			width = 0;
			height = 0;
			fade_start = {};
			fade_length = {};
		}

		int generation = 0;
		AudioChannel handle;

		bool is_creature_voice;
		bool positioned;
		float x;
		float y;
		float width;
		float height;

		std::chrono::time_point<std::chrono::steady_clock> fade_start;
		fmilliseconds fade_length;
	};
	std::vector<SoundData> sources;

	void updateVolume(SoundData& source);
	void updateVolumes();
	SoundData* getSoundData(Sound& source);
	Sound getNewSound(AudioChannel handle, bool is_creature_voice = false);

	bool sound_effects_muted = false;
	float sound_effects_volume = 1.0;

	float viewpoint_center_x;
	float viewpoint_center_y;
};

extern SoundManager soundmanager;
