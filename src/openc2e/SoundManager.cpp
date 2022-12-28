#include "SoundManager.h"

#include "Engine.h"
#include "Map.h"
#include "PathResolver.h"
#include "World.h"
#include "caosValue.h"

#include <fmt/core.h>

// this is all for SoundManager::tick
#include "Camera.h"
#include "MetaRoom.h"
#include "Room.h"
#include "World.h"

#include <cmath>

SoundManager soundmanager;

SoundManager::SoundManager() {
}
SoundManager::~SoundManager() {
}

float SoundManager::getVolume() {
	return sound_effects_volume;
}

void SoundManager::setVolume(float volume) {
	sound_effects_volume = volume;
	updateVolumes();
}

bool SoundManager::isMuted() {
	return sound_effects_muted;
}

void SoundManager::setMuted(bool muted) {
	sound_effects_muted = muted;
	updateVolumes();
}

bool SoundManager::SoundData::isAlive() {
	if (handle && engine.audio->audio_channel_get_state(handle) == AUDIO_PLAYING) {
		return true;
	} else {
		// resetAndIncrementGeneration();
		return false;
	}
}

SoundManager::SoundData* SoundManager::getSoundData(Sound& sound) {
	static_assert(sizeof(Sound) == sizeof(SoundManager::SoundId), "");
	SoundId id = *reinterpret_cast<SoundId*>(&sound); // TODO

	SoundData* data = sources.try_get(id);
	if (!data) {
		return nullptr;
	}

	return data;
}

Sound SoundManager::getNewSound(AudioChannel handle, bool is_creature_voice) {
	SoundData data;
	data.handle = handle;
	data.is_creature_voice = is_creature_voice;
	updateVolume(data);

	SoundId id = sources.add(std::move(data));
	static_assert(sizeof(Sound) == sizeof(SoundManager::SoundId), "");
	return *reinterpret_cast<Sound*>(&id); // TODO
}

bool SoundManager::areCreatureVoicesMuted() {
	if (engine.version == 3) {
		// TODO: is this slow?
		auto it = world.variables.find("engine_dumb_creatures");
		return it != world.variables.end() && it->second.hasInt() && it->second.getInt() != 0;
	} else {
		return false;
	}
}

template <typename T, typename U, typename V>
auto clamp(T value, U low, V high) {
	assert(!(high < low));
	return (value < low) ? low : (high < value) ? high : value;
}

void SoundManager::updateVolume(SoundData& s) {
	if (!s.isAlive()) {
		return;
	}

	float volume = sound_effects_muted ? 0 : sound_effects_volume;

	if (s.is_creature_voice && areCreatureVoicesMuted()) {
		volume = 0;
	}

	if (s.positioned) {
		MetaRoom* room = world.map->metaRoomAt(s.x, s.y);
		if (room && engine.camera->getMetaRoom() == room) {
			// std::remainder gives the distance between x and camerax, taking
			// into account metaroom wraparound ("modular distance")
			const float distx = std::remainder(s.x + s.width / 2 - engine.camera->getXCentre(), room->width());
			const float disty = s.y + s.height / 2 - engine.camera->getYCentre();

			const float screen_width = engine.camera->getWidth();
			const float screen_height = engine.camera->getHeight();

			// If a sound is on-screen, then play it at full volume.
			// If it's more than a screen offscreen, then mute it.
			// TODO: Does this sound right?
			const float starts_fading = 0.5;
			const float cutoff = 1.5;
			// the math
			float volx = 1.0;
			if (std::abs(distx) / screen_width > starts_fading) {
				volx = clamp(1 - (std::abs(distx) / screen_width - starts_fading) / (cutoff - starts_fading), 0, 1);
			}
			float voly = 1.0;
			if (std::abs(disty) / screen_height > starts_fading) {
				voly = clamp(1 - (std::abs(disty) / screen_height - starts_fading) / (cutoff - starts_fading), 0, 1);
			}
			volume *= std::min(volx, voly);

			// Pan sound as we get closer to screen edge
			// TODO: Does this sound right?
			float pan = clamp(distx / screen_width, -1, 1);
			engine.audio->audio_channel_set_pan(s.handle, pan);
		} else if (!room) {
			// TODO: think about volume when positioning outside-metaroom agents
			volume = 0;
		} else {
			volume = 0;
		}
	}

	if (s.fade_start != decltype(s.fade_start)()) {
		float volume_multiplier = 1 - (std::chrono::steady_clock::now() - s.fade_start) / s.fade_length;
		if (volume_multiplier <= 0) {
			engine.audio->audio_channel_stop(s.handle);
		}
		volume *= volume_multiplier;
	}

	engine.audio->audio_channel_set_volume(s.handle, volume);
}

void SoundManager::updateVolumes() {
	for (auto i : sources.enumerate()) {
		if (!i.value->isAlive()) {
			// Make sure erasing during enumeration is okay!!!
			sources.erase(i.id);
			continue;
		}
		updateVolume(*i.value);
	}
}

Sound SoundManager::playSound(std::string name, bool loop) {
	if (name.size() == 0)
		return {};

	std::string filename = findSoundFile(name + ".wav");
	if (filename.size() == 0) {
		if (engine.version < 3)
			return {}; // creatures 1 and 2 ignore non-existent audio clips
		throw Exception(fmt::format("No such clip '{}.wav'", name));
	}

	auto handle = engine.audio->play_clip(filename, loop);
	if (!handle) {
		// note that more specific error messages can be thrown by implementations of play_clip
		throw Exception("failed to load audio clip " + filename);
	}

	return getNewSound(handle);
}

Sound SoundManager::playVoice(std::string name) {
	if (name.size() == 0)
		return {};

	std::string filename = findSoundFile(name + ".wav");
	if (filename.size() == 0) {
		if (engine.version < 3)
			return {}; // creatures 1 and 2 ignore non-existent audio clips
		throw Exception(fmt::format("No such clip '{}.wav'", name));
	}

	auto handle = engine.audio->play_clip(filename);
	if (!handle) {
		// note that more specific error messages can be thrown by implementations of play_clip
		throw Exception("failed to load audio clip " + filename);
	}

	return getNewSound(handle, true);
}


void SoundManager::tick() {
	// track our current viewpoint center
	viewpoint_center_x = engine.camera->getXCentre();
	viewpoint_center_y = engine.camera->getYCentre();

	// update sounds
	// TODO: is this slow?
	updateVolumes();
}

/* vim: set noet: */
