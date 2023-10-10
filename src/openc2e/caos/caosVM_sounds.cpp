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

#include "Agent.h"
#include "Camera.h"
#include "Engine.h"
#include "Map.h"
#include "MetaRoom.h"
#include "MusicManager.h"
#include "Room.h"
#include "SoundManager.h"
#include "World.h"
#include "caosScript.h"
#include "caosVM.h"
#include "common/throw_ifnot.h"

#include <iostream>
#include <limits.h>
#include <memory>
#include <string>
using std::cerr;
using std::cout;

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

bool agentOnCamera(Agent* targ, bool checkall = false); // caosVM_camera.cpp

/**
 SNDE (command) filename (string)
 %status maybe

 Play an uncontrolled sound at the target agent's current position.
*/

/**
 SNDE (command) filename (bareword)
 %status maybe
 %variants c1 c2

 Play an uncontrolled sound at the target agent's current position.
*/

/**
 SNDV (command) filename (string)
 %status maybe
 %variants c1

 Play an uncontrolled sound at the target agent's current position.
*/

void c_SNDE(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	valid_agent(vm->targ);
	if (engine.camera->getMetaRoom() != world.map->metaRoomAt(vm->targ->x, vm->targ->y) || !agentOnCamera(vm->targ))
		return; // TODO: is it correct behaviour for only onscreen agents to play?
	vm->targ->playAudio(filename, false, false);
}

/**
 SNDC (command) filename (string)
 %status maybe
 
 Start playing a controlled sound with the target agent, which will follow the agent as it moves.
*/

/**
 SNDC (command) filename (bareword)
 %status maybe
 %variants c1 c2

 Start playing a controlled sound with the target agent, which will follow the agent as it moves.
*/

void c_SNDC(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	valid_agent(vm->targ);
	if (engine.camera->getMetaRoom() != world.map->metaRoomAt(vm->targ->x, vm->targ->y) || !agentOnCamera(vm->targ))
		return; // TODO: is it correct behaviour for only onscreen agents to play?
	vm->targ->playAudio(filename, true, false);
}

/**
 SNDL (command) filename (string)
 %status maybe
 
 Start playing a looping controlled sound with the target agent, which will follow the agent as it moves.
*/

/**
 SNDL (command) filename (bareword)
 %status maybe
 %variants c1 c2

 Start playing a looping controlled sound with the target agent, which will follow the agent as it moves.
*/

void c_SNDL(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	valid_agent(vm->targ);
	vm->targ->playAudio(filename, true, true);
}

/**
 SNDQ (command) filename (string) delay (integer)
 %status stub
 %variants all

 Play an uncontrolled sound at the target agent's current position, but delay before playing.
*/
void c_SNDQ(caosVM* vm) {
	VM_PARAM_INTEGER(delay);
	VM_PARAM_STRING(filename);

	valid_agent(vm->targ);
	(void)delay;
	(void)filename;

	// This appears to be a CAOS command in every game, but isn't used in any official
	// scripts.
}

/**
 MMSC (command) x (integer) y (integer) track_name (string)
 %status maybe

 Set the music track to be played in the metaroom containing the given coordinates.
*/
void c_MMSC(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_STRING(track_name)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	MetaRoom* r = world.map->metaRoomAt(x, y);
	THROW_IFNOT(r); // note that real c2e doesn't check

	r->music = track_name;
}

/**
 MMSC (string) x (integer) y (integer)
 %status maybe

 Returns the music track to be played in the metaroom containing the given coordinates.
*/
void v_MMSC(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	MetaRoom* r = world.map->metaRoomAt(x, y);
	THROW_IFNOT(r); // note that real c2e doesn't check

	vm->result.setString(r->music);
}

/**
 RMSC (command) x (integer) y (integer) track_name (string)
 %status maybe

 Set the music track to be played in the room containing the given coordinates.
*/
void c_RMSC(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_STRING(track_name)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	std::shared_ptr<Room> r = world.map->roomAt(x, y);
	THROW_IFNOT(r);

	r->music = track_name;
}

/**
 RMSC (string) x (integer) y (integer)
 %status maybe

 Returns the music track to be played in the room containing the given coordinates.
*/
void v_RMSC(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	std::shared_ptr<Room> r = world.map->roomAt(x, y);
	THROW_IFNOT(r);

	vm->result.setString(r->music);
}

/**
 FADE (command)
 %status maybe
 %variants c1 c2 cv c3 sm
*/
void c_FADE(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	if (vm->targ->sound) {
		vm->targ->sound.fadeOut();
	}
}

/**
 STPC (command)
 %status maybe
 %variants c1 c2 cv c3 sm
*/
void c_STPC(caosVM* vm) {
	valid_agent(vm->targ);
	if (vm->targ->sound) {
		vm->targ->sound.stop();
	}
}

