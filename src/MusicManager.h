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

	size_t render(uint16 *data, size_t len);
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

	shared_ptr<MusicTrack> parent;

	std::map<std::string, float> variables;
	float updaterate, volume, interval, beatsynch;

	MusicLayer(shared_ptr<MusicTrack> p);
};

class MusicAleotoricLayer : public MusicLayer {
protected:
	MNGAleotoricLayerNode *node;

	shared_ptr<MusicEffect> effect;
	std::vector<shared_ptr<MusicVoice> > voices;
	unsigned int currvoice;

public:
	MusicAleotoricLayer(MNGAleotoricLayerNode *n, shared_ptr<MusicTrack> p);
};

class MusicLoopLayer : public MusicLayer {
protected:
	MNGLoopLayerNode *node;

	std::pair<char *, int> wave;

public:
	MusicLoopLayer(MNGLoopLayerNode *n, shared_ptr<MusicTrack> p);
};

class MusicTrack : public boost::enable_shared_from_this<class MusicTrack> {
protected:
	MNGTrackDecNode *node;
	MNGFile *parent;

	std::vector<shared_ptr<MusicLayer> > layers;

	float fadein, fadeout, beatlength, volume;

public:
	MusicTrack(MNGFile *p, MNGTrackDecNode *n);
	void init();
	virtual ~MusicTrack();

	MNGFile *getParent() { return parent; }
};

#endif
/* vim: set noet: */
