/*
 *  AudioBackend.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Sun Aug 12 2007.
 *  Copyright (c) 2007 Bryan Donlan. All rights reserved.
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

#include "AudioChannel.h"
#include "AudioState.h"

#include <memory>
#include <string>

class AudioBackend : public std::enable_shared_from_this<AudioBackend> {
  protected:
	AudioBackend() {}

  public:
	virtual ~AudioBackend() {}
	virtual void init() = 0;
	virtual void shutdown() = 0;

	virtual AudioChannel play_clip(const std::string& filename, bool looping = false) = 0;
	virtual AudioChannel play_wav_data(const uint8_t* data, size_t size, bool looping = false) = 0;

	virtual void audio_channel_set_volume(AudioChannel, float) = 0;
	virtual void audio_channel_set_pan(AudioChannel, float pan) = 0;
	virtual void audio_channel_fade_out(AudioChannel, int32_t milliseconds) = 0;
	virtual AudioState audio_channel_get_state(AudioChannel) = 0;
	virtual void audio_channel_stop(AudioChannel) = 0;

	virtual void play_midi_file(const std::string& filename) = 0;
	virtual void midi_set_volume(float) = 0;
	virtual void midi_stop() = 0;
};
