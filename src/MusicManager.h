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

#include "mngfile/mngfile.h"
#include "MNGMusic.h"
#include <map>
#include <memory>
#include <string>

class MusicManager {
public:
	MusicManager();
	~MusicManager();

	void tick();
	void playTrack(std::string track, unsigned int how_long_before_changing_track_ms);
	void render(signed short *data, size_t len);

private:
	std::map<std::string, MNGFile *> files;
	MNGMusic mng_music;
	unsigned int how_long_before_changing_track_ms = 0;
};

extern MusicManager musicmanager;

#endif
/* vim: set noet: */
