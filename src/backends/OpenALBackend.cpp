/*
 *  OpenALBackend.cpp
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

#include "OpenALBackend.h"
#include <alut.h>
#include <iostream>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include "SDL.h"
#include "exceptions.h"
#include "World.h"

#ifndef OPENAL_SUPPORT
#error OPENAL_SUPPORT isn't set, so this file shouldn't be being compiled
#endif

// seconds
#define BUFFER_LEN 0.25

#ifndef NDEBUG
#define CHECK_BACKEND_LIFE do { this->backend(); } while(0)
#else
#define CHECK_BACKEND_LIFE do { } while(0)
#endif

const ALfloat zdist = -1.0;
const ALfloat plnemul = 0.0;
const ALfloat scale = 1.0/100;

static const ALfloat null_vec[] = { 0.0, 0.0, 0.0 };

static std::string al_error_str(ALenum err) {
	switch (err) {
		case AL_NO_ERROR:
			return std::string("No error");
		case AL_INVALID_NAME:
			return std::string("Invalid name");
		case AL_INVALID_ENUM:
			return std::string("Invalid enum");
		case AL_INVALID_VALUE:
			return std::string("Invalid value");
		case AL_INVALID_OPERATION:
			return std::string("Invalid operation");
		case AL_OUT_OF_MEMORY:
			return std::string("Out of memory (we're crashing soon)");
		default:
			return boost::str(boost::format("Unknown error %d") % (int)err);
	}
}

static void al_throw_maybe() {
	ALenum err = alGetError();
	if (err == AL_NO_ERROR) return;
	throw creaturesException(boost::str(boost::format("OpenAL error (%d): %s") % err % al_error_str(err)));
}

void OpenALBackend::init() {
	alGetError();
	alutGetError();

	if (!alutInitWithoutContext(NULL, NULL)) {
		ALenum err = alutGetError();
		throw creaturesException(boost::str(
			boost::format("Failed to init OpenAL/ALUT: %s") % alutGetErrorString(err)
		));
	}

	// It seems that relying on hardware drivers is an awful idea - always use software if it's available.
	device = alcOpenDevice("Generic Software");
	if (!device) {
		device = alcOpenDevice(NULL);
		if (!device) {
			ALenum err = alGetError();
			throw creaturesException(boost::str(boost::format("OpenAL error (%d): %s") % err % al_error_str(err)));
		}
	}
	context = alcCreateContext(device, NULL);
	if (!context) {
		ALenum err = alGetError();
		throw creaturesException(boost::str(boost::format("OpenAL error (%d): %s") % err % al_error_str(err)));
	}
	alcMakeContextCurrent(context);

	setMute(false);

	static const ALfloat init_ori[6] = {
		0.0, 0.0, 1.0, /* at */
		0.0, -1.0, 0.0  /* up */
	};
	static const ALfloat init_vel[3] = {
		0.0, 0.0, 0.0
	};
	static const ALfloat init_pos[3] = {
		0.0, 0.0, 0.0
	};
	memcpy((void *)ListenerOri, (void *)init_ori, sizeof init_ori);
	memcpy((void *)ListenerVel, (void *)init_vel, sizeof init_vel);
	memcpy((void *)ListenerPos, (void *)init_pos, sizeof init_pos);

	ListenerPos[2] = zdist;
	updateListener();
}

void OpenALBackend::updateListener() {
	alListenerfv(AL_POSITION, ListenerPos);
	alListenerfv(AL_VELOCITY, ListenerVel);
	alListenerfv(AL_ORIENTATION, ListenerOri);
}

void OpenALBackend::setViewpointCenter(float x, float y) {
	ListenerPos[0] = x * scale;
	ListenerPos[1] = y * scale;
	updateListener();
}

void OpenALBackend::shutdown() {
	using namespace boost::lambda;

	bgmSource.reset();

	OpenALSource::SourceList sl = activeSources;
	std::for_each(sl.begin(), sl.end(), boost::lambda::bind(&OpenALSource::forceCleanup, *boost::lambda::_1));

	// beware, forceCleanup will release most(/all?) buffers
	OpenALBuffer::BufferList bl = activeBuffers;
	std::for_each(bl.begin(), bl.end(), boost::lambda::bind(&OpenALBuffer::destroy, *boost::lambda::_1));
	
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	alutExit();
}

void OpenALBackend::setMute(bool m) {
	if (m)
		alListenerf(AL_GAIN, 0.0f);
	else
		alListenerf(AL_GAIN, 1.0f);
	muted = m;
}

