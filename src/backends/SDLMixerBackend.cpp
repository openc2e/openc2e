/*
 *  SDLMixerBackend.cpp
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

#include "SDLMixerBackend.h"
#include "SDL.h"
#include "exceptions.h"
#include "World.h"

void SDLMixerBackend::init() {
	// TODO: ensure SDLBackend is in use?

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
		throw creaturesException(std::string("SDL error during sound initialization: ") + SDL_GetError());

	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096) < 0)
		throw creaturesException(std::string("SDL_mixer error during sound initialization: ") + Mix_GetError());
}

void SDLMixerBackend::shutdown() {
	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SDLMixerBackend::setViewpointCenter(float, float) {
	// TODO
}

void SDLMixerBackend::setMute(bool m) {
	// TODO
	muted = m;
}

boost::shared_ptr<AudioSource> SDLMixerBackend::newSource() {
	return boost::shared_ptr<AudioSource>(new SDLMixerSource());
}

AudioClip SDLMixerBackend::loadClip(const std::string &filename) {
	std::string fname = world.findFile(std::string("/Sounds/") + filename + ".wav");
	if (fname.size() == 0) return AudioClip();

	Mix_Chunk *buffer = Mix_LoadWAV(fname.c_str());
	if (!buffer) return AudioClip();

	AudioClip clip(new SDLMixerBuffer(buffer));
	return clip;
}

SDLMixerSource::SDLMixerSource() {
	channel = -1;
}

SDLMixerSource::~SDLMixerSource() {
}

AudioClip SDLMixerSource::getClip() const {
	AudioClip clip(static_cast<AudioBuffer *>(this->clip.get()));
	return clip;
}

void SDLMixerSource::setClip(const AudioClip &clip_) {
	SDLMixerBuffer *obp = dynamic_cast<SDLMixerBuffer *>(clip_.get());
	assert(obp);
	stop();
	this->clip = SDLMixerClip(obp);
}

SourceState SDLMixerSource::getState() const {
	if (channel == -1) return SS_STOP;
	if (!Mix_Playing(channel)) return SS_STOP;

	return SS_PLAY; // TODO: SS_PAUSE
}

void SDLMixerSource::play() {
	assert(clip);
	setFollowingView(followview); // re-register in the backend if needed

	channel = Mix_PlayChannel(-1, clip->buffer, 0);

	Mix_UnregisterAllEffects(channel); // TODO: needed?
	Mix_Volume(channel, 128); // default, TODO: needed?
}

void SDLMixerSource::stop() {
	if (channel == -1) return;

	// TODO
	bool oldfollow = followview;
	setFollowingView(false);          // unregister in backend
	followview = oldfollow;

	Mix_HaltChannel(channel);
	channel = -1;
}

void SDLMixerSource::fadeOut() {
	Mix_FadeOutChannel(channel, 500); // TODO: is 500 a good value?
	// TODO
}

void SDLMixerSource::setPos(float x, float y, float plane) {
	this->SkeletonAudioSource::setPos(x, y, plane);
	// TODO
	// Mix_SetPanning(soundchannel, left, right);
}

bool SDLMixerSource::isLooping() const {
	return false; // TODO
}

void SDLMixerSource::setLooping(bool l) {
	// TODO
}

void SDLMixerSource::setVolume(float v) {
	this->SkeletonAudioSource::setVolume(v);
	// TODO
	// Mix_Volume(soundchannel, volume);
}

void SDLMixerSource::setMute(bool m) {
	this->SkeletonAudioSource::setMute(m);
	// TODO
}

void SDLMixerSource::setFollowingView(bool f) {
	followview = f;
	// TODO
}

SDLMixerBuffer::SDLMixerBuffer(Mix_Chunk *b) {
	assert(b);
	buffer = b;
}

SDLMixerBuffer::~SDLMixerBuffer() {
	Mix_FreeChunk(buffer);
}

unsigned int SDLMixerBuffer::length_ms() const {
	return buffer->alen / 22050; // TODO: good?
}

unsigned int SDLMixerBuffer::length_samples() const {
	return buffer->alen;
}

/* vim: set noet: */
