/*
 *  SDLMixerBackend.h
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

#pragma once

#include "common/audio/AudioBackend.h"
#include "common/singleton.h"

#include <memory>
#include <vector>

typedef struct _Mix_Music Mix_Music;

class SDLMixerBackend : public AudioBackend, public Singleton<SDLMixerBackend> {
  private:
	friend Singleton<SDLMixerBackend>;
	SDLMixerBackend();
	Mix_Music* midi = nullptr;

  public:
	void init();
	void shutdown();

	AudioChannel play_clip(const std::string& filename, bool looping = false);
	AudioChannel play_wav_data(const uint8_t* data, size_t size, bool looping = false);

	void audio_channel_set_volume(AudioChannel, float);
	void audio_channel_set_pan(AudioChannel, float pan);
	AudioState audio_channel_get_state(AudioChannel);
	void audio_channel_stop(AudioChannel);

	void play_midi_file(const std::string& filename);
	void midi_set_volume(float);
	void midi_stop();
};