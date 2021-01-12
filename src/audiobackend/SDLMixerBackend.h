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

#include "audiobackend/AudioBackend.h"
#include "utils/singleton.h"

#include <SDL_mixer.h>
#include <memory>
#include <vector>

class SDLMixerBackend : public AudioBackend, public Singleton<SDLMixerBackend> {
  private:
	friend Singleton<SDLMixerBackend>;
	SDLMixerBackend();
	Mix_Music* midi = nullptr;
	Mix_Chunk* arbitrary_audio_chunk = nullptr;

  public:
	void init();
	void shutdown();

	AudioChannel playClip(const std::string& filename, bool looping = false);
	AudioChannel playWavData(const uint8_t* data, size_t size, bool looping = false);
	AudioChannel playStream(AudioStream*);

	void fadeOutChannel(AudioChannel);
	void setChannelVolume(AudioChannel, float);
	void setChannelPan(AudioChannel, float pan);
	AudioState getChannelState(AudioChannel);
	void stopChannel(AudioChannel);

	void playMIDIFile(const std::string& filename);
	void setMIDIVolume(float);
	void stopMIDI();
};