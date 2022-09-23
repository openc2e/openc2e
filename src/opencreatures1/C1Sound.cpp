#include "C1Sound.h"

#include "C1SoundManager.h"
#include "common/audio/AudioBackend.h"

C1Sound::operator bool() {
	if (id == static_cast<uint32_t>(~0)) {
		return false;
	}
	if (get_state() == AUDIO_PLAYING) {
		return true;
	}
	id = static_cast<uint32_t>(~0);
	return false;
}

void C1Sound::fade_out() {
	if (auto source_data = soundmanager->get_sound_data(*this)) {
		// fade out over 15 ticks, which in C3/DS is 3/4 of a second
		source_data->fade_start = std::chrono::steady_clock::now();
		source_data->fade_length = std::chrono::milliseconds(750);
	} else {
		id = static_cast<uint32_t>(~0);
	}
}

void C1Sound::stop() {
	if (auto source_data = soundmanager->get_sound_data(*this)) {
		soundmanager->stop(*source_data);
		// data will be destroyed later in tick() ?
	}
	id = static_cast<uint32_t>(~0);
}

void C1Sound::set_position(float x, float y, float width, float height) {
	if (auto source_data = soundmanager->get_sound_data(*this)) {
		source_data->positioned = true;
		source_data->x = x;
		source_data->y = y;
		source_data->width = width;
		source_data->height = height;
		soundmanager->update_volume(*source_data);
	} else {
		id = static_cast<uint32_t>(~0);
	}
}

AudioState C1Sound::get_state() {
	if (auto source_data = soundmanager->get_sound_data(*this)) {
		return soundmanager->get_channel_state(*source_data);
	} else {
		id = static_cast<uint32_t>(~0);
		return AUDIO_STOPPED;
	}
}
