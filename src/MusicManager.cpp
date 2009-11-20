/*
 *  MusicManager.cpp
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

#include "MusicManager.h"
#include "World.h"
#include "AudioBackend.h"
#include "Engine.h"
#include <iostream> // for debug messages
using namespace boost;

MusicManager musicmanager;

struct MusicStream : public AudioStreamBase {
	virtual bool isStereo() const { return true; }
	virtual int sampleRate() const { return 44100; }
	virtual int latency() const { return 1000; }
	virtual bool reset() { return true; }
	virtual int bitDepth() const { return 16; }

	virtual size_t produce(void *data, size_t len) {
		return musicmanager.render((uint16 *)data, len);
	}
};

MusicManager::MusicManager() {
}

MusicManager::~MusicManager() {
	for (std::map<std::string, MNGFile *>::iterator i = files.begin(); i != files.end(); i++) {
		delete i->second;
	}
}

void MusicManager::tick() {
}

void MusicManager::playTrack(std::string track, unsigned int latency) {
	std::string filename, trackname;

	std::string::size_type n = track.find("\\");
	if (n == std::string::npos) {
		filename = "Music.mng"; // TODO
		trackname = track;
	} else {
		filename = track.substr(0, n);
		trackname = track.substr(n + 1, std::string::npos);
	}

	std::string realfilename = world.findFile("Sounds/" + filename);
	if (!realfilename.size()) {
		std::cout << "Couldn't find MNG file '" << filename << "'!" << std::endl;
		return; // TODO: exception?
	}

	MNGFile *file;
	if (files.find(realfilename) == files.end()) {
		file = new MNGFile(realfilename);
		files[filename] = file;
	} else {
		file = files[realfilename];
	}

	if (file->tracks.find(trackname) == file->tracks.end()) {
		std::cout << "Couldn't find MNG track '" << trackname << "' ('" << track << "')!" << std::endl;
		return; // TODO: exception?
	}

	shared_ptr<MusicTrack> tracknode(new MusicTrack(file, file->tracks[trackname]));
	playTrack(tracknode);
}

void MusicManager::playTrack(shared_ptr<MusicTrack> track) {
	currenttrack = track;

	startPlayback();
}

void MusicManager::startPlayback() {
	shared_ptr<AudioSource> src = engine.audio->getBGMSource();
	if (!src) return;

	if (!stream) {
		// we assume no-one else ever steals the BGM stream from
		// under us, but what are we meant to do then anyway?
		stream = shared_ptr<MusicStream>(new MusicStream);
		src->setStream(stream);
		src->play();
	}
}

size_t MusicManager::render(uint16 *data, size_t len) {
	return len;
}

float evaluateExpression(MNGExpression *e) {
	MNGConstantNode *c = dynamic_cast<MNGConstantNode *>(e);
	if (c) {
		return c->getValue();
	}

	throw MNGFileException("couldn't evaluate expression " + e->dump());
}

MusicEffect::MusicEffect(MNGEffectDecNode *n) {
	node = n;

	// TODO
}

MusicVoice::MusicVoice(MNGVoiceNode *n) {
	node = n;

	// TODO
}

MusicLayer::MusicLayer(shared_ptr<MusicTrack> p) {
	parent = p;

	updaterate = 1.0f;
	volume = 1.0f;
	interval = 0.0f;
	beatsynch = 0.0f;
}

MusicAleotoricLayer::MusicAleotoricLayer(MNGAleotoricLayerNode *n, shared_ptr<MusicTrack> p) : MusicLayer(p) {
	node = n;
	currvoice = 0;

	for (std::list<MNGNode *>::iterator i = node->children->begin(); i != node->children->end(); i++) {
		MNGNode *n = *i;

		MNGEffectNode *e = dynamic_cast<MNGEffectNode *>(n);
		if (e) {
			if (effect)
				throw MNGFileException("got effect '" + e->getName() + "' but we already have one!");

			std::map<std::string, class MNGEffectDecNode *> &effects = parent->getParent()->effects;
			if (effects.find(e->getName()) == effects.end())
				throw MNGFileException("couldn't find effect '" + e->getName() + "'");

			MNGEffectDecNode *n = effects[e->getName()];
			effect = shared_ptr<MusicEffect>(new MusicEffect(n));
			continue;
		}

		MNGVoiceNode *v = dynamic_cast<MNGVoiceNode *>(n);
		if (v) {
			shared_ptr<MusicVoice> voice(new MusicVoice(v));
			voices.push_back(voice);
			continue;
		}

		MNGUpdateNode *u = dynamic_cast<MNGUpdateNode *>(n);
		if (u) {
			// TODO
			continue;
		}

		// TODO: LayerVolume
		// TODO: variable
		// TODO: beatsynch
		// TODO: updaterate

		MNGIntervalNode *in = dynamic_cast<MNGIntervalNode *>(n);
		if (in) {
			interval = evaluateExpression(in->getExpression());
			continue;
		}

		throw MNGFileException("unexpected node in AleotoricLayer: " + n->dump());
	}
}

MusicLoopLayer::MusicLoopLayer(MNGLoopLayerNode *n, shared_ptr<MusicTrack> p) : MusicLayer(p) {
	node = n;

	// TODO
}

MusicTrack::MusicTrack(MNGFile *p, MNGTrackDecNode *n) {
	node = n;
	parent = p;
}

// shared_from_this
void MusicTrack::init() {
	for (std::list<MNGNode *>::iterator i = node->children->begin(); i != node->children->end(); i++) {
		MNGNode *n = *i;

		MNGAleotoricLayerNode *al = dynamic_cast<MNGAleotoricLayerNode *>(n);
		if (al) {
			shared_ptr<MusicLayer> mal(new MusicAleotoricLayer(al, shared_from_this()));
			layers.push_back(mal);
			continue;
		}

		MNGLoopLayerNode *ll = dynamic_cast<MNGLoopLayerNode *>(n);
		if (ll) {
			shared_ptr<MusicLayer> mll(new MusicLoopLayer(ll, shared_from_this()));
			layers.push_back(mll);
			continue;
		}

		MNGFadeInNode *fi = dynamic_cast<MNGFadeInNode *>(n);
		if (fi) {
			// TODO
			continue;
		}

		MNGFadeOutNode *fo = dynamic_cast<MNGFadeOutNode *>(n);
		if (fo) {
			// TODO
			continue;
		}

		MNGBeatLengthNode *bl = dynamic_cast<MNGBeatLengthNode *>(n);
		if (bl) {
			// TODO
			continue;
		}

		MNGLayerVolumeNode *lv = dynamic_cast<MNGLayerVolumeNode *>(n);
		if (lv) {
			// TODO
			continue;
		}

		throw MNGFileException("unexpected node in Track: " + n->dump());
	}
}

MusicTrack::~MusicTrack() {
}

/* vim: set noet: */