void OpenALSource::setFollowingView(bool f) {
	boost::shared_ptr<OpenALBackend> bp = backend();
	if (f == followview)
		return; // nothing to do
	if (f) {
		setPos(0, 0, 0);
		alSourcei(source, AL_SOURCE_RELATIVE, 1);
	} else {
		setPos(bp->ListenerPos[0], bp->ListenerPos[1], bp->ListenerPos[2]);
		alSourcei(source, AL_SOURCE_RELATIVE, 0);
	}
	followview = f;
}

boost::shared_ptr<AudioSource> OpenALBackend::newSource() {
	return boost::shared_ptr<AudioSource>(new OpenALSource(shared_from_this()));
}

AudioClip OpenALBackend::loadClip(const std::string &filename) {
	std::string fname = world.findFile(std::string("Sounds/") + filename + ".wav");
	if (fname.size() == 0) return AudioClip();

	alGetError();
	ALuint buf = alutCreateBufferFromFile(fname.c_str());
	if (!buf) {
		ALenum err = alutGetError();
		throw creaturesException(boost::str(
					boost::format("Failed to load %s: %s") % fname % alutGetErrorString(err)
					));
	}

	AudioClip clip(new OpenALBuffer(shared_from_this(), buf));
	return clip;
}

void OpenALBackend::begin() {
	alcSuspendContext(alcGetCurrentContext());
}

void OpenALBackend::commit() {
	alcProcessContext(alcGetCurrentContext());
}

void OpenALSource::forceCleanup() {
	boost::shared_ptr<OpenALBackend> bp = backend_weak.lock();
	if (!bp)
		return;
	bp->activeSources.erase(slit);
	stop();
	alDeleteSources(1, &source);
	bp.reset();
	clip = NULL;
	stream.reset();
}

boost::shared_ptr<class OpenALBackend> OpenALSource::backend() const {
	boost::shared_ptr<OpenALBackend> bp = backend_weak.lock();
	if (!bp)
		throw creaturesException("Attempted to manipulate a source on a destroyed backend");
	return bp;
}

OpenALSource::OpenALSource(boost::shared_ptr<class OpenALBackend> backend) {
	assert(backend);
	this->backend_weak = backend;
	alGetError();
	alGenSources(1, &source);
	alSourcef(source, AL_PITCH, 1.0f);
	alSourcef(source, AL_GAIN, 1.0f);
	alSourcefv(source, AL_POSITION, null_vec);
	x = y = z = 0;
	alSourcefv(source, AL_VELOCITY, null_vec);
	alSourcei(source, AL_LOOPING, 0);
	al_throw_maybe();
	slit = backend->activeSources.insert(backend->activeSources.end(), this);
}

OpenALBuffer::OpenALBuffer(boost::shared_ptr<class OpenALBackend> bp, ALuint handle) {
	assert(bp);
	backend = bp;
	buffer = handle;

	blit = bp->activeBuffers.insert(bp->activeBuffers.end(), this);
}

void OpenALBuffer::destroy() {
	boost::shared_ptr<OpenALBackend> bp = backend.lock();
	if (bp) {
		alDeleteBuffers(1, &buffer);
		bp->activeBuffers.erase(blit);
		bp.reset();
	}
}

void OpenALBuffer::checkLife() const {
#ifndef NDEBUG
	if (!backend.lock()) {
		throw creaturesException("Attempted to manipulate a buffer on a destroyed backend");
	}
#endif
}

OpenALBuffer::~OpenALBuffer() {
	destroy();
}

unsigned int OpenALBuffer::length_samples() const {
	ALint bits;
	ALint channels;
	ALint size;
	checkLife();
	alGetBufferi(buffer, AL_BITS, &bits);
	alGetBufferi(buffer, AL_CHANNELS, &channels);
	alGetBufferi(buffer, AL_SIZE, &size);
	return size / (bits / 8 * channels);
}

unsigned int OpenALBuffer::length_ms() const {
	ALint freq;
	checkLife();
	alGetBufferi(buffer, AL_FREQUENCY, &freq);
	return length_samples() * 1000 / freq;
}

AudioClip OpenALSource::getClip() const {
	AudioClip clip(static_cast<AudioBuffer *>(this->clip.get()));
	return clip;
}

void OpenALSource::setClip(const AudioClip &clip_) {
	CHECK_BACKEND_LIFE; // make sure we're alive
	OpenALBuffer *obp = dynamic_cast<OpenALBuffer *>(clip_.get());
	assert(obp);
	stop();
	this->stream = AudioStream();
	this->clip = OpenALClip(obp);
	alSourcei(source, AL_BUFFER, clip->buffer);
}

AudioStream OpenALSource::getStream() const {
	return stream;
}

void OpenALSource::setStream(const AudioStream &stream) {
	CHECK_BACKEND_LIFE; // make sure we're alive
	assert(stream);
	assert(stream->bitDepth() == 8 || stream->bitDepth() == 16);
	stop();
	this->clip = OpenALClip();
	this->stream = stream;
}

