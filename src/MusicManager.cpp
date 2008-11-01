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
#include <iostream> // for debug messages

MusicManager musicmanager;

MusicManager::MusicManager() {
	currenttrack = 0;
}

MusicManager::~MusicManager() {
	if (currenttrack) {
		delete currenttrack;
	}

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

	MusicTrack *tracknode = new MusicTrack(file, file->tracks[trackname]);
	// TODO: play track :)
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

MusicLayer::MusicLayer(MusicTrack *p) {
	parent = p;

	updaterate = 1.0f;
	volume = 1.0f;
	interval = 0.0f;
	beatsynch = 0.0f;
}

MusicAleotoricLayer::MusicAleotoricLayer(MNGAleotoricLayerNode *n, MusicTrack *p) : MusicLayer(p) {
	node = n;
	currvoice = 0;
	effect = NULL;

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
			effect = new MusicEffect(n);
			continue;
		}

		MNGVoiceNode *v = dynamic_cast<MNGVoiceNode *>(n);
		if (v) {
			MusicVoice *voice = new MusicVoice(v);
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

		MNGIntervalNode *i = dynamic_cast<MNGIntervalNode *>(n);
		if (i) {
			interval = evaluateExpression(i->getExpression());
			continue;
		}

		throw MNGFileException("unexpected node in AleotoricLayer: " + n->dump());
	}
}

MusicLoopLayer::MusicLoopLayer(MNGLoopLayerNode *n, MusicTrack *p) : MusicLayer(p) {
	node = n;

	// TODO
}

MusicTrack::MusicTrack(MNGFile *p, MNGTrackDecNode *n) {
	node = n;
	parent = p;

	for (std::list<MNGNode *>::iterator i = node->children->begin(); i != node->children->end(); i++) {
		MNGNode *n = *i;

		MNGAleotoricLayerNode *al = dynamic_cast<MNGAleotoricLayerNode *>(n);
		if (al) {
			MusicAleotoricLayer *mal = new MusicAleotoricLayer(al, this);
			layers.push_back(mal);
			continue;
		}

		MNGLoopLayerNode *ll = dynamic_cast<MNGLoopLayerNode *>(n);
		if (ll) {
			MusicLoopLayer *mll = new MusicLoopLayer(ll, this);
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
	for (std::vector<MusicLayer *>::iterator i = layers.begin(); i != layers.end(); i++) {
		delete *i;
	}
}

/* vim: set noet: */
