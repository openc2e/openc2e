#pragma once

#include "audiobackend/AudioBackend.h"
#include "Sound.h"
#include "MNGMusic.h"
#include <map>
#include <memory>
#include <string>

class SoundManager {
public:
	SoundManager();
	~SoundManager();

	void tick();
	Sound playSound(std::string filename, bool loop);

	float getVolume();
	void setVolume(float);
	bool isMuted();
	void setMuted(bool);

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
			muted = false;
			volume = 1.0;
			positioned = false;
			x = 0;
			y = 0;
			width = 0;
			height = 0;
		}
		
		int generation = 0;
		AudioChannel handle;

		bool muted;
		float volume;
		bool positioned;
		float x;
		float y;
		float width;
		float height;
	};
	std::vector<SoundData> sources;
	
	void updateVolume(SoundData& source);
	void updateVolumes();
	SoundData* getSoundData(Sound& source);
	Sound getNewSound(AudioChannel);
	
	bool sound_effects_muted = false;
	float sound_effects_volume = 1.0;
	
	float viewpoint_center_x;
	float viewpoint_center_y;
};

extern SoundManager soundmanager;
