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

#ifndef SOUNDBACKEND_H
#define SOUNDBACKEND_H 1

#include <boost/intrusive_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <string>

class AudioBuffer {
	friend void intrusive_ptr_add_ref(AudioBuffer *p) {
		p->add_ref();
	}
	friend void intrusive_ptr_release(AudioBuffer *p) {
		p->del_ref();
	}

protected:
	virtual void add_ref() = 0;
	virtual void del_ref() = 0;

public:
	virtual ~AudioBuffer() { }
	virtual unsigned int length_ms() const = 0; /* milliseconds */
	virtual unsigned int length_samples() const = 0;
};

typedef boost::intrusive_ptr<AudioBuffer> AudioClip;

/* Base class for sources of streaming data (eg, MNG music)
 *
 * The values of isStereo, bitDepth, and preBufferSize MUST remain the same for
 * the life of an instance.
 *
 * produce() shall produce up to len bytes of data (to be stored in the buffer pointed to by 'data'),
 * and return however many it actually produced. If less than len bytes are produced, playback will cease
 * (and loop if applicable) once the current buffer of data is exhausted.
 *
 * The implementation should not make any assumptions about how often any of these functions
 * will be called, if at all.
 */
typedef boost::shared_ptr<struct AudioStreamBase> AudioStream;
struct AudioStreamBase {
	virtual ~AudioStreamBase() { }

	// Stereo sources will be played without spatial features
	// left then right format
	virtual bool isStereo() const = 0;
	// Must be 8 or 16
	virtual int bitDepth() const = 0;
	virtual int sampleRate() const = 0;
	// The backend will attempt to maintain an internal buffer of approximately
	// this many milliseconds of audio data.
	virtual int latency() const = 0;

	virtual size_t produce(void *data, size_t len) = 0;

	// Called on a loop; return true on success or false to halt playback anyway
	virtual bool reset() = 0;
};

enum SourceState { SS_STOP, SS_PLAY, SS_PAUSE };

class AudioSource : public boost::enable_shared_from_this<AudioSource> {
protected:
	AudioSource() { }

public:
	virtual ~AudioSource() { }
	virtual AudioClip getClip() = 0;
	virtual void setClip(const AudioClip &) = 0; /* Valid only in STOP state */
	virtual SourceState getState() const = 0;

	// The effect of calling this function on a AudioSource initialized with setClip
	// is undefined
	virtual AudioStream getStream() const = 0;
	virtual void setStream(const AudioStream &) = 0;
	// Convenience function for setting to a new AudioStream.
	// Ownership passes to AudioSource (or rather, AudioStream's reference counting)
	virtual void setStream(AudioStreamBase *p) {
		setStream(AudioStream(p));
	}

	virtual void play() = 0; /* requires that getClip() not be a null ref */
	virtual void stop() = 0;
	virtual void pause() = 0;
	virtual void fadeOut() = 0;
	virtual void setPos(float x, float y, float plane) = 0;
	virtual void getPos(float &x, float &y, float &plane) const = 0;
	virtual void setVelocity(float x, float y) = 0;
	virtual bool isLooping() const = 0;
	virtual void setLooping(bool) = 0;
	virtual void setVolume(float vol) = 0;
	virtual float getVolume() const = 0;
	float getEffectiveVolume() const { return isMuted() ? 0 : getVolume(); }
	virtual bool isMuted() const = 0;
	virtual void setMute(bool) = 0;

	// Set true to lock the location of this source to the viewpoint
	virtual bool isFollowingView() const = 0;
	virtual void setFollowingView(bool) = 0;
};

class AudioBackend : public boost::enable_shared_from_this<AudioBackend> {
protected:
	AudioBackend() { }

public:
	virtual void init() = 0;
	virtual void shutdown() = 0;
	virtual ~AudioBackend() { }
	virtual void setViewpointCenter(float x, float y) = 0;
	virtual void setMute(bool) = 0;
	virtual bool isMuted() const = 0;

	/* TODO: global vol controls */
	virtual boost::shared_ptr<AudioSource> newSource() = 0;
	/* Returns an AudioSource with a fixed position relative to the viewpoint.
	 *
	 * The effect of invoking setPos on this source is undefined.
	 */
	virtual boost::shared_ptr<AudioSource> getBGMSource() = 0;
	virtual AudioClip loadClip(const std::string &filename) = 0;

	virtual void begin() { }
	virtual void commit() { }
	virtual void poll() { }
};

#endif

/* vim: set noet: */
