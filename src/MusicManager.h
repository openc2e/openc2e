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
using std::shared_ptr;

class MusicManager {
private:
	std::map<std::string, MNGFile *> files;

	shared_ptr<class MusicTrack> currenttrack, nexttrack;
	bool playing_silence;
	unsigned int current_latency;

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

struct FloatAudioBuffer {
	unsigned int start_offset;
	size_t len, position;
	float *data;
	float volume, pan;

	FloatAudioBuffer() { data = NULL; len = 0; position = 0; start_offset = 0; volume = 1.0f; pan = 0.0f; }
	FloatAudioBuffer(float *d, size_t l, unsigned int o = 0, float v = 1.0f, float p = 0.0f) {
		position = 0;
		len = l;
		data = d;
		start_offset = o;
		volume = v;
		pan = p;
	}
};

class MusicWave {
protected:
	FloatAudioBuffer buffer;

public:
	MusicWave(MNGFile *p, MNGWaveNode *w);
	~MusicWave();
	FloatAudioBuffer &getData() { return buffer; }
};

class MusicStage {
protected:
	MNGStageNode *node;

	MNGExpression *pan, *volume, *delay, *tempodelay;

public:
	MusicStage(MNGStageNode *n);
	std::vector<FloatAudioBuffer> applyStage(std::vector<FloatAudioBuffer> &sources, float beatlength);
};

class MusicEffect {
protected:
	MNGEffectDecNode *node;
	std::vector<shared_ptr<MusicStage> > stages;

public:
	MusicEffect(MNGEffectDecNode *n);
	std::vector<FloatAudioBuffer> applyEffect(class MusicTrack *t, std::vector<FloatAudioBuffer> src, float beatlength);
};

class MusicVoice {
protected:
	MNGVoiceNode *node;
	MNGUpdateNode *updatenode;
	class MusicLayer *parent;
	shared_ptr<MusicWave> wave;
	shared_ptr<MusicEffect> effect;

	std::vector<MNGConditionNode *> conditions;

	MNGExpression *interval_expression;
	float interval, volume;

public:
	MusicVoice(shared_ptr<class MusicLayer> p, MNGVoiceNode *n);
	shared_ptr<MusicWave> getWave() { return wave; }
	float getInterval() { return interval; }
	float getVolume() { return volume; }
	shared_ptr<MusicEffect> getEffect() { return effect; }
	bool shouldPlay();
	void runUpdateBlock();
	MusicLayer *getParent() { return parent; }
};

class MusicLayer : public std::enable_shared_from_this<class MusicLayer> {
protected:
	MNGUpdateNode *updatenode;

	MusicTrack *parent;
	unsigned int next_offset;

	std::map<std::string, float> variables;
	float updaterate, volume, interval, beatsynch, pan;

	MusicLayer(shared_ptr<MusicTrack> p);
	void runUpdateBlock();

public:
	MusicTrack *getParent() { return parent; }
	float &getVariable(std::string name) { return variables[name]; }
	virtual void update(unsigned int latency) = 0;
	float getVolume() { return volume; }
	float getInterval() { return interval; }
	float getPan() { return pan; }
};

class MusicAleotoricLayer : public MusicLayer {
protected:
	MNGAleotoricLayerNode *node;

	shared_ptr<MusicEffect> effect;
	std::vector<shared_ptr<MusicVoice> > voices;

public:
	MusicAleotoricLayer(MNGAleotoricLayerNode *n, shared_ptr<MusicTrack> p);
	void init();
	void update(unsigned int latency);
};

class MusicLoopLayer : public MusicLayer {
protected:
	MNGLoopLayerNode *node;

	unsigned int update_period;
	shared_ptr<MusicWave> wave;

public:
	MusicLoopLayer(MNGLoopLayerNode *n, shared_ptr<MusicTrack> p);
	void init();
	void update(unsigned int latency);
};

class MusicTrack : public std::enable_shared_from_this<class MusicTrack> {
protected:
	MNGTrackDecNode *node;
	MNGFile *parent;

	std::vector<shared_ptr<MusicLayer> > layers;

	float fadein, fadeout, beatlength, volume;

	unsigned int fadein_count, fadeout_count;

	unsigned int current_offset;
	std::vector<FloatAudioBuffer> buffers;

public:
	MusicTrack(MNGFile *p, MNGTrackDecNode *n);
	void init();
	virtual ~MusicTrack();
	void render(signed short *data, size_t len);
	void addBuffer(FloatAudioBuffer buf) { buffers.push_back(buf); }
	unsigned int getCurrentOffset() { return current_offset; }
	void update(unsigned int latency);
	float getVolume() { return volume; }
	float getBeatLength() { return beatlength; }

	void startFadeIn();
	void startFadeOut();
	bool fadedOut();

	MNGFile *getParent() { return parent; }
	std::string getName() { return node->getName(); }
};

#endif
/* vim: set noet: */
