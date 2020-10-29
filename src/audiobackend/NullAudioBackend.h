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
#include <memory>

class NullAudioSource : public AudioSource {
	SourceState getState() const {
		return SS_STOP;
	}
	void play() { }
	void stop() { }
	void pause() { }
	void fadeOut() { }
	void setPos(float, float, float) { }
	void getPos(float&, float&, float&) const { }
	void setVelocity(float, float) { }
	bool isLooping() const { return false; }
	void setLooping(bool) { }
	void setVolume(float) { }
	float getVolume() const { return 0; }
	bool isMuted() const { return true; }
	void setMute(bool) { }
	bool isFollowingView() const { return false; }
	void setFollowingView(bool) { }
	void setStream(const AudioStream&) { }
	AudioStream getStream() const { return {}; }
};

class NullAudioBackend : public AudioBackend {
protected:
	bool muted;

public:
	NullAudioBackend() { }
	void init() { muted = false; }
	void shutdown() { }
	void setViewpointCenter(float, float) { }
	void setMute(bool b) { muted = b; }
	bool isMuted() const { return muted; }

	std::shared_ptr<AudioSource> loadClip(const std::string &filename) {
		if (filename.size() == 0) return std::shared_ptr<AudioSource>();
		return std::shared_ptr<AudioSource>(new NullAudioSource());
	}
	void setBackgroundMusic(const std::string& filename) { }
	void setBackgroundMusic(AudioStream stream) { }
	void stopBackgroundMusic() { }
};
