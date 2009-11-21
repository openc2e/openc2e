/*
 *  MusicManager.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Fri Oct 31 2008.
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

#ifndef _MUSICMANAGER_H
#define _MUSICMANAGER_H

#include "music/mngfile.h"
#include "endianlove.h"
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
#include <boost/enable_shared_from_this.hpp>

class MusicManager {
private:
	std::map<std::string, MNGFile *> files;
	shared_ptr<class MusicTrack> currenttrack;

	shared_ptr<class MusicStream> stream;

	void startPlayback();

	void playTrack(shared_ptr<class MusicTrack> track);

public:
	MusicManager();
	~MusicManager();

	void tick();
	void playTrack(std::string track, unsigned int latency);

	void render(signed short *data, size_t len);
};

extern MusicManager musicmanager;

class MusicWave {
protected:
	unsigned char *data;
	unsigned int length;

public:
	MusicWave(MNGFile *p, MNGWaveNode *w);
	unsigned char *getData() { return data; }
	unsigned int getLength() { return length; }
};

class MusicEffect {
protected:
	MNGEffectDecNode *node;

public:
	MusicEffect(MNGEffectDecNode *n);
};

class MusicVoice {
protected:
	MNGVoiceNode *node;
	shared_ptr<MusicWave> wave;

	float interval;

public:
	MusicVoice(MNGFile *p, MNGVoiceNode *n);
	shared_ptr<MusicWave> getWave() { return wave; }
	float getInterval() { return interval; }
};

class MusicLayer {
protected:
	MNGUpdateNode *updatenode;

	shared_ptr<MusicTrack> parent;
	unsigned int next_offset;

	std::map<std::string, float> variables;
	float updaterate, volume, interval, beatsynch;

	MusicLayer(shared_ptr<MusicTrack> p);
	void runUpdateBlock();

public:
	virtual void update() = 0;
};

class MusicAleotoricLayer : public MusicLayer {
protected:
	MNGAleotoricLayerNode *node;

	shared_ptr<MusicEffect> effect;
	std::vector<shared_ptr<MusicVoice> > voices;

public:
	MusicAleotoricLayer(MNGAleotoricLayerNode *n, shared_ptr<MusicTrack> p);
	void update();
};

class MusicLoopLayer : public MusicLayer {
protected:
	MNGLoopLayerNode *node;

	std::pair<char *, int> wave;

public:
	MusicLoopLayer(MNGLoopLayerNode *n, shared_ptr<MusicTrack> p);
	void update();
};

struct FloatAudioBuffer {
	unsigned int start_offset;
	size_t len, position;
	float *data;

	FloatAudioBuffer(float *d, size_t l, unsigned int o) { position = 0; len = l; data = d; start_offset = o; }
};

class MusicTrack : public boost::enable_shared_from_this<class MusicTrack> {
protected:
	MNGTrackDecNode *node;
	MNGFile *parent;

	std::vector<shared_ptr<MusicLayer> > layers;

	float fadein, fadeout, beatlength, volume;

	unsigned int current_offset;
	std::vector<FloatAudioBuffer> buffers;

public:
	MusicTrack(MNGFile *p, MNGTrackDecNode *n);
	void init();
	virtual ~MusicTrack();
	void render(signed short *data, size_t len);
	void addBuffer(FloatAudioBuffer buf) { buffers.push_back(buf); }
	unsigned int getCurrentOffset() { return current_offset; }
	void update();

	MNGFile *getParent() { return parent; }
};

#endif
/* vim: set noet: */
