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

#ifndef _SDLMIXERBACKEND_H
#define _SDLMIXERBACKEND_H

#include "SkeletonAudioBackend.h"

#include "SDL_mixer.h"

class SDLMixerBuffer;

namespace boost {
	static inline void intrusive_ptr_add_ref(SDLMixerBuffer *p);
	static inline void intrusive_ptr_release(SDLMixerBuffer *p);
}

class SDLMixerBuffer : public SkeletonAudioBuffer {
	friend void boost::intrusive_ptr_add_ref(SDLMixerBuffer *p);
	friend void boost::intrusive_ptr_release(SDLMixerBuffer *p);

protected:
	SDLMixerBuffer(Mix_Chunk *);

	friend class SDLMixerBackend;
	friend class SDLMixerSource;

	Mix_Chunk *buffer;

public:
	~SDLMixerBuffer();
	virtual unsigned int length_ms() const; /* milliseconds */
	virtual unsigned int length_samples() const;
};

typedef boost::intrusive_ptr<SDLMixerBuffer> SDLMixerClip;
namespace boost {
	static inline void intrusive_ptr_add_ref(SDLMixerBuffer *p) {
		p->add_ref();
	}
	static inline void intrusive_ptr_release(SDLMixerBuffer *p) {
		p->del_ref();
	}
}

class SDLMixerSource : public SkeletonAudioSource {
protected:
	SDLMixerSource();
	
	friend class SDLMixerBackend;
	
	SDLMixerClip clip;
	int channel;

public:
	~SDLMixerSource();

	virtual AudioClip getClip() const;
	virtual void setClip(const AudioClip &);
	virtual SourceState getState() const;
	virtual void play();
	virtual void stop();
	virtual void fadeOut();
	virtual void setPos(float x, float y, float plane);
	virtual bool isLooping() const;
	virtual void setLooping(bool);
	virtual void setVolume(float v);
	virtual void setMute(bool);
	virtual void setFollowingView(bool);

	virtual AudioStream getStream() const {
		// STUB
		return AudioStream();
	}
	virtual void setStream(const AudioStream &) {
		// STUB
	}
};

class SDLMixerBackend : public AudioBackend {
protected:
	bool muted;

public:
	SDLMixerBackend() { }
	void init();
	void shutdown();

	void setViewpointCenter(float, float);
	void setMute(bool);
	bool isMuted() const { return muted; }
	boost::shared_ptr<AudioSource> newSource();
	AudioClip loadClip(const std::string &);

	boost::shared_ptr<AudioSource> getBGMSource() {
		// STUB
		return boost::shared_ptr<AudioSource>();
	}
};

#endif

/* vim: set noet: */
