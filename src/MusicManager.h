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

#include "mngfile.h"

class MusicManager {
private:
	std::map<std::string, MNGFile *> files;

	class MusicTrack *currenttrack;

public:
	MusicManager();
	~MusicManager();

	void tick();

	void playTrack(std::string track, unsigned int latency);
};

extern MusicManager musicmanager;

class MusicEffect {
protected:
	MNGEffectDecNode *node;

public:
	MusicEffect(MNGEffectDecNode *n);
};

class MusicVoice {
protected:
	MNGVoiceNode *node;

public:
	MusicVoice(MNGVoiceNode *n);
};

class MusicLayer {
protected:
	MNGUpdateNode *updatenode;

	MusicTrack *parent;

	std::map<std::string, float> variables;
	float updaterate, volume, interval, beatsynch;

	MusicLayer(MusicTrack *p);
};

class MusicAleotoricLayer : public MusicLayer {
protected:
	MNGAleotoricLayerNode *node;

	MusicEffect *effect;
	std::vector<MusicVoice *> voices;
	unsigned int currvoice;

public:
	MusicAleotoricLayer(MNGAleotoricLayerNode *n, MusicTrack *p);
};

class MusicLoopLayer : public MusicLayer {
protected:
	MNGLoopLayerNode *node;

	std::pair<char *, int> wave;

public:
	MusicLoopLayer(MNGLoopLayerNode *n, MusicTrack *p);
};

class MusicTrack {
protected:
	MNGTrackDecNode *node;
	MNGFile *parent;

	std::vector<MusicLayer *> layers;

	float fadein, fadeout, beatlength, volume;

public:
	MusicTrack(MNGFile *p, MNGTrackDecNode *n);
	virtual ~MusicTrack();

	MNGFile *getParent() { return parent; }
};

#endif
/* vim: set noet: */
