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

#include "caos_assert.h"
#include "caosVM.h"
#include "caosScript.h"
#include "Agent.h"
#include "World.h"
#include "Engine.h"
#include "MusicManager.h"
#include "SoundManager.h"
#include "Camera.h"
#include "MetaRoom.h"
#include "Room.h"
#include "Map.h"
#include <string>
#include <limits.h>
#include <iostream>
#include <memory>
using std::cout;
using std::cerr;

#ifndef M_PI
# define M_PI           3.14159265358979323846  /* pi */
#endif

bool agentOnCamera(Agent *targ, bool checkall = false); // caosVM_camera.cpp

/**
 SNDE (command) filename (string)
 %status maybe

 Play an uncontrolled sound at the target agent's current position.
*/

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
 %cost c1,c2 0

 Play an uncontrolled sound at the target agent's current position.
*/

void caosVM::c_SNDE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	valid_agent(targ);
	if (engine.camera->getMetaRoom() != world.map->metaRoomAt(targ->x, targ->y) || !agentOnCamera(targ)) return; // TODO: is it correct behaviour for only onscreen agents to play?
	targ->playAudio(filename, false, false);
}

/**
 SNDC (command) filename (string)
 %status maybe
 
 Start playing a controlled sound with the target agent, which will follow the agent as it moves.
*/

/**
 SNDC (command) filename (bareword)
 %status maybe
 %pragma variants c1 c2
 %cost c1,c2 0

 Start playing a controlled sound with the target agent, which will follow the agent as it moves.
*/

void caosVM::c_SNDC() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	valid_agent(targ);
	if (engine.camera->getMetaRoom() != world.map->metaRoomAt(targ->x, targ->y) || !agentOnCamera(targ)) return; // TODO: is it correct behaviour for only onscreen agents to play?
	targ->playAudio(filename, true, false);
}

/**
 SNDL (command) filename (string)
 %status maybe
 
 Start playing a looping controlled sound with the target agent, which will follow the agent as it moves.
*/

/**
 SNDL (command) filename (bareword)
 %status maybe
 %pragma variants c1 c2
 %cost c1,c2 0

 Start playing a looping controlled sound with the target agent, which will follow the agent as it moves.
*/

void caosVM::c_SNDL() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	valid_agent(targ);
	targ->playAudio(filename, true, true);
}

/**
 SNDQ (command) filename (string) delay (integer)
 %status stub
 %pragma variants all

 Play an uncontrolled sound at the target agent's current position, but delay before playing.
*/
void caosVM::c_SNDQ() {
	VM_PARAM_INTEGER(delay);
	VM_PARAM_STRING(filename);

	valid_agent(targ);

	// This appears to be a CAOS command in every game, but isn't used in any official
	// scripts.
}

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

	MetaRoom *r = world.map->metaRoomAt(x, y);
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

	MetaRoom *r = world.map->metaRoomAt(x, y);
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

	std::shared_ptr<Room> r = world.map->roomAt(x, y);
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

	std::shared_ptr<Room> r = world.map->roomAt(x, y);
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
	if (targ->sound) {
		targ->sound.fadeOut();
	}
}

/**
 STPC (command)
 %status maybe
 %pragma variants c1 c2 cv c3 sm
 %cost c1,c2 0
*/
void caosVM::c_STPC() {
	valid_agent(targ);
	if (targ->sound) {
		targ->sound.stop();
	}
}

/**
 STRK (command) latency (integer) track (string)
 %status maybe

 Play the specified music track. It will play for at least latency seconds.
*/
void caosVM::c_STRK() {
	VM_PARAM_STRING(track)
	VM_PARAM_INTEGER(latency)

	engine.musicmanager->playTrack(track, latency * 1000);
}

