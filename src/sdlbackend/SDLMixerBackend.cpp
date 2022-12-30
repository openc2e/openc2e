/*
 *  SDLMixerBackend.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Thu Oct 09 2008.
 *  Copyright (c) 2008 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "SDLMixerBackend.h"

#include "common/Exception.h"
#include "common/scope_guard.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <memory>
#include <string.h>

// Try to run at 44,100 Hz. Across supported games, the most common sound
// file sampling rates are 11,025 Hz, 22,050 Hz, 32,075 Hz and 44,100 Hz.
// There are only three sounds above 44,100 (all in Creatures Village, at
// 48,000 Hz).
constexpr int SDLMIXERBACKEND_FREQUENCY = 44100;
constexpr int SDLMIXERBACKEND_CHUNK_SIZE = 1024;

#if SDL_MIXER_COMPILEDVERSION < SDL_VERSIONNUM(2, 0, 2)
enum {
	MIX_INIT_MID = MIX_INIT_FLUIDSYNTH
};
#endif

struct MixChunkDeleter {
	void operator()(Mix_Chunk* chunk) {
		if (chunk) {
			Mix_FreeChunk(chunk);
		}
	}
};

struct ChunkRef {
	std::string name;
	std::unique_ptr<Mix_Chunk, MixChunkDeleter> ptr;
	int ref = 0;
	void add_ref() {
		ref++;
	}
	void release() {
		ref--;
	}
};

static std::vector<uint16_t> s_channel_to_generation;
static std::vector<size_t> s_channel_to_chunk;
static std::vector<ChunkRef> s_chunks;

static int audiochannel_to_int(AudioChannel source) {
	uint16_t channel = source.handle & 0xffff;
	uint16_t generation = (source.handle >> 16) & 0xffff;
	if (channel >= s_channel_to_generation.size()) {
		return -1;
	}
	if (s_channel_to_generation[channel] != generation) {
		return -1;
	}
	return channel;
}

static AudioChannel int_to_audio_channel(int channel) {
	if (channel < 0)
		return {};
	return {
		(uint32_t)channel | ((uint32_t)s_channel_to_generation[channel]) << 16};
}

AudioBackend* SDLMixerBackend::get_instance() {
	static SDLMixerBackend s_sdl_mixer_backend;
	return &s_sdl_mixer_backend;
}

void SDLMixerBackend::play_midi_file(const std::string& filename) {
	Mix_Music* music = Mix_LoadMUS(filename.c_str());
	if (!music) {
		printf("* SDLMixer: Couldn't load %s: %s\n", filename.c_str(), Mix_GetError());
		return;
	}
	midi_stop();
	Mix_PlayMusic(music, -1); // TODO: is looping forever correct?
	midi = music;
}

void SDLMixerBackend::midi_set_volume(float volume) {
	Mix_VolumeMusic(volume * MIX_MAX_VOLUME);
}

void SDLMixerBackend::midi_stop() {
	if (midi) {
		Mix_HaltMusic();
		Mix_FreeMusic(midi);
		midi = nullptr;
	}
}

static std::string sdl_audioformat_to_string(SDL_AudioFormat fmt) {
	switch (fmt) {
		case AUDIO_U8: return "AUDIO_U8";
		case AUDIO_S8: return "AUDIO_S8";
		case AUDIO_U16LSB: return "AUDIO_U16LSB";
		case AUDIO_U16MSB: return "AUDIO_U16MSB";
		case AUDIO_S16LSB: return "AUDIO_S16LSB";
		case AUDIO_S16MSB: return "AUDIO_S16MSB";
		case AUDIO_S32LSB: return "AUDIO_S32LSB";
		case AUDIO_S32MSB: return "AUDIO_S32MSB";
		case AUDIO_F32LSB: return "AUDIO_F32LSB";
		case AUDIO_F32MSB: return "AUDIO_F32MSB";
		default: return fmt::format("{}", fmt);
	}
}

void SDLMixerBackend::init() {
	// TODO: ensure SDLBackend is in use?

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
		throw Exception(std::string("SDL error during sound initialization: ") + SDL_GetError());

	if (Mix_OpenAudioDevice(SDLMIXERBACKEND_FREQUENCY, AUDIO_F32SYS, 2,
			SDLMIXERBACKEND_CHUNK_SIZE, nullptr, SDL_AUDIO_ALLOW_ANY_CHANGE) < 0) {
		throw Exception(std::string("SDL_mixer error during sound initialization: ") + Mix_GetError());
	}

	int actual_frequency;
	SDL_AudioFormat actual_format;
	int actual_channels;
	if (Mix_QuerySpec(&actual_frequency, &actual_format, &actual_channels) == 1) {
		fmt::print("* SDL Mixer: format={} channels={} freq={}Hz\n",
			sdl_audioformat_to_string(actual_format), actual_channels, actual_frequency);
	}

	if (!(Mix_Init(MIX_INIT_MID) & MIX_INIT_MID)) {
		printf("* SDLMixer: failed to load MIDI support: %s\n", Mix_GetError());
	}

	Mix_ChannelFinished([](int i) {
		// release reference to the chunk we just played
		if ((unsigned int)i >= s_channel_to_generation.size() || (unsigned int)i >= s_channel_to_chunk.size()) {
			printf("* SDLMixer: channel %i finished that we weren't tracking, this shouldn't happen\n", i);
		} else {
			s_channel_to_generation[i]++;
			s_chunks[s_channel_to_chunk[i]].release();
		}
	});
}

void SDLMixerBackend::shutdown() {
	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

static int playChannel(size_t chunk_idx, bool looping) {
	int channel = Mix_PlayChannel(-1, s_chunks[chunk_idx].ptr.get(), (looping ? -1 : 0));
	if (channel == -1) {
		Mix_AllocateChannels(Mix_AllocateChannels(-1) + 1);
		channel = Mix_PlayChannel(-1, s_chunks[chunk_idx].ptr.get(), (looping ? -1 : 0));
	}
	if (channel == -1) {
		printf("Couldn't play source: %s\n", Mix_GetError());
		return -1;
	}
	SDL_LockAudio();
	if ((unsigned int)channel >= s_channel_to_generation.size()) {
		s_channel_to_generation.resize(channel + 1, 0);
		s_channel_to_chunk.resize(channel + 1);
	}
	s_channel_to_chunk[channel] = chunk_idx;
	SDL_UnlockAudio();
	return channel;
}

static size_t add_chunk(std::string name, std::unique_ptr<Mix_Chunk, MixChunkDeleter>&& ptr) {
	for (size_t i = 0; i < s_chunks.size(); ++i) {
		if (s_chunks[i].ref == 0) {
			s_chunks[i].name = name;
			s_chunks[i].ptr = std::move(ptr);
			s_chunks[i].ref = 1;
			return i;
		}
		if (s_chunks[i].ref < 0) {
			throw Exception("ChunkRef was less than zero!");
		}
	}
	SDL_LockAudio();
	// Lock because we might reallocate s_chunks
	s_chunks.emplace_back(ChunkRef{name, std::move(ptr), 1});
	SDL_UnlockAudio();
	return s_chunks.size() - 1;
}

AudioChannel SDLMixerBackend::play_clip(const std::string& filename, bool looping) {
	// do we have this file already loaded?
	size_t chunk_idx = (size_t)-1;
	for (size_t i = 0; i < s_chunks.size(); ++i) {
		if (s_chunks[i].name == filename && s_chunks[i].ptr) {
			SDL_LockAudio();
			s_chunks[i].add_ref();
			SDL_UnlockAudio();
			chunk_idx = i;
			break;
		}
	}

	// nope, have to load it from scratch
	if (chunk_idx == (size_t)-1) {
		std::unique_ptr<Mix_Chunk, MixChunkDeleter> chunk{Mix_LoadWAV(filename.c_str())};
		if (!chunk) {
			return {};
		}
		chunk_idx = add_chunk(filename, std::move(chunk));
	}

	// new channel to play this chunk
	int channel = playChannel(chunk_idx, looping);
	if (channel == -1) {
		SDL_LockAudio();
		s_chunks[chunk_idx].release();
		SDL_UnlockAudio();
		return {};
	}

	// set panning, which will reduce volume by ~3db
	// (assumes mono audio, which isn't always true, there are a few true
	// stereo WAV files)
	audio_channel_set_pan(int_to_audio_channel(channel), 0.0);
	return int_to_audio_channel(channel);
}

AudioChannel SDLMixerBackend::play_wav_data(const uint8_t* data, size_t size, bool looping) {
	std::unique_ptr<Mix_Chunk, MixChunkDeleter> chunk{Mix_LoadWAV_RW(SDL_RWFromConstMem(data, size), SDL_TRUE)};
	if (!chunk) {
		return {};
	}

	size_t chunk_idx = add_chunk("", std::move(chunk));
	int channel = playChannel(chunk_idx, looping);
	if (channel == -1) {
		SDL_LockAudio();
		s_chunks[chunk_idx].release();
		SDL_UnlockAudio();
		return {};
	}

	// set panning, which will reduce volume by ~3db
	// (assumes mono audio, which is always true for MNG data)
	audio_channel_set_pan(int_to_audio_channel(channel), 0.0);
	return int_to_audio_channel(channel);
}

void SDLMixerBackend::audio_channel_set_volume(AudioChannel source, float v) {
	int channel = audiochannel_to_int(source);
	if (channel == -1)
		return;
	Mix_Volume(channel, v * MIX_MAX_VOLUME);
}

void SDLMixerBackend::audio_channel_set_pan(AudioChannel source, float pan) {
	int channel = audiochannel_to_int(source);
	if (channel == -1)
		return;

	pan = SDL_clamp(pan, -1, 1);

	// sin/cos panning algorithm
	// assumes mono audio that has been duplicated to two channels
	// (not always true, there are a few game WAV files that are stereo. but
	// we can't handle panning true stereo without writing our own Mixer Effect.)
	float x = (pan + 1) / 2;
	float left = cos(x * M_PI / 2);
	float right = sin(x * M_PI / 2);
	Mix_SetPanning(channel, 255 * left, 255 * right);
}

void SDLMixerBackend::audio_channel_fade_out(AudioChannel source, int32_t milliseconds) {
	int channel = audiochannel_to_int(source);
	if (channel == -1)
		return;
	Mix_FadeOutChannel(channel, milliseconds);
}

AudioState SDLMixerBackend::audio_channel_get_state(AudioChannel source) {
	int channel = audiochannel_to_int(source);
	if (channel != -1 && Mix_Playing(channel)) {
		return AUDIO_PLAYING;
	}
	return AUDIO_STOPPED;
}

void SDLMixerBackend::audio_channel_stop(AudioChannel source) {
	int channel = audiochannel_to_int(source);
	if (channel == -1)
		return;
	Mix_HaltChannel(channel);
}

/* vim: set noet: */