/**
 STRK (command) latency (integer) track (string)
 %status maybe

 Play the specified music track. It will play for at least latency seconds.
*/
void c_STRK(caosVM* vm) {
	VM_PARAM_STRING(track)
	VM_PARAM_INTEGER(latency)

	engine.musicmanager->playTrackForAtLeastThisManyMilliseconds(track, latency * 1000);
}

/**
 VOLM (command) type (integer) volume (integer)
 %status maybe

 Changes the volume of the specified type of audio; 0 for sound effects, 1 for midi or 2 for dynamic music.
 Volume is from -10000 (silent) to 0 (maximum).
*/
void c_VOLM(caosVM* vm) {
	VM_PARAM_INTEGER(volume)
	VM_PARAM_INTEGER(type)

	float scaled_volume = volume / 10000.0 + 1;

	if (type == 0) {
		soundmanager.setVolume(scaled_volume);
	} else if (type == 1) {
		engine.musicmanager->midi_set_volume(scaled_volume);
	} else if (type == 2) {
		engine.musicmanager->setVolume(scaled_volume);
	} else {
		// In Creatures Village's !startup.cos
		// TODO: do a full stacktrace like CaosException?
		fmt::print(
			"exec of \"{}\" raised warning: VOLM: Can't set volume of audio type {}\n",
			vm->currentscript->filename.size() ? vm->currentscript->filename : "(null)",
			type);
	}
}

/**
 VOLM (integer) type (integer)
 %status maybe

 Return the volume of the specified type of audio; 0 for sound effects, 1 for midi or 2 for dynamic music.
 Volume is from -10000 (silent) to 0 (maximum).
*/
void v_VOLM(caosVM* vm) {
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

	vm->result.setInt(std::round((volume - 1) * 10000));
}

/**
 MUTE (integer) andmask (integer) eormask (integer)
 %status maybe

 This returns/sets information about which types of in-game audio are muted.
 Set andmask for the information you want returned, and eormask for the information you want changed.
 1 is for normal sound, and 2 is for music (so 3 is for both combined).
*/
void v_MUTE(caosVM* vm) {
	VM_PARAM_INTEGER(eormask)
	VM_PARAM_INTEGER(andmask)

	int value = 0;
	if (soundmanager.isMuted())
		value |= 1;
	if (engine.musicmanager->isMuted())
		value |= 2;

	value ^= eormask;

	soundmanager.setMuted(value & 1);
	engine.musicmanager->setMuted(value & 2);

	vm->result.setInt(value & andmask);
}

/**
 SEZZ (command) text (string)
 %status maybe

 Tells the target agent to speak the specified text.
*/
void c_SEZZ(caosVM* vm) {
	VM_PARAM_STRING(text)

	valid_agent(vm->targ);

	vm->targ->speak(text);
}

/**
 VOIC (command) genus (integer) gender (integer) age (integer)
 %status stub

  Set the the voice of the target agent to specified creature voice, choosing the nearest match.
*/
void c_VOIC(caosVM* vm) {
	VM_PARAM_INTEGER(age)
	VM_PARAM_INTEGER(gender)
	VM_PARAM_INTEGER(genus)

	(void)age;
	(void)gender;
	(void)genus;

	valid_agent(vm->targ);
	// TODO
	vm->targ->setVoice("DefaultVoice");
}

/**
 VOIS (command) voice (string)
 %status maybe

 Set the voice of the target agent to the specified voice (a catalogue tag).
*/
void c_VOIS(caosVM* vm) {
	VM_PARAM_STRING(voice)

	valid_agent(vm->targ);

	vm->targ->setVoice(voice);
	// TODO: reload DefaultVoice on failure?
}

/**
 MIDI (command) midifile (string)
 %status done

 Plays the MIDI file specified, or stops playing if passed an empty string.
*/
void c_MIDI(caosVM* vm) {
	VM_PARAM_STRING(midifile)

	engine.musicmanager->playTrack(midifile);
}

/**
 PLDS (command) filename (bareword)
 %status stub
 %variants c1 c2

 Preload the specified sound file if TARG is visible or just offscreen.
*/
void c_PLDS(caosVM* vm) {
	VM_PARAM_STRING(filename)

	valid_agent(vm->targ);
	if (engine.camera->getMetaRoom() != world.map->metaRoomAt(vm->targ->x, vm->targ->y))
		return; // TODO: needs better check ;)

	// TODO
}

/**
 DBG: SBGM (command)
 %status maybe

 Stops the BGM source. This probably doesn't do what you want it to do.
 Don't touch.
 */
void c_DBG_SBGM(caosVM*) {
	engine.musicmanager->stop();
}


/* vim: set noet: */
