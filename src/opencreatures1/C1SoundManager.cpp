#include "C1SoundManager.h"

#include "EngineContext.h"
#include "PathManager.h"
#include "common/Exception.h"
#include "common/Repr.h"
#include "common/audio/AudioBackend.h"

#include <cmath>
#include <fmt/core.h>

constexpr bool SOUND_MANAGER_DEBUG = false;

C1SoundManager::C1SoundManager() {
}
C1SoundManager::~C1SoundManager() {
}

bool C1SoundManager::is_muted() {
	return muted;
}

void C1SoundManager::set_muted(bool muted_) {
	muted = muted_;
	update_volumes();
}

C1SoundManager::SoundData* C1SoundManager::get_sound_data(AudioChannel channel) {
	if (!channel) {
		return nullptr;
	}

	uint16_t index = channel.handle & 0xffff;

	if (index >= data.size()) {
		return nullptr;
	}
	SoundData* sounddata = &data[index];
	if (sounddata->channel != channel) {
		return nullptr;
	}

	return sounddata;
}

template <typename T, typename U, typename V>
auto clamp(T value, U low, V high) {
	assert(!(high < low));
	return (value < low) ? low : (high < value) ? high : value;
}

struct DistanceInfo {
	float distx;
	float disty;
	float volume;
	float pan;
};

static DistanceInfo calculate_distance(RectF listener, int32_t world_wrap_width, RectF sound) {
	const auto centerx = listener.x + listener.width / 2;
	const auto centery = listener.y + listener.height / 2;

	// std::remainder gives the distance between x and centerx, taking
	// into account world wraparound ("modular distance")
	const float distx = std::remainder(
		sound.x + sound.width / 2.0f - centerx,
		(world_wrap_width ? world_wrap_width : 1) * 1.0f);
	const float disty = sound.y + sound.height / 2.0f - centery;

	const float screen_width = listener.width;
	const float screen_height = listener.height;

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
	float volume = std::min(volx, voly);

	// Pan sound as we get closer to screen edge
	// TODO: Does this sound right?
	float pan = clamp(distx / screen_width, -1, 1);

	DistanceInfo d;
	d.distx = distx;
	d.disty = disty;
	d.volume = volume;
	d.pan = pan;
	return d;
}


void C1SoundManager::update_volume(SoundData& s) {
	if (g_engine_context.audio_backend->audio_channel_get_state(s.channel) != AUDIO_PLAYING) {
		return;
	}

	float volume = muted ? 0 : 1;

	if (s.position != RectF{}) {
		auto distance = calculate_distance(listener, world_wrap_width, s.position);
		volume *= distance.volume;
		g_engine_context.audio_backend->audio_channel_set_pan(s.channel, distance.pan);
	}

	g_engine_context.audio_backend->audio_channel_set_volume(s.channel, volume);
}

void C1SoundManager::update_volumes() {
	for (auto& s : data) {
		update_volume(s);
	}
}

C1Sound C1SoundManager::play_sound(std::string name, bool loop) {
	return play_positioned_sound(name, RectF{}, loop);
}

C1Sound C1SoundManager::play_positioned_sound(std::string name, RectF initial_position, bool loop) {
	if (name.size() == 0) {
		return {};
	}

	if (initial_position != RectF{}) {
		auto distance = calculate_distance(listener, world_wrap_width, initial_position);
		if (distance.volume <= 0) {
			if (loop) {
				// TODO: probably worth not loading looping sounds, or at least
				// pausing them, until they're audible. SDL_Mixer still applies
				// effects and mixes channels that have zero volume.
			} else {
				// don't bother if it's too far off-screen
				return {};
			}
		}
	}

	std::string filename = g_engine_context.paths->find_path(PATH_TYPE_SOUND, name + ".wav");
	if (filename.empty()) {
		// creatures 1 ignores non-existent audio clips
		fmt::print("WARNING: couldn't find audio clip {}\n", repr(name));
		return {};
	}

	auto channel = g_engine_context.audio_backend->play_clip(filename, loop);
	if (!channel) {
		// note that more specific error messages can be thrown by implementations of play_clip
		throw_exception("failed to play audio clip {}{}", filename, loop ? " (loop)" : "");
	}

	uint16_t index = channel.handle & 0xffff;
	if (index >= data.size()) {
		data.resize(index + 1);
	}
	data[index] = {};
	data[index].channel = channel;
	data[index].name = name;
	data[index].position = initial_position;
	data[index].looping = loop;
	update_volume(data[index]);

	return C1Sound{channel};
}

void C1SoundManager::set_listener_position(RectF listener_) {
	if (SOUND_MANAGER_DEBUG) {
		fmt::print("C1SoundManager::set_listener_position\n");
		for (auto& s : data) {
			if (g_engine_context.audio_backend->audio_channel_get_state(s.channel) == AUDIO_PLAYING) {
				fmt::print("channel={} name={} loop={} position={}\n", s.channel.handle, s.name, s.looping, s.position);
			}
		}
		fmt::print("\n");
	}

	if (listener == listener_) {
		return;
	}
	listener = listener_;
	update_volumes();
}

void C1SoundManager::set_listener_world_wrap_width(int32_t world_wrap_width_) {
	if (world_wrap_width == world_wrap_width_) {
		return;
	}
	world_wrap_width = world_wrap_width_;
	update_volumes();
}
