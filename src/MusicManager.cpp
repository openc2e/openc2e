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
#include "Engine.h"

// this is all for MusicManager::tick
#include "World.h"
#include "Camera.h"
#include "Room.h"
#include "MetaRoom.h"

MusicManager musicmanager;

MusicManager::MusicManager() = default;
MusicManager::~MusicManager() {
	for (std::map<std::string, MNGFile *>::iterator i = files.begin(); i != files.end(); i++) {
		delete i->second;
	}
}

void MusicManager::playTrack(std::string track, unsigned int _how_long_before_changing_track_ms) {
	if (track == last_track) {
		return;
	}
	last_track = track;

	if (track == "Silence") {
		mng_music.playSilence();
	} else {

		// TODO: this needs to be smarter, based on the gametype, use of the CAOS command
		// "MIDI", and engine_usemidimusicsystem. C2E has three channels that can be
		// controlled separately: sound effects (0), MIDI (1), and generated music (2)
		if (engine.gametype == "cv" || engine.gametype == "sm") {
			std::string filename = world.findFile("Sounds/" + track + ".mid");
			if (!filename.size()) {
				fmt::print("Couldn't find MIDI file '{}'!\n", track);
				return;
			}
			engine.audio->setBackgroundMusic(filename);
			how_long_before_changing_track_ms = _how_long_before_changing_track_ms;
			return;
		}

		std::string filename, trackname;

		std::string::size_type n = track.find("\\");
		if (n == std::string::npos) {
			filename = "Music.mng"; // TODO
			trackname = track;
		} else {
			filename = track.substr(0, n);
			trackname = track.substr(n + 1, std::string::npos);
		}

		MNGFile *file;
		std::transform(filename.begin(), filename.end(), filename.begin(), (int(*)(int))tolower);
		if (files.find(filename) == files.end()) {
			std::string realfilename = world.findFile("Sounds/" + filename);
			if (!realfilename.size()) {
				fmt::print("Couldn't find MNG file '{}'!\n", filename);
				return; // TODO: exception?
			}

			file = new MNGFile(realfilename);
			files[filename] = file;
		} else {
			file = files[filename];
		}

		mng_music.playTrack(file, trackname);
		mng_music.startPlayback(*engine.audio);
	}
	how_long_before_changing_track_ms = _how_long_before_changing_track_ms;
}

void MusicManager::tick() {
	if (how_long_before_changing_track_ms > 0) {
		how_long_before_changing_track_ms -= world.ticktime;
	}
	if (how_long_before_changing_track_ms <= 0) {
		// TODO: this behaviour is different in C2, and should probably be cleverer
		MetaRoom *m = engine.camera->getMetaRoom();
		if (m) {
			std::shared_ptr<Room> r = m->roomAt(engine.camera->getXCentre(), engine.camera->getYCentre());
			if (r && r->music.size()) {
				playTrack(r->music, 0);
			} else if (m->music.size()) {
				playTrack(m->music, 0);
			}
		}
	}
}

void MusicManager::render(signed short *data, size_t len) {
	mng_music.render(data, len);
}

/* vim: set noet: */
