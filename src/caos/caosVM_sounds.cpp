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
#include "Camera.h"
#include <iostream>
using std::cout;
using std::cerr;

#include "Backend.h" // hack for now

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
 %status stub
*/
void caosVM::c_MMSC() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_STRING(track_name)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	// TODO
}

/**
 MMSC (string) x (integer) y (integer)
 %status stub
*/
void caosVM::v_MMSC() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	// TODO
	result.setString("");
}

/**
 RMSC (command) x (integer) y (integer) track_name (string)
 %status stub
*/
void caosVM::c_RMSC() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_STRING(track_name)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	// TODO
}

/**
 RMSC (string) x (integer) y (integer)
 %status stub
*/
void caosVM::v_RMSC() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	// TODO
	result.setString("");
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
 %status stub

 Play the specified music track. It will play for at least latency seconds.
*/
void caosVM::c_STRK() {
	VM_PARAM_STRING(track)
	VM_PARAM_INTEGER(latency)

	// TODO
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
 %status stub
*/
void caosVM::c_SEZZ() {
	VM_PARAM_STRING(text)

	valid_agent(targ);

	// TODO
}

/**
 VOIS (command) voice (string)
 %status stub
*/
void caosVM::c_VOIS() {
	VM_PARAM_STRING(voice)
	
	valid_agent(targ);

	// TODO
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

/* vim: set noet: */
