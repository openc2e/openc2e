/*
 *  caosVM_sounds.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Jun 01 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#include "openc2e.h"
#include "caosVM.h"
#include "Agent.h"
#include "World.h"
#include "Engine.h"
#include "AudioBackend.h"
#include "MusicManager.h"
#include "Camera.h"
#include "MetaRoom.h"
#include "Room.h"
#include <iostream>
using std::cout;
using std::cerr;

bool agentOnCamera(Agent *targ, bool checkall = false); // caosVM_camera.cpp

/**
 SNDE (command) filename (string)
 %status maybe

 Play an uncontrolled sound at the target agent's current position.
*/
void caosVM::c_SNDE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	valid_agent(targ);
	if (world.camera->getMetaRoom() != world.map.metaRoomAt(targ->x, targ->y) || !agentOnCamera(targ)) return; // TODO: is it correct behaviour for only onscreen agents to play?
	targ->playAudio(filename, false, false);
}

/**
 SNDE (command) filename (bareword)
 %status maybe
 %pragma variants c1 c2
 %cost c1,c2 0

 Play an uncontrolled sound at the target agent's current position.
*/

/**
 SNDV (command) filename (string)
 %status maybe
 %pragma variants c1
 %pragma implementation caosVM::c_SNDE
 %cost c1,c2 0

 Play an uncontrolled sound at the target agent's current position.
*/

/**
 SNDC (command) filename (string)
 %status maybe
 
 Start playing a controlled sound with the target agent, which will follow the agent as it moves.
*/
void caosVM::c_SNDC() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	valid_agent(targ);
	if (world.camera->getMetaRoom() != world.map.metaRoomAt(targ->x, targ->y) || !agentOnCamera(targ)) return; // TODO: is it correct behaviour for only onscreen agents to play?
	targ->playAudio(filename, true, false);
}

/**
 SNDC (command) filename (bareword)
 %status maybe
 %pragma variants c1 c2
 %cost c1,c2 0

 Start playing a controlled sound with the target agent, which will follow the agent as it moves.
*/

/**
 SNDL (command) filename (string)
 %status maybe
 
 Start playing a looping controlled sound with the target agent, which will follow the agent as it moves.
*/
void caosVM::c_SNDL() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	valid_agent(targ);
	targ->playAudio(filename, true, true);
}

/**
 SNDL (command) filename (bareword)
 %status maybe
 %pragma variants c1 c2
 %cost c1,c2 0
 
 Start playing a looping controlled sound with the target agent, which will follow the agent as it moves.
*/

/**
 MMSC (command) x (integer) y (integer) track_name (string)
 %status maybe

 Set the music track to be played in the metaroom containing the given coordinates.
*/
void caosVM::c_MMSC() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_STRING(track_name)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	MetaRoom *r = world.map.metaRoomAt(x, y);
	caos_assert(r); // note that real c2e doesn't check

	r->music = track_name;
}

/**
 MMSC (string) x (integer) y (integer)
 %status maybe

 Returns the music track to be played in the metaroom containing the given coordinates.
*/
void caosVM::v_MMSC() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	MetaRoom *r = world.map.metaRoomAt(x, y);
	caos_assert(r); // note that real c2e doesn't check

	result.setString(r->music);
}

/**
 RMSC (command) x (integer) y (integer) track_name (string)
 %status maybe

 Set the music track to be played in the room containing the given coordinates.
*/
void caosVM::c_RMSC() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_STRING(track_name)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	shared_ptr<Room> r = world.map.roomAt(x, y);
	caos_assert(r);

	r->music = track_name;
}

/**
 RMSC (string) x (integer) y (integer)
 %status maybe

 Returns the music track to be played in the room containing the given coordinates.
*/
void caosVM::v_RMSC() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	shared_ptr<Room> r = world.map.roomAt(x, y);
	caos_assert(r);

	result.setString(r->music);
}

/**
 FADE (command)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0
*/
void caosVM::c_FADE() {
	VM_VERIFY_SIZE(0)
		
	valid_agent(targ);
	if (targ->sound)
		targ->sound->fadeOut();
}

/**
 STPC (command)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0
*/
void caosVM::c_STPC() {
	valid_agent(targ);
	targ->sound.reset();
}

/**
 STRK (command) latency (integer) track (string)
 %status maybe

 Play the specified music track. It will play for at least latency seconds.
*/
void caosVM::c_STRK() {
	VM_PARAM_STRING(track)
	VM_PARAM_INTEGER(latency)

	musicmanager.playTrack(track, latency);
}

/**
 VOLM (command) type (integer) volume (integer)
 %status stub

 Changes the volume of the specified type of audio; 0 for sound effects, 1 for midi or 2 for dynamic music.
 Volume is from -10000 (silent) to 0 (maximum).
*/
void caosVM::c_VOLM() {
	VM_PARAM_INTEGER(volume)
	VM_PARAM_INTEGER(type)

	// TODO
}

