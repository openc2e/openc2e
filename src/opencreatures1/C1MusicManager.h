#pragma once

#include "C1ControlledSound.h"
#include "common/audio/AudioChannel.h"

#include <chrono>
#include <memory>

class PathManager;

class C1MusicManager {
  public:
	C1MusicManager();
	~C1MusicManager();

	float getVolume();
	void setVolume(float volume);
	bool isMuted();
	void setMuted(bool muted);

	void update();

  private:
	using c1clock = std::chrono::steady_clock;

	void updateVolumes();

	C1ControlledSound m_sound;
	bool m_muted = false;
	float m_volume = 1.0;
	std::chrono::time_point<c1clock> m_timepoint_to_play_next_sound;
};