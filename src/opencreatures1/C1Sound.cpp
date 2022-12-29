#include "C1Sound.h"

#include "C1SoundManager.h"
#include "EngineContext.h"
#include "common/audio/AudioBackend.h"

C1Sound::operator bool() {
	if (!channel) {
		return false;
	}
	if (get_state() == AUDIO_PLAYING) {
		return true;
	}
	channel = {};
	return false;
}

void C1Sound::fade_out() {
	// fade out over 15 ticks, which in C1 is 1.5 seconds
	g_engine_context.audio_backend->audio_channel_fade_out(channel, 1500);
}

void C1Sound::stop() {
	g_engine_context.audio_backend->audio_channel_stop(channel);
}

void C1Sound::set_volume(float volume) {
	g_engine_context.audio_backend->audio_channel_set_volume(channel, volume);
}

void C1Sound::set_position(float x, float y, float width, float height) {
	if (auto* data = g_engine_context.sounds->get_sound_data(channel)) {
		data->position.x = x;
		data->position.y = y;
		data->position.width = width;
		data->position.height = height;
		g_engine_context.sounds->update_volume(*data);
	}
}

AudioState C1Sound::get_state() {
	return g_engine_context.audio_backend->audio_channel_get_state(channel);
}
