#pragma once

#include "openc2e-audiobackend/AudioChannel.h"

#include <chrono>
#include <memory>

class AudioBackend;
class PathManager;

class C1MusicManager {
  public:
	C1MusicManager(std::shared_ptr<PathManager>, std::shared_ptr<AudioBackend>);
	~C1MusicManager();

	float getVolume();
	void setVolume(float volume);
	bool isMuted();
	void setMuted(bool muted);

	void update();

  private:
     using c1clock = std::chrono::steady_clock;
      
	void updateVolumes();

	std::shared_ptr<AudioBackend> m_backend;
    std::shared_ptr<PathManager> m_path_manager;
    AudioChannel m_audio_channel;
	bool m_muted = false;
	float m_volume = 1.0;
	std::chrono::time_point<c1clock> m_timepoint_to_play_next_sound;
};