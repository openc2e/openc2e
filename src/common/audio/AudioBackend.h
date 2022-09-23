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

/* Base class for sources of streaming data (eg, MNG music)
 *
 * produce() shall produce up to len bytes of data (to be stored in the buffer pointed to by 'data'),
 * and return however many it actually produced. If less than len bytes are produced, playback will cease
 * (and loop if applicable) once the current buffer of data is exhausted.
 *
 * The implementation should not make any assumptions about how often any of these functions
 * will be called, if at all.
 */
struct AudioStream {
	virtual ~AudioStream() {}
	virtual size_t produce(void* data, size_t len_in_bytes) = 0;
};

class AudioBackend : public std::enable_shared_from_this<AudioBackend> {
  protected:
	AudioBackend() {}

  public:
	virtual ~AudioBackend() {}
	virtual void init() = 0;
	virtual void shutdown() = 0;

	virtual AudioChannel playClip(const std::string& filename, bool looping = false) = 0;
	virtual AudioChannel playWavData(const uint8_t* data, size_t size, bool looping = false) = 0;
	virtual AudioChannel playStream(AudioStream*) = 0;

	virtual void setChannelVolume(AudioChannel, float) = 0;
	virtual void setChannelPan(AudioChannel, float pan) = 0;
	virtual AudioState getChannelState(AudioChannel) = 0;
	virtual void stopChannel(AudioChannel) = 0;

	virtual void playMIDIFile(const std::string& filename) = 0;
	virtual void setMIDIVolume(float) = 0;
	virtual void stopMIDI() = 0;
};
