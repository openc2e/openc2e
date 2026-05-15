#include "SDLMixerBackend.h"

#include "common/Ascii.h"
#include "common/Exception.h"
#include "common/NumericCast.h"
#include "common/OutPtr.h"
#include "common/slotmap/DenseSlotMap.h"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>
#include <SDL3_native_midi/SDL_native_midi.h>
#include <ghc/filesystem.hpp>
#include <mutex>

namespace fs = ghc::filesystem;

template <typename T, void (*F)(T*)>
struct deleter {
	void operator()(T* ptr) {
		F(ptr);
	}
};

using MIXAudioDeleter = deleter<MIX_Audio, MIX_DestroyAudio>;
using MIXMixerDeleter = deleter<MIX_Mixer, MIX_DestroyMixer>;
using MIXTrackDeleter = deleter<MIX_Track, MIX_DestroyTrack>;
using NativeMidiSongDeleter = deleter<NativeMidi_Song, NativeMidi_DestroySong>;
using SDLFreeDeleter = deleter<void, SDL_free>;

struct ChannelInfo {
	std::unique_ptr<MIX_Track, MIXTrackDeleter> track;
};

static std::mutex s_channels_mutex;
static DenseSlotMap<ChannelInfo> s_channels;
static std::unique_ptr<NativeMidi_Song, NativeMidiSongDeleter> s_midi;
static std::unique_ptr<MIX_Mixer, MIXMixerDeleter> s_mixer;

template <typename F>
static void audio_channel_to_track(AudioChannel source, F&& f) {
	std::lock_guard<std::mutex> lock(s_channels_mutex);
	ChannelInfo* channel_p = s_channels.try_get(SlotMapKey::from_integral(source.handle));
	if (!channel_p) {
		return;
	}
	// Hold lock while callback is using MIX_Track pointer, so it doesn't get destroyed
	// out from under us.
	f(channel_p->track.get());
}

static AudioChannel track_to_audio_channel(std::unique_ptr<MIX_Track, MIXTrackDeleter>&& track) {
	using KeyIntegerType = decltype(SlotMapKey().to_integral());
	static_assert(sizeof(void*) >= sizeof(KeyIntegerType), "");

	MIX_Track* trackp = track.get();
	SlotMapKey key;
	{
		std::lock_guard<std::mutex> lock(s_channels_mutex);
		key = s_channels.insert(ChannelInfo{std::move(track)});
	}

	MIX_SetTrackStoppedCallback(
		trackp, [](void* key_, MIX_Track*) {
			auto key = SlotMapKey::from_integral(numeric_cast<KeyIntegerType>(reinterpret_cast<uintptr_t>(key_)));
			std::lock_guard<std::mutex> lock(s_channels_mutex);
			s_channels.erase(key);
		},
		reinterpret_cast<void*>(numeric_cast<uintptr_t>(key.to_integral())));

	return AudioChannel{key.to_integral()};
}

AudioBackend* SDLMixerBackend::get_instance() {
	static SDLMixerBackend s_sdl_mixer_backend;
	return &s_sdl_mixer_backend;
}

void SDLMixerBackend::play_midi_file(const std::string& filename) {
	// TODO: also support SDL_mixer's Timidity/FluidSynth support
	s_midi.reset(NativeMidi_LoadSong(filename.c_str()));
	if (!s_midi) {
		fmt::print("* SDLMixer: Couldn't load {}: {}\n", filename, SDL_GetError());
		return;
	}
	NativeMidi_Start(s_midi.get(), -1); // TODO: is looping forever correct?
	const auto shortname = to_ascii_lowercase(fs::path(filename).filename().string());
	fmt::print("INFO [SDLMixer] Playing {} looping=true\n", shortname);
}

void SDLMixerBackend::midi_set_volume(float volume) {
	NativeMidi_SetVolume(volume);
}

void SDLMixerBackend::midi_stop() {
	s_midi.reset();
}

static std::string sdl_audioformat_to_string(SDL_AudioFormat audioformat) {
	switch (audioformat) {
		case SDL_AUDIO_U8: return "u8";
		case SDL_AUDIO_S8: return "s8";
		case SDL_AUDIO_S16LE: return "s16le";
		case SDL_AUDIO_S16BE: return "s16be";
		case SDL_AUDIO_S32LE: return "s32le";
		case SDL_AUDIO_S32BE: return "s32be";
		case SDL_AUDIO_F32LE: return "f32le";
		case SDL_AUDIO_F32BE: return "f32be";
		default: return fmt::format("{}", fmt::underlying(audioformat));
	}
}

void SDLMixerBackend::init() {
	// TODO: ensure SDLBackend is in use?

	if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
		throw Exception(std::string("SDL error during sound initialization: ") + SDL_GetError());
	}

	if (!MIX_Init()) {
		printf("* SDLMixer: Mix_Init failed: %s\n", SDL_GetError());
	}

	if (!NativeMidi_Init()) {
		printf("* SDLMixer: NativeMidi_Init failed: %s\n", SDL_GetError());
	}

	s_mixer.reset(MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr));
	if (!s_mixer) {
		printf("* SDLMixer: Couldn't create mixer on default device: %s\n", SDL_GetError());
	}

	SDL_AudioSpec actual;
	if (MIX_GetMixerFormat(s_mixer.get(), &actual)) {
		fmt::print("* SDL Mixer: name={} channels={} freq={}Hz format={}\n",
			SDL_GetCurrentAudioDriver(),
			actual.channels,
			actual.freq,
			sdl_audioformat_to_string(actual.format));
	} else {
		fmt::print("* SDL Mixer: error getting format: {}\n", SDL_GetError());
	}
}

