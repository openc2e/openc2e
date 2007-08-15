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

class NullAudioBackend : public AudioBackend {
protected:
	bool muted;
	AudioClip clip;

public:
	NullAudioBackend() { }
	void init() { muted = false; }
	void shutdown() { }
	void setViewpointCenter(float, float) { }
	void setMute(bool b) { muted = b; }
	bool isMuted() { return muted; }
	boost::shared_ptr<AudioSource> newSource() { return boost::shared_ptr<AudioSource>(); }
	AudioClip loadClip(const std::string &) { return AudioClip(); }
};
 
#endif