/**
 VOLM (command) type (integer) volume (integer)
 %status maybe

 Changes the volume of the specified type of audio; 0 for sound effects, 1 for midi or 2 for dynamic music.
 Volume is from -10000 (silent) to 0 (maximum).
*/
void caosVM::c_VOLM() {
	VM_PARAM_INTEGER(volume)
	VM_PARAM_INTEGER(type)

	float scaled_volume = volume / 10000.0 + 1;

	if (type == 0) {
		soundmanager.setVolume(scaled_volume);
	} else if (type == 1) {
		engine.musicmanager->setMIDIVolume(scaled_volume);
	} else if (type == 2) {
		engine.musicmanager->setVolume(scaled_volume);
	} else {
		// In Creatures Village's !startup.cos
		// TODO: do a full stacktrace like CaosException?
		printf(
			"exec of \"%s\" raised warning: VOLM: Can't set volume of audio type %i\n",
			currentscript->filename.size() ? currentscript->filename.c_str() : "(null)",
			type
		);
	}
}

/**
 VOLM (integer) type (integer)
 %status maybe

 Return the volume of the specified type of audio; 0 for sound effects, 1 for midi or 2 for dynamic music.
 Volume is from -10000 (silent) to 0 (maximum).
*/
void caosVM::v_VOLM() {
	VM_PARAM_INTEGER(type)

	float volume = 1.0;
	if (type == 0) {
		volume = soundmanager.getVolume();
	} else if (type == 1) {
		// Official documentation says "Currently not supported for MIDI", but Sea-Monkeys
		// uses it.
		volume = engine.musicmanager->getMIDIVolume();
	} else if (type == 2) {
		volume = engine.musicmanager->getVolume();
	} else {
		throw caosException("Can't get volume of audio type " + std::to_string(type));
	}

	result.setInt(std::round((volume - 1) * 10000));
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

	int value = 0;
	if (soundmanager.isMuted()) value |= 1;
	if (engine.musicmanager->isMuted()) value |= 2;

	value ^= eormask;

	soundmanager.setMuted(value & 1);
	engine.musicmanager->setMuted(value & 2);

	result.setInt(value & andmask);
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
 VOIC (command) genus (integer) gender (integer) age (integer)
 %status stub

  Set the the voice of the target agent to specified creature voice, choosing the nearest match.
*/
void caosVM::c_VOIC() {
	VM_PARAM_INTEGER(age)
	VM_PARAM_INTEGER(gender)
	VM_PARAM_INTEGER(genus)

	valid_agent(targ);
	// TODO
	targ->setVoice("DefaultVoice");
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

	engine.musicmanager->playTrack(midifile, 0);
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
	if (engine.camera->getMetaRoom() != world.map->metaRoomAt(targ->x, targ->y)) return; // TODO: needs better check ;)

	// TODO
}

struct SineStream : AudioStream {
	int position = 0;
	bool stereo = true;
	int frequency;
	SineStream(int frequency_)
		: frequency(frequency_) {}

	// avoid panning?
	virtual size_t produce(void *data, size_t len) {
		int sampleCount = len / (stereo ? 4 : 2);
		int p = 0;
		signed short *buf = (signed short *)data;

		for (int i = 0; i < sampleCount; i++) {
			float t1 = sin(2*M_PI*position*350/frequency);
			buf[p++] = (SHRT_MAX / 2) * t1;
			if (stereo) {
				float t2 = sin(2*M_PI*position*440/frequency);
				buf[p++] = (SHRT_MAX / 2) * t2;
			}
			position++;
		}
		return len;
	}
};

/**
 DBG: SINE (command) rate (integer) track (integer)
 %status stub

 Plays a sine wave coming from TARG

 track = 0 to fix the source at TARG's current location; track = 1 to follow
 view, track = 2 to inject it into the BGM source
 */
void caosVM::c_DBG_SINE() {
	VM_PARAM_INTEGER(track);
	VM_PARAM_INTEGER(rate);
	if (track != 2) {
		valid_agent(targ);

		if (targ->sound) {
			targ->sound.stop();
			targ->sound = {};
		}
	}

	auto stream = std::make_shared<SineStream>(rate);
	// if (track == 2) {
		// soundmanager.playMusic(stream);
	// } else {
		// TODO
	// }
}

/**
 DBG: SBGM (command)
 %status maybe

 Stops the BGM source. This probably doesn't do what you want it to do.
 Don't touch.
 */
void caosVM::c_DBG_SBGM() {
	engine.musicmanager->stop();
}


/* vim: set noet: */
