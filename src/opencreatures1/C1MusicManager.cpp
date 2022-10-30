#include "C1MusicManager.h"

#include "EngineContext.h"
#include "PathManager.h"
#include "common/Random.h"
#include "common/audio/AudioBackend.h"

#include <fmt/core.h>

// Creatures 1 plays the first sound after 20 ticks, and each sound after that
// every 50 + (rand() % 50) ticks.
// Each tick takes 100ms.

using namespace std::chrono;

static constexpr int MS_PER_TICK = 100;

C1MusicManager::C1MusicManager() {
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
	g_engine_context.audio_backend->setChannelVolume(m_audio_channel, m_muted ? 0 : m_volume * 0.8f);
}

void C1MusicManager::update() {
	// play C1 music
	if (m_timepoint_to_play_next_sound <= c1clock::now() && g_engine_context.audio_backend->getChannelState(m_audio_channel) == AUDIO_STOPPED) {
		auto sounds = g_engine_context.paths->find_path_wildcard(PATH_TYPE_SOUND, "MU*.wav");
		if (sounds.size()) {
			m_audio_channel = g_engine_context.audio_backend->playClip(rand_choice(sounds));
		}
		auto ticks_until_next_sound = rand_int32(50, 99);
		m_timepoint_to_play_next_sound = c1clock::now() + milliseconds(MS_PER_TICK * ticks_until_next_sound);
	}
	// update volumes based on new volumes, muting, etc
	updateVolumes();
}
