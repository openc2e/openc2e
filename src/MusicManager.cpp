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

	MusicTrack *tracknode = new MusicTrack(file->tracks[trackname]);
	// TODO: play track :)
}

MusicLoopLayer::MusicLoopLayer(MNGLoopLayerNode *n, MusicTrack *p) {
	parent = p;
	node = n;
}

MusicTrack::MusicTrack(MNGTrackDecNode *n) {
	node = n;
}

MusicTrack::~MusicTrack() {
	for (std::vector<MusicLayer *>::iterator i = layers.begin(); i != layers.end(); i++) {
		delete *i;
	}
}

/* vim: set noet: */
