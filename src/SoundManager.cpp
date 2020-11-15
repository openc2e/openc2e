#include "SoundManager.h"
#include "World.h"
#include "Engine.h"
#include "Map.h"

// this is all for SoundManager::tick
#include "World.h"
#include "Camera.h"
#include "Room.h"
#include "MetaRoom.h"

SoundManager soundmanager;

SoundManager::SoundManager() {}
SoundManager::~SoundManager() {}

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
	if (handle && engine.audio->getChannelState(handle) == AUDIO_PLAYING) {
		return true;
	} else {
		resetAndIncrementGeneration();
		return false;
	}
}

Sound SoundManager::getNewSound(AudioChannel handle) {
	size_t i = 0;
	for (; i < sources.size(); ++i) {
		if (!sources[i].isAlive()) {
			break;
		}
	}
	if (i >= sources.size()) {
		sources.resize(i + 1);
	}
	
	sources[i].handle = handle;
	updateVolume(sources[i]);
	
	Sound source;
	source.index = i;
	source.generation = sources[source.index].generation;
	
	return source;
}

void SoundManager::updateVolume(SoundData& s) {
	if (!s.isAlive()) {
		return;
	}
	
	float volume = (s.muted || sound_effects_muted) ? 0 : s.volume * sound_effects_volume;
	engine.audio->setChannelVolume(s.handle, volume);
}

void SoundManager::updateVolumes() {
	for (auto &s : sources) {
		updateVolume(s);
	}
}

SoundManager::SoundData* SoundManager::getSoundData(Sound& source) {
	if (source.index < 0
		|| static_cast<unsigned int>(source.index) >= sources.size()
		|| source.generation != sources[source.index].generation
		|| !sources[source.index].isAlive())
	{
		source = {};
		return nullptr;
	}
	return &sources[source.index];
}

Sound SoundManager::playSound(std::string name, bool loop) {
	if (name.size() == 0) return {};

	std::string filename = world.findFile(fmt::format("Sounds/{}.wav", name));
	if (filename.size() == 0) {
		if (engine.version < 3) return {}; // creatures 1 and 2 ignore non-existent audio clips
		throw creaturesException(fmt::format("No such clip '{}.wav'", name));
	}

	auto handle = engine.audio->playClip(filename, loop);
	if (!handle) {
		// note that more specific error messages can be thrown by implementations of playClip
		throw creaturesException("failed to load audio clip " + filename);
	}

	return getNewSound(handle);
}

static bool inrange_at(const MetaRoom *room, float x, float y, unsigned int width, unsigned int height) {
	const static unsigned int buffer = 500;

	if (engine.camera->getMetaRoom() != room)
		return false;
	if (x + buffer < engine.camera->getX() || x + width - buffer > engine.camera->getX() + engine.camera->getWidth())
		return false;
	if (y + buffer < engine.camera->getY() || y + height - buffer > engine.camera->getY() + engine.camera->getHeight())
		return false;
	return true;
}

void SoundManager::tick() {
	// track our current viewpoint center
	viewpoint_center_x = engine.camera->getXCentre();
	viewpoint_center_y = engine.camera->getYCentre();

	// update positioned sounds
	for (auto& s : sources) {
		if (!(s.isAlive() && s.positioned)) {
			return;
		}

		MetaRoom *room = world.map->metaRoomAt(s.x, s.y);
		if (!room) {
			// TODO: think about inrange when positioning outside-metaroom agents
			continue;
		}

		// TODO: combine all this into inrange (somehow)
		float xc = s.x;
		bool inrange = false;
		if (inrange_at(room, s.x, s.y, s.width, s.height)) {
			xc = s.x;
			inrange = true;
		} else if (room->wraparound()) {
			if (inrange_at(room, s.x - room->width(), s.y, s.width, s.height)) {
				xc = s.x - room->width();
				inrange = true;
			} else if (inrange_at(room, s.x + room->width(), s.y, s.width, s.height)) {
				xc = s.x + room->width();
				inrange = true;
			}
		}

		s.muted = !inrange;
		updateVolume(s);
	}
}

/* vim: set noet: */
