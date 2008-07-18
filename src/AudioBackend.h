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

class AudioBuffer;

namespace boost {
	static inline void intrusive_ptr_add_ref(AudioBuffer *p);
	static inline void intrusive_ptr_release(AudioBuffer *p);
};

class AudioBuffer {
	friend void boost::intrusive_ptr_add_ref(AudioBuffer *p);
	friend void boost::intrusive_ptr_release(AudioBuffer *p);

protected:
	virtual void add_ref() = 0;
	virtual void del_ref() = 0;

public:
	virtual ~AudioBuffer() { }
	virtual unsigned int length_ms() const = 0; /* milliseconds */
	virtual unsigned int length_samples() const = 0;
};

typedef boost::intrusive_ptr<AudioBuffer> AudioClip;
namespace boost {
	static inline void intrusive_ptr_add_ref(AudioBuffer *p) {
		p->add_ref();
	}
	static inline void intrusive_ptr_release(AudioBuffer *p) {
		p->del_ref();
	}
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
	virtual AudioClip loadClip(const std::string &filename) = 0;

	virtual void begin() { }
	virtual void commit() { }
};

#endif

/* vim: set noet: */
