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

#include "audiobackend/SDLMixerBackend.h"
#include "creaturesException.h"
#include <SDL.h>
#include <algorithm>
#include <cassert>
#include <climits>
#include <memory>

SDLMixerBackend::SDLMixerBackend() {
	bgm_source.reset(new SDLMixerSource);
}

void SDLMixerBackend::mixer_callback(void *userdata, uint8_t *buffer, int num_bytes) {	
	SDLMixerBackend *backend = (SDLMixerBackend*)userdata;
	AudioStream stream = ((SDLMixerSource*)backend->bgm_source.get())->stream;
	if (!stream) {
		return;
	}

	size_t num_samples = num_bytes / 2;
	auto& bgm_render_buffer = backend->bgm_render_buffer;
	bgm_render_buffer.resize(num_samples);
	stream->produce(bgm_render_buffer.data(), bgm_render_buffer.size() * 2);

	if (backend->muted) {
		return;
	}

	int16_t *buf = (int16_t*)buffer;
	for (size_t i = 0; i < num_samples; ++i) {
		buf[i] = std::max(SHRT_MIN, std::min(buf[i] + bgm_render_buffer[i], SHRT_MAX));
	}
}

void SDLMixerBackend::init() {
	// TODO: ensure SDLBackend is in use?

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
		throw creaturesException(std::string("SDL error during sound initialization: ") + SDL_GetError());

	if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 4096) < 0)
		throw creaturesException(std::string("SDL_mixer error during sound initialization: ") + Mix_GetError());

	Mix_HookMusic(SDLMixerBackend::mixer_callback, this);

	Mix_AllocateChannels(50); // TODO
}

void SDLMixerBackend::shutdown() {
	Mix_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void SDLMixerBackend::setViewpointCenter(float, float) {
	// TODO
}

void SDLMixerBackend::setMute(bool m) {
	muted = m;
	Mix_Volume(-1, muted ? 0 : MIX_MAX_VOLUME);
}

std::shared_ptr<AudioSource> SDLMixerBackend::newSource() {
	return std::shared_ptr<AudioSource>(new SDLMixerSource());
}

std::shared_ptr<AudioSource> SDLMixerBackend::loadClip(const std::string &filename) {
	Mix_Chunk *buffer = Mix_LoadWAV(filename.c_str());
	if (!buffer) return std::shared_ptr<AudioSource>();

	SDLMixerSource* source = new SDLMixerSource();
	source->clip = SDLMixerClip(new SDLMixerBuffer(buffer));

	return std::shared_ptr<AudioSource>(source);
}

std::shared_ptr<AudioSource> SDLMixerBackend::getBGMSource() {
	return bgm_source;
}

SDLMixerSource::SDLMixerSource() {
	looping = false;
	muted = false;
	volume = 1.0f;
	followview = false;
	channel = -1;
}

SDLMixerSource::~SDLMixerSource() {
}

SourceState SDLMixerSource::getState() const {
	if (channel == -1) return SS_STOP;
	if (!Mix_Playing(channel)) return SS_STOP;

	if (Mix_Paused(channel)) return SS_PAUSE;
	
	return SS_PLAY;
}

void SDLMixerSource::play() {
	if (!clip) return;
	setFollowingView(followview); // re-register in the backend if needed

	channel = Mix_PlayChannel(-1, clip->buffer, (looping ? -1 : 0));

	Mix_UnregisterAllEffects(channel); // TODO: needed?
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
}

void SDLMixerSource::setPos(float x_, float y_, float z_) {
	x = x_;
	y = y_;
	z = z_;
	// TODO
	// Mix_SetPanning(soundchannel, left, right);
}

void SDLMixerSource::getPos(float &x_, float &y_, float &z_) const {
	x_ = x;
	y_ = y;
	z_ = z;
	// TODO
	// Mix_GetPanning(soundchannel, left, right); ?
}


bool SDLMixerSource::isLooping() const {
	return looping;
}

void SDLMixerSource::setLooping(bool l) {
	looping = l;
	// handled in play()
}

void SDLMixerSource::setVolume(float v) {
	volume = v;
	Mix_VolumeChunk(clip->buffer, v * MIX_MAX_VOLUME);
}

void SDLMixerSource::setMute(bool m) {
	Mix_VolumeChunk(clip->buffer, m ? 0 : volume * MIX_MAX_VOLUME);
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
