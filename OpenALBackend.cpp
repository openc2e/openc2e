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
#include <AL/alut.h>
#include <iostream>
#include <boost/format.hpp>
#include <boost/thread.hpp>
#include <boost/lambda/bind.hpp>
#include "SDL.h"
#include "exceptions.h"
#include "World.h"

#ifndef OPENAL_SUPPORT
#error OPENAL_SUPPORT isn't set, so this file shouldn't be being compiled
#endif

const ALfloat zdist = -1.0;
const ALfloat plnemul = 0.0;
const ALfloat scale = 1.0/50;

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

	if (!alutInit(NULL, NULL)) {
		ALenum err = alutGetError();
		throw creaturesException(boost::str(
			boost::format("Failed to init OpenAL/ALUT: %s") % alutGetErrorString(err)
		));
	}

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
	alutExit();
}

void OpenALBackend::setMute(bool m) {
	if (m)
		alListenerf(AL_GAIN, 0.0f);
	else
		alListenerf(AL_GAIN, 1.0f);
	muted = m;
}

boost::shared_ptr<AudioSource> OpenALBackend::newSource() {
	return boost::shared_ptr<AudioSource>(new OpenALSource(shared_from_this()));
}

AudioClip OpenALBackend::loadClip(const std::string &filename) {
	std::string fname = world.findFile(std::string("/Sounds/") + filename + ".wav");
	if (fname.size() == 0) return AudioClip();

	alGetError();
	ALuint buf = alutCreateBufferFromFile(fname.c_str());
	if (!buf) {
		ALenum err = alGetError();
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

OpenALSource::OpenALSource(boost::shared_ptr<class OpenALBackend> backend) {
	this->backend = backend;
	alGetError();
	alGenSources(1, &source);
	alSourcef(source, AL_PITCH, 1.0f);
	alSourcef(source, AL_GAIN, 1.0f);
	alSourcefv(source, AL_POSITION, null_vec);
	alSourcefv(source, AL_VELOCITY, null_vec);
	alSourcei(source, AL_LOOPING, 0);
	al_throw_maybe();
}

OpenALBuffer::OpenALBuffer(boost::shared_ptr<class OpenALBackend> backend, ALuint handle) {
	this->backend = backend;
	buffer = handle;
}

unsigned int OpenALBuffer::length_samples() {
	ALint bits;
	ALint channels;
	ALint size;
	alGetBufferi(buffer, AL_BITS, &bits);
	alGetBufferi(buffer, AL_CHANNELS, &channels);
	alGetBufferi(buffer, AL_SIZE, &size);
	return size / (bits / 8 * channels);
}

unsigned int OpenALBuffer::length_ms() {
	ALint freq;
	alGetBufferi(buffer, AL_FREQUENCY, &freq);
	return length_samples() * 1000 / freq;
}

AudioClip OpenALSource::getClip() {
	AudioClip clip(static_cast<AudioBuffer *>(this->clip.get()));
	return clip;
}

void OpenALSource::setClip(AudioClip &clip_) {
	OpenALBuffer *obp = dynamic_cast<OpenALBuffer *>(clip_.get());
	assert(obp);
	stop();
	this->clip = OpenALClip(obp);
	if (clip) {
		alSourcei(source, AL_BUFFER, clip->buffer);
	}
}

SourceState OpenALSource::getState() {
	int state;
	alGetSourcei(source, AL_SOURCE_STATE, &state);
	switch (state) {
		case AL_INITIAL: case AL_STOPPED: return SS_STOP;
		case AL_PLAYING: return SS_PLAY; /* XXX: AL_INITIAL? */
		case AL_PAUSED:  return SS_PAUSE;
		default:
		{
			std::cerr << "Unknown openal state, stopping stream: " << state << std::endl;
			stop();
			return SS_STOP;
		}
	}
}

void OpenALSource::play() {
	assert(clip);
	alSourcePlay(source);
}

void OpenALSource::stop() {
	alSourceStop(source);
}

void OpenALSource::pause() {
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
	boost::thread th(boost::lambda::bind<void>(fadeSource, shared_from_this()));
}

void OpenALSource::setPos(float x, float y, float plane) {
	if (this->x == x && this->y == y && this->z == plane) return;
	this->SkeletonAudioSource::setPos(x, y, plane);
	alSource3f(source, AL_POSITION, x * scale, y * scale, plane * plnemul);
}

void OpenALSource::setVelocity(float x, float y) {
	// experiment with this later
	x = y = 0;
	alSource3f(source, AL_VELOCITY, x * scale, y * scale, 0);
}

bool OpenALSource::isLooping() {
	int l;
	alGetSourcei(source, AL_LOOPING, &l);
	return l == AL_TRUE;
}

void OpenALSource::setLooping(bool l) {
	alSourcei(source, AL_LOOPING, l ? AL_TRUE : AL_FALSE);
}

void OpenALSource::setVolume(float v) {
	if (v == this->volume) return;
	this->SkeletonAudioSource::setVolume(v);
	alSourcef(source, AL_GAIN, getEffectiveVolume());
}

void OpenALSource::setMute(bool m) {
	if (m == muted) return;
	this->SkeletonAudioSource::setMute(m);
	alSourcef(source, AL_GAIN, getEffectiveVolume());
}

/* vim: set noet: */