/**
 VOLM (integer) type (integer)
 %status stub

 Return the volumne of the specified type of audio; 0 for sound effects, 1 for midi or 2 for dynamic music.
 Volume is from -10000 (silent) to 0 (maximum).
*/
void caosVM::v_VOLM() {
	VM_PARAM_INTEGER(type)

	result.setInt(0); // TODO
}

/**
 MUTE (integer) andmask (integer) eormask (integer)
 %status maybe

 This returns/sets information about which types of in-game audio are muted.
 Set andmask for the information you want returned, and eormask for the information you want changed.
 1 is for normal sound, and 2 is for music (so 3 is for both combined).
*/
void caosVM::v_MUTE() {
	VM_PARAM_INTEGER(eormask)
	VM_PARAM_INTEGER(andmask)

	// TODO: we should maintain state despite having no audio engine, probably
	// (UI scripting assumes changes were successful)
	if (!engine.audio) {
		result.setInt(0);
		return;
	}

	// TODO: music

	if (eormask & 1) engine.audio->setMute(!engine.audio->isMuted());

	int r = 0;
	if (andmask & 1 && engine.audio->isMuted()) r += 1;
	result.setInt(r);
}

/**
 SEZZ (command) text (string)
 %status maybe

 Tells the target agent to speak the specified text.
*/
void caosVM::c_SEZZ() {
	VM_PARAM_STRING(text)

	valid_agent(targ);

	targ->speak(text);
}

/**
 VOIS (command) voice (string)
 %status maybe

 Set the voice of the target agent to the specified voice (a catalogue tag).
*/
void caosVM::c_VOIS() {
	VM_PARAM_STRING(voice)
	
	valid_agent(targ);

	targ->setVoice(voice);
	// TODO: reload DefaultVoice on failure?
}

/**
 MIDI (command) midifile (string)
 %status stub

 Plays the MIDI file specified, or stops playing if passed an empty string.
*/
void caosVM::c_MIDI() {
	VM_PARAM_STRING(midifile)

	// TODO
}

/**
 PLDS (command) filename (bareword)
 %status stub
 %pragma variants c1 c2

 Preload the specified sound file if TARG is visible or just offscreen.
*/
void caosVM::c_PLDS() {
	VM_PARAM_STRING(filename)

	valid_agent(targ);
	if (world.camera->getMetaRoom() != world.map.metaRoomAt(targ->x, targ->y)) return; // TODO: needs better check ;)

	// TODO
}

struct SineStream : AudioStreamBase {
	double period;
	double phase;
	bool stereo;
	int amplitude;
	SineStream(int freq, bool stereo_, int ampl)
		: period((double)44100/(double)freq), phase(0), stereo(stereo_), amplitude(ampl) { }

	// avoid panning?
	virtual bool isStereo() const { return stereo; }
	virtual int sampleRate() const { return 44100; }
	virtual int latency() const { return 1000; }
	virtual bool reset() { return true; }
	virtual int bitDepth() const { return 16; }
	virtual size_t produce(void *data, size_t len) {
		int sampleCount = len / (stereo ? 4 : 2);
		int p = 0;
		unsigned short *buf = (unsigned short *)data;

		for (int i = 0; i < sampleCount; i++) {
			phase += (1/period);
			if (phase > 1)
				phase = phase - 1;
			double wave = sin( phase * M_PI * 2) * amplitude;
			buf[p++] = (signed short)wave;
			if (stereo)
				buf[p++] = (signed short)wave;
		}
		return len;
	}
};

/**
 DBG: SINE (command) rate (integer) stereo (integer) track (integer) amplitude (integer)
 %status maybe

 Plays a sine wave coming from TARG

 track = 0 to fix the source at TARG's current location; track = 1 to follow
 view, track = 2 to inject it into the BGM source
 */
void caosVM::c_DBG_SINE() {
	VM_PARAM_INTEGER(ampl);
	VM_PARAM_INTEGER(track);
	VM_PARAM_INTEGER(stereo);
	VM_PARAM_INTEGER(rate);
	if (track != 2) {
		valid_agent(targ);

		if (targ->sound)
			targ->sound->stop();
	}
	std::shared_ptr<AudioSource> src;
	if (track == 2)
		src = engine.audio->getBGMSource();
	else
		src = targ->sound = engine.audio->newSource();
	if (!src)
		throw creaturesException("Audio is unavailable");
	src->setStream(AudioStream(new SineStream(rate, stereo, ampl)));
	if (track != 2) {
		src->setFollowingView(!track);
	}
	src->play();
}

/**
 DBG: SBGM (command)
 %status maybe

 Stops the BGM source. This probably doesn't do what you want it to do.
 Don't touch.
 */
void caosVM::c_DBG_SBGM() {
	std::shared_ptr<AudioSource> src = engine.audio->getBGMSource();
	if (src)
		src->stop();
}


/* vim: set noet: */
