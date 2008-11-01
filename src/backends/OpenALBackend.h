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

#include <al.h>
#include <alc.h>
#include <string>
#include <map>
#include <list>
#include <vector>

class OpenALBuffer;

namespace boost {
	static inline void intrusive_ptr_add_ref(OpenALBuffer *p);
	static inline void intrusive_ptr_release(OpenALBuffer *p);
}

class OpenALBuffer : public SkeletonAudioBuffer {
	friend void boost::intrusive_ptr_add_ref(OpenALBuffer *p);
	friend void boost::intrusive_ptr_release(OpenALBuffer *p);

protected:
	typedef std::list<OpenALBuffer *> BufferList;

	BufferList::iterator blit;

	boost::weak_ptr<class OpenALBackend> backend;
	OpenALBuffer(boost::shared_ptr<class OpenALBackend>, ALuint);
	friend class OpenALBackend;
	friend class OpenALSource;

	ALuint buffer;

	void destroy();
	void checkLife() const;

public:
	~OpenALBuffer();
	virtual unsigned int length_ms() const; /* milliseconds */
	virtual unsigned int length_samples() const;
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

typedef boost::shared_ptr<class OpenALStreamBuf> OpenALStreamBufP;
class OpenALStreamBuf {
	ALuint bufferID, format;
	int freq, bitDepth, approx_ms;
	bool stereo;
	OpenALStreamBuf(int freq, int bitDepth, bool stereo);

	int approxLen() const { return approx_ms; }
	friend class OpenALSource;
public:
	~OpenALStreamBuf();

	void writeAudioData(const void *data, size_t len);
};

class OpenALSource : public SkeletonAudioSource {
protected:
	typedef std::list<OpenALSource *> SourceList;

	SourceList::iterator slit;

	boost::weak_ptr<class OpenALBackend> backend_weak;
	boost::shared_ptr<class OpenALBackend> backend() const;
	OpenALSource(boost::shared_ptr<class OpenALBackend>);
	friend class OpenALBackend;

	OpenALClip clip;
	AudioStream stream;
	ALuint source;

	bool poll();
	bool bufferdata();

	std::list<OpenALStreamBufP> streambuffers;
	std::vector<OpenALStreamBufP> unusedbuffers;
	int buf_est_ms;
	bool drain;
	void realSetPos(float x, float y, float plane);

	void forceCleanup();
public:
	~OpenALSource() { forceCleanup(); stop(); alDeleteSources(1, &source); }

	virtual AudioClip getClip() const;
	virtual void setClip(const AudioClip &); /* Valid only in STOP state */
	virtual SourceState getState() const;

	AudioStream getStream() const;
	void setStream(const AudioStream &);

	virtual void play(); /* requires that getClip() not be a null ref */
	virtual void stop();
	virtual void pause();
	virtual void fadeOut();
	virtual void setPos(float x, float y, float plane);
	virtual void setVelocity(float x, float y);
	virtual bool isLooping() const;
	virtual void setLooping(bool);
	virtual void setVolume(float v);
	virtual void setMute(bool);
	virtual void setFollowingView(bool);
};

class OpenALBackend : public AudioBackend {
protected:
	ALfloat ListenerOri[6];
	ALfloat ListenerVel[3];
	ALfloat ListenerPos[3];
	bool muted;

	// these are weak to avoid a reference loop
	std::map<OpenALSource *, boost::weak_ptr<AudioSource> > followingSrcs;
	std::map<OpenALSource *, boost::weak_ptr<AudioSource> > pollingSrcs;
	friend class OpenALSource;
	friend class OpenALBuffer;

	OpenALSource::SourceList activeSources;
	OpenALBuffer::BufferList activeBuffers;

	ALCdevice *device;
	ALCcontext *context;

	void startPolling(OpenALSource *);
	void stopPolling(OpenALSource *);
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
	bool isMuted() const { return muted; }

	boost::shared_ptr<AudioSource> newSource();
	boost::shared_ptr<AudioSource> getBGMSource() {
		// STUB
		return boost::shared_ptr<AudioSource>();
	}
	AudioClip loadClip(const std::string &filename);

	void begin();
	void commit();
	void poll();
};

#endif

/* vim: set noet: */