SourceState OpenALSource::getState() const {
	int state;
	CHECK_BACKEND_LIFE; // make sure we're alive
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	switch (state) {
		case AL_INITIAL: case AL_STOPPED: return SS_STOP;
		case AL_PLAYING: return SS_PLAY; /* XXX: AL_INITIAL? */
		case AL_PAUSED:  return SS_PAUSE;
		default:
		{
			std::cerr << "Unknown openal state, stopping stream: " << state << std::endl;
			const_cast<OpenALSource *>(this)->stop();
			return SS_STOP;
		}
	}
}

void OpenALSource::play() {
	assert( (!!clip) != (!!stream) ); // clip OR stream, not both, not neither
	CHECK_BACKEND_LIFE; // make sure we're alive
	if (stream) {
		streambuffers.clear();
		unusedbuffers.clear();
		buf_est_ms = 0;
		drain = false;
		backend()->startPolling(this);
		bufferdata();
	}
	alSourcePlay(source);
}

bool OpenALSource::poll() {
	if (!stream)
		return false;
	return bufferdata();
}

static ALuint audioFormat(bool stereo, int bitdepth) {
	switch (bitdepth | !!stereo) {
		case 8: return AL_FORMAT_MONO8;
		case 16: return AL_FORMAT_MONO16;
		case 9: return AL_FORMAT_STEREO8;
		case 17: return AL_FORMAT_STEREO16;
		default: assert(!"Impossible");
	}
	abort();
}

OpenALStreamBuf::OpenALStreamBuf(int freq, int bitDepth, bool stereo) {
	this->freq = freq;
	this->stereo = stereo;
	this->bitDepth = bitDepth;
	this->format = audioFormat(stereo, bitDepth);
	alGenBuffers(1, &bufferID);
}

OpenALStreamBuf::~OpenALStreamBuf() {
	alDeleteBuffers(1, &bufferID);
}

static size_t bytesPerSample(bool stereo, int bitdepth) {
	return (bitdepth / 8) << !!stereo;
}

void OpenALStreamBuf::writeAudioData(const void *data, size_t len) {
	alBufferData(bufferID, format, data, len, freq);
	approx_ms = ((1000 * len) / bytesPerSample(stereo, bitDepth)) / freq;
}

bool OpenALSource::bufferdata() {
	static unsigned char tempbuf[8192];
	unsigned int donebuffers;

	alGetSourcei(source, AL_BUFFERS_PROCESSED, (ALint *)&donebuffers);

	assert(streambuffers.size() >= donebuffers);

	ALuint *bufferNames = new ALuint[donebuffers];
	alSourceUnqueueBuffers(source, donebuffers, bufferNames);

	for (unsigned int i = 0; i < donebuffers && !streambuffers.empty(); i++) {
		OpenALStreamBufP p = streambuffers.front();
		assert(p->bufferID == bufferNames[i]);
		buf_est_ms -= p->approxLen();
		unusedbuffers.push_back(streambuffers.front());
		streambuffers.pop_front();
	}

	delete [] bufferNames;


	int buf_goal = stream->latency();
	std::vector<ALuint> newbuffers;
	while (!drain && (buf_est_ms < buf_goal || streambuffers.empty())) {
		size_t result = stream->produce(tempbuf, sizeof tempbuf);
		if (result) {
			OpenALStreamBufP buf;
			if (unusedbuffers.empty())
				buf = boost::shared_ptr<OpenALStreamBuf>(new OpenALStreamBuf(stream->sampleRate(), stream->bitDepth(), stream->isStereo()));
			else {
				buf = unusedbuffers.back();
				unusedbuffers.pop_back();
			}
			buf->writeAudioData(tempbuf, sizeof tempbuf);
			buf_est_ms += buf->approxLen();
			streambuffers.push_back(buf);
			newbuffers.push_back(buf->bufferID);
		}
		if (result < sizeof tempbuf) {
			if (!isLooping() || !stream->reset()) {
				// shut down
				drain = true;
			}
		}
	}
	if (newbuffers.size()) alSourceQueueBuffers(source, newbuffers.size(), &newbuffers[0]);

	if (streambuffers.empty() && drain) {
		stop();
		return false;
	}
	if (getState() != SS_PLAY) alSourcePlay(source); // make sure we kick-start a drained stream
	return true;
}

void OpenALSource::stop() {
	CHECK_BACKEND_LIFE;
	alSourceStop(source);
	alSourcei(source, AL_BUFFER, NULL); // remove all queued buffers

	if (stream) {
		streambuffers.clear();
		unusedbuffers.clear();
		backend()->stopPolling(this);
	}
}

