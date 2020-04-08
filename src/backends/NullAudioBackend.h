/*
 *  NullAudioBackend.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Sun Aug 12 2007.
 *  Copyright (c) 2007 Bryan Donlan. All rights reserved.
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

#ifndef _NULLAUDIOBACKEND_H
#define _NULLAUDIOBACKEND_H

#include "AudioBackend.h"
#include "backends/SkeletonAudioBackend.h"

class NullAudioSource : public SkeletonAudioSource {};

class NullAudioBackend : public AudioBackend {
protected:
	bool muted;

public:
	NullAudioBackend() { }
	void init() { muted = false; }
	void shutdown() { }
	void setViewpointCenter(float, float) { }
	void setMute(bool b) { muted = b; }
	bool isMuted() const { return muted; }
	boost::shared_ptr<AudioSource> newSource() { return boost::shared_ptr<AudioSource>(); }
	boost::shared_ptr<AudioSource> loadClip(const boost::string &filename) {
		boost::string fname = world.findFile(boost::string("Sounds/") + filename + ".wav");
		if (fname.size() == 0) return boost::shared_ptr<AudioSource>();
		return boost::shared_ptr<AudioSource>(new NullAudioSource());
	}

	boost::shared_ptr<AudioSource> getBGMSource() {
		return boost::shared_ptr<AudioSource>();
	}
};
 
#endif