void SDLMixerBackend::shutdown() {
	{
		std::lock_guard<std::mutex> lock(s_channels_mutex);
		s_channels = {};
	}
	s_mixer.reset();
	MIX_Quit();

	s_midi.reset();
	NativeMidi_Quit();

	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SDLMixerBackend::audio_channel_set_volume(AudioChannel source, float v) {
	audio_channel_to_track(source, [&](auto* track) {
		MIX_SetTrackGain(track, v);
	});
}

static void audio_channel_set_pan(AudioChannel source, float pan) {
	audio_channel_to_track(source, [&](auto* track) {
		// assumes mono audio that has been duplicated to two channels
		// (not always true, there are a few game WAV files that are stereo. but
		// we can't handle panning true stereo without writing our own logic.)
		MIX_Point3D position;
		position.x = SDL_clamp(pan, -1, 1);
		position.y = 0;
		position.z = 0;
		MIX_SetTrack3DPosition(track, &position);
	});
}

void SDLMixerBackend::audio_channel_set_pan(AudioChannel source, float pan) {
	return ::audio_channel_set_pan(source, pan);
}

void SDLMixerBackend::audio_channel_fade_out(AudioChannel source, int32_t milliseconds) {
	audio_channel_to_track(source, [&](auto* track) {
		MIX_StopTrack(track, MIX_TrackMSToFrames(track, milliseconds));
	});
}

AudioState SDLMixerBackend::audio_channel_get_state(AudioChannel source) {
	AudioState result = AUDIO_STOPPED;
	audio_channel_to_track(source, [&](auto* track) {
		if (MIX_TrackPlaying(track)) {
			result = AUDIO_PLAYING;
		}
	});
	return result;
}

void SDLMixerBackend::audio_channel_stop(AudioChannel source) {
	audio_channel_to_track(source, [&](auto* track) {
		// Call MIX_StopTrack fading out over one frame to avoid segfaults:
		// https://github.com/libsdl-org/SDL_mixer/issues/852
		MIX_StopTrack(track, 1);
	});
}

static AudioChannel play_sdl_io(const std::string& name, SDL_IOStream* io, bool looping) {
	// Use SDL_LoadWAV because MIX_LoadAudio will read loop points from WAV files
	// and then loop the sound forever, which we don't want. See https://github.com/libsdl-org/SDL_mixer/issues/847
	// These Creatures 1 files have loop points: fall, kito, kitp, tele, pian, show, hmle, hdsk, hfml, sqek
	SDL_AudioSpec spec;
	std::unique_ptr<uint8_t, SDLFreeDeleter> data;
	uint32_t datalen = 0;
	if (!SDL_LoadWAV_IO(io, true, &spec, out_ptr(data), &datalen)) {
		printf("* SDLMixer: error loading %s: %s\n", name.c_str(), SDL_GetError());
		return {};
	}

	// TODO: expose concept of a loaded piece of audio (e.g., a MIX_Audio*) and let the
	// engine load and manage audio lifetimes, so we don't reload the same data over
	// and over.
	std::unique_ptr<MIX_Audio, MIXAudioDeleter> audio(MIX_LoadRawAudio(s_mixer.get(), data.get(), datalen, &spec));
	if (!audio) {
		printf("* SDLMixer: error loading %s: %s\n", name.c_str(), SDL_GetError());
		return {};
	}

	std::unique_ptr<MIX_Track, MIXTrackDeleter> track(MIX_CreateTrack(s_mixer.get()));
	if (!track) {
		printf("* SDLMixer: error creating track for %s: %s\n", name.c_str(), SDL_GetError());
		return {};
	}
	MIX_SetTrackAudio(track.get(), audio.get());

	if (looping) {
		MIX_SetTrackLoops(track.get(), -1);
	}

	MIX_Track* trackp = track.get();
	auto channel = track_to_audio_channel(std::move(track));

	// set initial panning, which will reduce volume a bit
	// (assumes mono audio, which isn't always true, there are a few true stereo WAV files)
	audio_channel_set_pan(channel, 0.0f);

	// now start the track
	MIX_PlayTrack(trackp, 0);

	const auto shortname = to_ascii_lowercase(fs::path(name).filename().string());
	fmt::print("INFO [SDLMixer] Playing {} format={}/{}Hz/{}{}\n", shortname, spec.channels, spec.freq, sdl_audioformat_to_string(spec.format), looping ? " looping=true" : "");

	return channel;
}

AudioChannel SDLMixerBackend::play_clip(const std::string& filename, bool looping) {
	return play_sdl_io(filename, SDL_IOFromFile(filename.c_str(), "rb"), looping);
}

AudioChannel SDLMixerBackend::play_wav_data(const std::string& name, const uint8_t* buf, size_t size, bool looping) {
	return play_sdl_io(name, SDL_IOFromMem((void*)buf, size), looping);
}