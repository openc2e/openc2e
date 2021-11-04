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

#include <SDL.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <climits>
#include <memory>
#include <string.h>

#if SDL_MIXER_COMPILEDVERSION < SDL_VERSIONNUM(2, 0, 2)
enum {
	MIX_INIT_MID = MIX_INIT_FLUIDSYNTH
};
#endif

constexpr int SDLMIXERBACKEND_CHUNK_SIZE = 1024;

static std::vector<uint16_t> s_channel_to_generation;

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

SDLMixerBackend::SDLMixerBackend() {
}

void SDLMixerBackend::playMIDIFile(const std::string& filename) {
	Mix_Music* music = Mix_LoadMUS(filename.c_str());
	if (!music) {
		printf("* SDLMixer: Couldn't load %s: %s\n", filename.c_str(), Mix_GetError());
		return;
	}
	stopMIDI();
	Mix_PlayMusic(music, -1); // TODO: is looping forever correct?
	midi = music;
}

void SDLMixerBackend::setMIDIVolume(float volume) {
	Mix_VolumeMusic(volume * MIX_MAX_VOLUME);
}

void SDLMixerBackend::stopMIDI() {
	if (midi) {
		Mix_HaltMusic();
		Mix_FreeMusic(midi);
		midi = nullptr;
	}
}

void SDLMixerBackend::init() {
	// TODO: ensure SDLBackend is in use?

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
		throw Exception(std::string("SDL error during sound initialization: ") + SDL_GetError());

	// Try to run at 44,100 Hz. Across supported games, the most common sound
	// file sampling rates are 11,025 Hz, 22,050 Hz, 32,075 Hz and 44,100 Hz.
	// There are only three sounds above 44,100 (all in Creatures Village, at
	// 48,000 Hz).
	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, SDLMIXERBACKEND_CHUNK_SIZE) < 0)
		throw Exception(std::string("SDL_mixer error during sound initialization: ") + Mix_GetError());

	if (!Mix_Init(MIX_INIT_MID)) {
		printf("* SDLMixer: failed to load MIDI support: %s\n", Mix_GetError());
	}

	Mix_ChannelFinished([](int i) {
		// free the chunk we just played
		// DANGEROUS: each channel must use its own chunk! if you want channels to share data,
		// create new chunks each time using Mix_QuickLoad_RAW(chunk->abuf, chunk->alen)
		Mix_Chunk* chunk = Mix_GetChunk(i);
		if (chunk) {
			Mix_FreeChunk(chunk);
		}
		// track which handles we've given out are still valid
		if ((unsigned int)i >= s_channel_to_generation.size()) {
			printf("* SDLMixer: channel %i finished that we weren't tracking, this shouldn't happen\n", i);
		} else {
			s_channel_to_generation[i]++;
		}
	});
}

void SDLMixerBackend::shutdown() {
	if (arbitrary_audio_chunk) {
		Mix_FreeChunk(arbitrary_audio_chunk);
		arbitrary_audio_chunk = nullptr;
	}
	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

static int playChannel(Mix_Chunk* chunk, bool looping) {
	int channel = Mix_PlayChannel(-1, chunk, (looping ? -1 : 0));
	if (channel == -1) {
		Mix_AllocateChannels(Mix_AllocateChannels(-1) + 1);
		channel = Mix_PlayChannel(-1, chunk, (looping ? -1 : 0));
	}
	if (channel == -1) {
		printf("Couldn't play source: %s\n", Mix_GetError());
		return -1;
	}
	if ((unsigned int)channel >= s_channel_to_generation.size()) {
		SDL_LockAudio();
		s_channel_to_generation.resize(channel + 1, 0);
		SDL_UnlockAudio();
	}
	return channel;
}

AudioChannel SDLMixerBackend::playClip(const std::string& filename, bool looping) {
	Mix_Chunk* chunk = Mix_LoadWAV(filename.c_str());
	if (!chunk)
		return {};

	int channel = playChannel(chunk, looping);
	if (channel == -1) {
		Mix_FreeChunk(chunk);
		return {};
	}
	return int_to_audio_channel(channel);
}

AudioChannel SDLMixerBackend::playWavData(const uint8_t* data, size_t size, bool looping) {
	Mix_Chunk* chunk = Mix_LoadWAV_RW(SDL_RWFromConstMem(data, size), SDL_TRUE);
	if (!chunk) {
		return {};
	}

	int channel = playChannel(chunk, looping);
	if (channel == -1) {
		Mix_FreeChunk(chunk);
		return {};
	}
	return int_to_audio_channel(channel);
}

AudioChannel SDLMixerBackend::playStream(AudioStream* stream) {
	static std::array<uint8_t, SDLMIXERBACKEND_CHUNK_SIZE * 4> arbitrary_audio_data;

	Mix_Chunk* arbitrary_audio_chunk = Mix_QuickLoad_RAW(arbitrary_audio_data.data(), arbitrary_audio_data.size());
	if (!arbitrary_audio_chunk) {
		return {};
	}

	int channel = playChannel(arbitrary_audio_chunk, true);
	if (channel == -1) {
		return {};
	}
	Mix_RegisterEffect(
		channel, [](int channel, void* buf, int length_in_bytes, void* udata) {
			(void)channel;
			AudioStream* stream = (AudioStream*)udata;
			stream->produce(buf, length_in_bytes);
		},
		NULL, stream);
	return int_to_audio_channel(channel);
}

void SDLMixerBackend::setChannelVolume(AudioChannel source, float v) {
	int channel = audiochannel_to_int(source);
	if (channel == -1)
		return;
	Mix_Volume(channel, v * MIX_MAX_VOLUME);
}

void SDLMixerBackend::setChannelPan(AudioChannel source, float pan) {
	int channel = audiochannel_to_int(source);
	if (channel == -1)
		return;
	if (pan > 0) {
		pan = pan > 1 ? 1 : pan;
		Mix_SetPanning(channel, 255 * (1 - pan), 255);
	} else {
		pan = pan < -1 ? -1 : pan;
		Mix_SetPanning(channel, 255, 255 * (1 + pan));
	}
}

AudioState SDLMixerBackend::getChannelState(AudioChannel source) {
	int channel = audiochannel_to_int(source);
	if (channel != -1 && Mix_Playing(channel)) {
		return AUDIO_PLAYING;
	}
	return AUDIO_STOPPED;
}

void SDLMixerBackend::stopChannel(AudioChannel source) {
	int channel = audiochannel_to_int(source);
	if (channel == -1)
		return;
	Mix_HaltChannel(channel);
}

/* vim: set noet: */
