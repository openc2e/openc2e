#include "Sound.h"
#include "SoundManager.h"
#include "audiobackend/AudioBackend.h"
#include "Engine.h"

Sound::Sound() = default;

Sound::operator bool() {
	return getState() == AUDIO_PLAYING;
}

void Sound::fadeOut() {
	if (auto source_data = soundmanager.getSoundData(*this)) {
		engine.audio->fadeOutChannel(source_data->handle);
	}
}

void Sound::stop() {
	if (auto source_data = soundmanager.getSoundData(*this)) {
		engine.audio->stopChannel(source_data->handle);
		// data will be destroyed later in tick() ?
	}
}

void Sound::setPosition(float x, float y, float width, float height) {
	if (auto source_data = soundmanager.getSoundData(*this)) {
		source_data->positioned = true;
		source_data->x = x;
		source_data->y = y;
		source_data->width = width;
		source_data->height = height;
		soundmanager.updateVolume(*source_data);
	}
}

void Sound::setMuted(bool muted) {
	if (auto source_data = soundmanager.getSoundData(*this)) {
		source_data->muted = muted;
		soundmanager.updateVolume(*source_data);
	}
}

AudioState Sound::getState() {
	if (auto source_data = soundmanager.getSoundData(*this)) {
		return engine.audio->getChannelState(source_data->handle);
	} else {
		return AUDIO_STOPPED;
	}
}

void Sound::setVolume(float volume) {
	if (auto source_data = soundmanager.getSoundData(*this)) {
		source_data->volume = volume;
		soundmanager.updateVolume(*source_data);
	}
}