void OpenALSource::pause() {
	CHECK_BACKEND_LIFE;
	alSourcePause(source);
}

static void fadeSource(boost::weak_ptr<AudioSource> s) {
	int fadeout = 500; // TODO: good value?

	/* We adjust gain every 10ms until we reach fadeout ms. */
	const int steps = fadeout / 10;
	const float adjust = 1.0 / (float)steps;
	for (int i = 0; i < steps; i++) {
		boost::shared_ptr<AudioSource> p = s.lock();
		if (!p) return;
		p->setVolume(1.0f - adjust * (float)i);
		SDL_Delay(10); // XXX: do we have some other portable sleeping primitive we can use?
		// boost::thread::sleep is a bit iffy, it seems
	}
	boost::shared_ptr<AudioSource> p = s.lock();
	if (p)
		p->stop();
}

void OpenALSource::fadeOut() {
	CHECK_BACKEND_LIFE;
	boost::thread th(boost::lambda::bind<void>(fadeSource, shared_from_this()));
}

void OpenALSource::realSetPos(float x, float y, float plane) {
	CHECK_BACKEND_LIFE;
	if (this->x == x && this->y == y && this->z == plane) return;
	this->SkeletonAudioSource::setPos(x, y, plane);
	alSource3f(source, AL_POSITION, x * scale, y * scale, plane * plnemul);
}

void OpenALSource::setPos(float x, float y, float plane) {
	CHECK_BACKEND_LIFE;
	if (isFollowingView())
		return;
	realSetPos(x, y, plane);
}

void OpenALSource::setVelocity(float x, float y) {
	CHECK_BACKEND_LIFE;
	// experiment with this later
	x = y = 0;
	alSource3f(source, AL_VELOCITY, x * scale, y * scale, 0);
}

bool OpenALSource::isLooping() const {
	int l;
	CHECK_BACKEND_LIFE;
	alGetSourcei(source, AL_LOOPING, &l);
	return l == AL_TRUE;
}

void OpenALSource::setLooping(bool l) {
	CHECK_BACKEND_LIFE;
	alSourcei(source, AL_LOOPING, l ? AL_TRUE : AL_FALSE);
}

void OpenALSource::setVolume(float v) {
	CHECK_BACKEND_LIFE;
	if (v == this->volume) return;
	this->SkeletonAudioSource::setVolume(v);
	alSourcef(source, AL_GAIN, getEffectiveVolume());
}

void OpenALSource::setMute(bool m) {
	CHECK_BACKEND_LIFE;
	if (m == muted) return;
	this->SkeletonAudioSource::setMute(m);
	alSourcef(source, AL_GAIN, getEffectiveVolume());
}

void OpenALBackend::poll() {
	std::map<OpenALSource *, boost::weak_ptr<AudioSource> >::iterator it, next;
	it = pollingSrcs.begin();

	for (; it != pollingSrcs.end(); it = next) {
		next = it; next++;
		boost::shared_ptr<AudioSource> p = it->second.lock();
		if (!p) {
			pollingSrcs.erase(it);
			continue;
		}
		OpenALSource *src_p = it->first;
		assert(src_p == dynamic_cast<OpenALSource *>(p.get()));
		if (!src_p->poll())
			pollingSrcs.erase(it);
	}
}

void OpenALBackend::startPolling(OpenALSource *src_p) {
	pollingSrcs[src_p] = boost::weak_ptr<AudioSource>(src_p->shared_from_this());
}

void OpenALBackend::stopPolling(OpenALSource *src_p) {
	pollingSrcs.erase(src_p);
}

class OpenALBGMSource : public OpenALSource {
	protected:
		bool init;
		friend class OpenALBackend;
		OpenALBGMSource(boost::shared_ptr<OpenALBackend> b)
			: OpenALSource(b)
		{
			init = true;
			OpenALSource::setFollowingView(true);
			init = false;
		}

		void illegalOp() {
			if (!init)
				throw creaturesException("Attempted illegal operation on BGM source");
		}
	public:
		virtual void setPos(float x, float y, float z) {
			illegalOp();
			OpenALSource::setPos(x,y,z);
		}
		virtual void setFollowingView(bool f) {
			if (!f)
				illegalOp();
			OpenALSource::setFollowingView(f);
		}
		virtual void setVelocity(float xv, float yv) {
			illegalOp();
			OpenALSource::setVelocity(xv, yv);
		}
};

boost::shared_ptr<AudioSource> OpenALBackend::getBGMSource() {
	if (!bgmSource) {
		bgmSource = boost::shared_ptr<AudioSource>(
				new OpenALBGMSource(
					shared_from_this()
				)
			);
	}
	return bgmSource;
}
/* vim: set noet: */
