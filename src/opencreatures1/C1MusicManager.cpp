#include "C1MusicManager.h"

#include "PathManager.h"
#include "common/audio/AudioBackend.h"

#include <fmt/core.h>

// Creatures 1 plays the first sound after 20 ticks, and each sound after that
// every 50 + (rand() % 50) ticks.
// Each tick takes 100ms.

using namespace std::chrono;

static constexpr int MS_PER_TICK = 100;

C1MusicManager::C1MusicManager(std::shared_ptr<PathManager> path_manager, std::shared_ptr<AudioBackend> backend)
	: m_backend(backend), m_path_manager(path_manager) {
	m_timepoint_to_play_next_sound = c1clock::now() + milliseconds(20 * MS_PER_TICK);
}

C1MusicManager::~C1MusicManager() {
}

float C1MusicManager::getVolume() {
	return m_volume;
}

void C1MusicManager::setVolume(float volume) {
	m_volume = volume;
	updateVolumes();
}

bool C1MusicManager::isMuted() {
	return m_muted;
}

void C1MusicManager::setMuted(bool muted) {
	m_muted = muted;
	updateVolumes();
}


void C1MusicManager::updateVolumes() {
	// reduce overall volume of clips to 80%
	m_backend->setChannelVolume(m_audio_channel, m_muted ? 0 : m_volume * 0.8);
}

void C1MusicManager::update() {
	// play C1 music
	if (m_timepoint_to_play_next_sound <= c1clock::now() && m_backend->getChannelState(m_audio_channel) == AUDIO_STOPPED) {
		auto sounds = m_path_manager->find_path_wildcard(PATH_TYPE_SOUND, "MU*.wav");
		if (sounds.size()) {
			m_audio_channel = m_backend->playClip(sounds[rand() % sounds.size()]);
		}
		auto ticks_until_next_sound = 50 + (rand() % 50);
		m_timepoint_to_play_next_sound = c1clock::now() + milliseconds(MS_PER_TICK * ticks_until_next_sound);
	}
	// update volumes based on new volumes, muting, etc
	updateVolumes();
}
