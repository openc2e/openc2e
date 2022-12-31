#include "C1ControlledSound.h"

#include "C1SoundManager.h"
#include "EngineContext.h"
#include "common/audio/AudioBackend.h"

C1ControlledSound::C1ControlledSound(C1ControlledSound&& other) {
	channel = other.channel;
	other.channel = {};
}

C1ControlledSound& C1ControlledSound::operator=(C1ControlledSound&& other) {
	fade_out();
	channel = other.channel;
	other.channel = {};
	return *this;
}

C1ControlledSound::~C1ControlledSound() {
	// TODO: stop or fade out?
	// Ideally this is only called when an Object is destroyed, and otherwise
	// existing controlled sounds are handled when new ones are set
	if (channel) {
		stop();
	}
}

C1ControlledSound::operator bool() {
	if (!channel) {
		return false;
	}
	if (get_state() == AUDIO_PLAYING) {
		return true;
	}
	channel = {};
	return false;
}

void C1ControlledSound::fade_out() {
	// fade out over 15 ticks, which in C1 is 1.5 seconds
	get_audio_backend()->audio_channel_fade_out(channel, 1500);
}

void C1ControlledSound::stop() {
	get_audio_backend()->audio_channel_stop(channel);
}

void C1ControlledSound::set_volume(float volume) {
	if (auto* data = g_engine_context.sounds->get_sound_data(channel)) {
		// do this rather than directly with the audio backend because
		// SoundManager does some of its own volume management with distance
		// attenuation, muting, etc., and will override anything we set directly
		// with the backend
		data->volume = volume;
	}
}

void C1ControlledSound::set_position(float x, float y, float width, float height) {
	if (auto* data = g_engine_context.sounds->get_sound_data(channel)) {
		data->position.x = x;
		data->position.y = y;
		data->position.width = width;
		data->position.height = height;
		g_engine_context.sounds->update_volume(*data);
	}
}

AudioState C1ControlledSound::get_state() {
	return get_audio_backend()->audio_channel_get_state(channel);
}
