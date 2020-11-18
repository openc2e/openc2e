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
#include "Engine.h"
#include "SoundManager.h"
#include "World.h"

// this is all for MusicManager::tick
#include "World.h"
#include "Camera.h"
#include "Room.h"
#include "MetaRoom.h"

MusicManager musicmanager;

MusicManager::MusicManager() = default;
MusicManager::~MusicManager() {
	stop();
	for (std::map<std::string, MNGFile *>::iterator i = files.begin(); i != files.end(); i++) {
		delete i->second;
	}
}

void MusicManager::stop() {
	if (engine.audio) {
		engine.audio->stopChannel(mng_channel);
		engine.audio->stopMIDI();
	}
}

float MusicManager::getVolume() {
	return music_volume;
}

void MusicManager::setVolume(float volume) {
	music_volume = volume;
	updateVolumes();
}

float MusicManager::getMIDIVolume() {
	return midi_volume;
}

void MusicManager::setMIDIVolume(float volume) {
	midi_volume = volume;
	updateVolumes();
}

bool MusicManager::isMuted() {
	return music_muted;
}

void MusicManager::setMuted(bool muted) {
	music_muted = muted;
	updateVolumes();
}

bool MusicManager::isMIDIMuted() {
	auto it = world.variables.find("engine_mute");
	return it != world.variables.end() && it->second.hasInt() && it->second.getInt() != 0;
}

void MusicManager::setMIDIMuted(bool muted) {
	world.variables["engine_mute"] = muted ? 1 : 0;
	updateVolumes();
}

void MusicManager::updateVolumes() {
	engine.audio->setMIDIVolume(isMIDIMuted() ? 0 : midi_volume);
	engine.audio->setChannelVolume(mng_channel, music_muted ? 0 : music_volume);
	engine.audio->setChannelVolume(creatures1_channel, music_muted ? 0 : music_volume * 0.4);
}

void MusicManager::playTrack(std::string track, unsigned int _how_long_before_changing_track_ms) {
	if (track == last_track) {
		return;
	}
	last_track = track;

	auto game_usemidimusicsystem = world.variables["engine_usemidimusicsystem"];
	bool usemidimusicsystem = game_usemidimusicsystem.hasInt() && game_usemidimusicsystem.getInt() != 0;
	// TODO: what happens if you call the CAOS command MIDI and usemidimusicsystem isn't enabled?

	if (usemidimusicsystem) {
		mng_music.playSilence(); // or just stop it?

		if (track == "") {
			engine.audio->stopMIDI();
			return;
		}

		std::string filename = world.findFile("Sounds/" + track + ".mid");
		if (!filename.size()) {
			fmt::print("Couldn't find MIDI file '{}'!\n", track);
			return;
		}
		engine.audio->playMIDIFile(filename);
		how_long_before_changing_track_ms = _how_long_before_changing_track_ms;
		return;
	}

	if (track == "Silence") {
		mng_music.playSilence();
	} else {
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
		if (engine.audio->getChannelState(mng_channel) != AUDIO_PLAYING) {
			mng_channel = engine.audio->playStream(&mng_music);
			updateVolumes();
		}
	}
	how_long_before_changing_track_ms = _how_long_before_changing_track_ms;
}

void MusicManager::tick() {
	// play C1 music
	// TODO: this doesn't seem to actually be every 7 seconds, but actually somewhat random
	// TODO: this should be linked to 'real' time, so it doesn't go crazy when game speed is modified
	// TODO: is this the right place for this?
	if (engine.version == 1 && (world.tickcount % 70) == 0 &&
	    engine.audio->getChannelState(creatures1_channel) == AUDIO_STOPPED)
	{
		auto sounds = world.findFiles("Sounds", "MU*.wav");
		if (sounds.size()) {
			creatures1_channel = engine.audio->playClip(sounds[rand() % sounds.size()]);
		}
	}

	// play MNG/MIDI music
	// TODO: how does engine_near_death_track_name work?
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
	
	// update volumes based on new volumes, muting, etc
	updateVolumes();
}

/* vim: set noet: */
