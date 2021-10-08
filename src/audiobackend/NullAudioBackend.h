/*
 *  NullAudioBackend.h
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

#include "audiobackend/AudioBackend.h"

class NullAudioBackend : public AudioBackend {
  public:
	NullAudioBackend() = default;
	void init() {}
	void shutdown() {}

	AudioChannel playClip(const std::string& filename, bool) {
		if (filename.size() == 0)
			return {};
		return {1};
	}
	virtual AudioChannel playWavData(const uint8_t*, size_t, bool) {
		return {1};
	}
	AudioChannel playStream(AudioStream*) {
		return {1};
	}

	void setChannelVolume(AudioChannel, float) {}
	void setChannelPan(AudioChannel, float) {}
	AudioState getChannelState(AudioChannel) {
		// TODO: pretend to play audio for the correct duration, so that consumers
		// of this API don't assume all sounds fail to play.
		return AUDIO_STOPPED;
	}

	void stopChannel(AudioChannel) {}

	void playMIDIFile(const std::string&) {}
	void setMIDIVolume(float) {}
	void stopMIDI() {}
};
