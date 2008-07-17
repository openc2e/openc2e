/*
 *  SkeletonAudioBackend.h
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

#ifndef SKELETONAUDIOBACKEND_H
#define SKELETONAUDIOBACKEND_H 1

#include "AudioBackend.h"

/* This header exists to provide a skeleton implementation for new
 * audio backends. Normally client code should not include it.
 */

class SkeletonAudioBuffer : public AudioBuffer {
protected:
	int refcnt;
	SkeletonAudioBuffer() { refcnt = 0; }
	friend class NullBackend;
	void add_ref() { refcnt++; }
	void del_ref() { assert(refcnt); refcnt--; if (!refcnt) delete this; }
};

class SkeletonAudioSource : public AudioSource {
protected:
	AudioClip clip;
	/* we don't track state here because, well, the clips don't support
	 * length values yet.
	 */
	bool looping, muted, followview;
	float x, y, z, volume;
	SkeletonAudioSource() { looping = false; muted = false; volume = 1.0f; followview = false; }

public:
	AudioClip getClip() { return clip; }
	void setClip(AudioClip &ac) {
		assert(getState() == SS_STOP);
		clip = ac;
	}
	SourceState getState() const {
		return SS_STOP;
	}
	void play() { assert(clip); }
	void stop() { }
	void pause() { }
	void fadeOut() { }
	void setPos(float x_, float y_, float z_) { x = x_; y = y_; z = z_; }
	void getPos(float &x_, float &y_, float &z_) const { x_ = x; y_ = y; z_ = z; }
	void setVelocity(float, float) { }
	bool isLooping() const { return looping; }
	void setLooping(bool l) { looping = l; }
	void setVolume(float v) { volume = v; }
	float getVolume() const { return volume; }
	bool isMuted() const { return muted; }
	void setMute(bool m) { muted = m; }
	bool isFollowingView() const { return followview; }
	void setFollowingView(bool v) { followview = v; }

};

#endif

/* vim: set noet: */
