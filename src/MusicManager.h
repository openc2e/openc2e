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

#include "audiobackend/AudioChannel.h"

#include <map>
#include <memory>
#include <string>

class AudioBackend;
class MNGMusic;

class MusicManager {
  public:
	MusicManager(const std::shared_ptr<AudioBackend>& backend);
	~MusicManager();
	void stop();

	float getVolume();
	void setVolume(float volume);
	float getMIDIVolume();
	void setMIDIVolume(float volume);
	bool isMuted();
	void setMuted(bool muted);
	bool isMIDIMuted();
	void setMIDIMuted(bool muted);

	void tick();
	void playTrack(std::string track);
	void playTrackForAtLeastThisManyMilliseconds(std::string track, unsigned int how_long_before_changing_track_ms);

  private:
	void updateVolumes();

	std::shared_ptr<AudioBackend> backend;
	bool music_muted = false;
	float music_volume = 1.0;
	float midi_volume = 1.0;

	AudioChannel creatures1_channel;
	unsigned int creatures1_ticks_until_next_sound;
	std::map<std::string, class MNGFile*> files;
	std::unique_ptr<MNGMusic> mng_music;
	std::string last_track;
	unsigned int how_long_before_changing_track_ms = 0;
};

#endif
/* vim: set noet: */
