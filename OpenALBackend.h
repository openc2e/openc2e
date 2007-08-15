/*
 *  OpenALBackend.h
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

#ifndef _OPENALBACKEND_H
#define _OPENALBACKEND_H

#include "SkeletonAudioBackend.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <string>

class OpenALBuffer;

namespace boost {
	static inline void intrusive_ptr_add_ref(OpenALBuffer *p);
	static inline void intrusive_ptr_release(OpenALBuffer *p);
}

class OpenALBuffer : public SkeletonAudioBuffer {
	friend void boost::intrusive_ptr_add_ref(OpenALBuffer *p);
	friend void boost::intrusive_ptr_release(OpenALBuffer *p);

protected:
	boost::shared_ptr<class OpenALBackend> backend;
	OpenALBuffer(boost::shared_ptr<class OpenALBackend>, ALuint);
	friend class OpenALBackend;
	friend class OpenALSource;

	ALuint buffer;

public:
	~OpenALBuffer() { alDeleteBuffers(1, &buffer); }
	virtual unsigned int length_ms(); /* milliseconds */
	virtual unsigned int length_samples();
};

typedef boost::intrusive_ptr<OpenALBuffer> OpenALClip;
namespace boost {
	static inline void intrusive_ptr_add_ref(OpenALBuffer *p) {
		p->add_ref();
	}
	static inline void intrusive_ptr_release(OpenALBuffer *p) {
		p->del_ref();
	}
};

class OpenALSource : public SkeletonAudioSource {
protected:
	boost::shared_ptr<class OpenALBackend> backend;
	OpenALSource(boost::shared_ptr<class OpenALBackend>);
	friend class OpenALBackend;

	OpenALClip clip;
	ALuint source;

public:
	~OpenALSource() { stop(); alDeleteSources(1, &source); }

	virtual AudioClip getClip();
	virtual void setClip(AudioClip &); /* Valid only in STOP state */
	virtual SourceState getState();
	virtual void play(); /* requires that getClip() not be a null ref */
	virtual void stop();
	virtual void pause();
	virtual void fadeOut();
	virtual void setPos(float x, float y, float plane);
	virtual void setVelocity(float x, float y);
	virtual bool isLooping();
	virtual void setLooping(bool);
	virtual void setVolume(float v);
	virtual void setMute(bool);
};

class OpenALBackend : public AudioBackend {
protected:
	ALfloat ListenerOri[6];
	ALfloat ListenerVel[3];
	ALfloat ListenerPos[3];
	bool muted;

public:
	boost::shared_ptr<OpenALBackend> shared_from_this() {
		return boost::static_pointer_cast<OpenALBackend, AudioBackend>(this->AudioBackend::shared_from_this());
	}
	boost::shared_ptr<const OpenALBackend> shared_from_this() const {
		return boost::static_pointer_cast<const OpenALBackend, const AudioBackend>(this->AudioBackend::shared_from_this());
	}

	OpenALBackend() { }
	void init();
	void shutdown();
	void updateListener();
	void setViewpointCenter(float x, float y);
	void setMute(bool m);
	bool isMuted() { return muted; }

	boost::shared_ptr<AudioSource> newSource();
	AudioClip loadClip(const std::string &filename);

	void begin();
	void commit();
};

#endif

/* vim: set noet: */
