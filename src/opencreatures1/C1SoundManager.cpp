#include "C1SoundManager.h"

#include "C1ControlledSound.h"
#include "EngineContext.h"
#include "PathManager.h"
#include "common/Exception.h"
#include "common/audio/AudioBackend.h"

#include <assert.h>
#include <fmt/core.h>
#include <math.h>

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

static float logarithmic_attenuation(float x, float x_near, float x_far) {
	// NOTE: `near` and `far` are special keywords on Windows, can't use them

	// logarithmic attenuation function:
	// given a value `x`, make it drop off starting at `x_near` and ending at `x_far`
	// -log2(x) is 1 at 0.5 and 0 at 1
	// so, adjust our input to that range:
	// -log2( (x - x_near) / (x_far - x_near) / 2 + 0.5f )

	if (x <= x_near) {
		return 1;
	}
	if (x >= x_far) {
		return 0;
	}
	return clamp(-log2f((x - x_near) / (x_far - x_near) / 2 + 0.5f), 0, 1);
}

static DistanceInfo calculate_distance(Rect2f listener, int32_t world_wrap_width, Rect2f sound) {
	const auto centerx = listener.x + listener.width / 2;
	const auto centery = listener.y + listener.height / 2;

	// std::remainder gives the distance between x and centerx, taking
	// into account world wraparound ("modular distance")
	const float distx = remainderf(
		sound.x + sound.width / 2.0f - centerx,
		(world_wrap_width ? world_wrap_width : 1) * 1.0f);
	const float disty = sound.y + sound.height / 2.0f - centery;

	const float screen_width = listener.width;
	const float screen_height = listener.height;

	// If a sound is the center half-box of the screen, then play it at full volume.
	// If it's more than a half-screen offscreen, then mute it.
	// TODO: Does this sound right?
	const float starts_fading = 0.25;
	const float cutoff = 0.75;

	float volx = logarithmic_attenuation(fabsf(distx) / screen_width, starts_fading, cutoff);
	float voly = logarithmic_attenuation(fabsf(disty) / screen_height, starts_fading, cutoff);
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
	if (get_audio_backend()->audio_channel_get_state(s.channel) != AUDIO_PLAYING) {
		return;
	}

	float volume = muted ? 0 : s.volume;

	if (s.position != Rect2f{}) {
		auto distance = calculate_distance(listener, world_wrap_width, s.position);
		volume *= distance.volume;
		get_audio_backend()->audio_channel_set_pan(s.channel, distance.pan);
	}

	get_audio_backend()->audio_channel_set_volume(s.channel, volume);
}

void C1SoundManager::update_volumes() {
	for (auto& s : data) {
		update_volume(s);
	}
}

AudioChannel C1SoundManager::play_sound_helper(std::string name, Rect2f initial_position, bool loop) {
	if (name.size() == 0) {
		return {};
	}

	if (initial_position != Rect2f{}) {
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
		fmt::print("WARNING: couldn't find audio clip {:?}\n", name);
		return {};
	}

	auto channel = get_audio_backend()->play_clip(filename, loop);
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
	return channel;
}

void C1SoundManager::play_uncontrolled_sound(std::string name, Rect2f initial_position) {
	play_sound_helper(name, initial_position, false);
}

C1ControlledSound C1SoundManager::play_controlled_sound(std::string name, Rect2f initial_position, bool loop) {
	return C1ControlledSound{play_sound_helper(name, initial_position, loop)};
}

void C1SoundManager::set_listener_position(Rect2f listener_) {
	if (SOUND_MANAGER_DEBUG) {
		fmt::print("C1SoundManager::set_listener_position\n");
		for (auto& s : data) {
			if (get_audio_backend()->audio_channel_get_state(s.channel) == AUDIO_PLAYING) {
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
