#include "C1MusicManager.h"

#include "C1SoundManager.h"
#include "EngineContext.h"
#include "PathManager.h"
#include "common/Random.h"

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
	// reduce overall volume of clips to 50% (80% in the official engine)
	m_sound.set_volume(m_muted ? 0 : m_volume * 0.5f);
}

void C1MusicManager::update() {
	// play C1 music
	if (m_timepoint_to_play_next_sound <= c1clock::now() && m_sound.get_state() == AUDIO_STOPPED) {
		auto sounds = g_engine_context.paths->find_path_wildcard(PATH_TYPE_SOUND, "MU*.wav");
		if (sounds.size()) {
			auto name = rand_choice(sounds).string();
			m_sound = g_engine_context.sounds->play_sound(name.substr(0, name.size() - 4));
		}
		auto ticks_until_next_sound = rand_int32(50, 99);
		m_timepoint_to_play_next_sound = c1clock::now() + milliseconds(MS_PER_TICK * ticks_until_next_sound);

		// set volume based on new volumes, muting, etc
		updateVolumes();
	}
}
