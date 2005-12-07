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
#include <iostream>
using std::cout;
using std::cerr;

#include "SDLBackend.h" // hack for now

/**
 SNDE (command) filename (string)
 %status maybe
*/
void caosVM::c_SNDE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	caos_assert(targ);
	if (world.camera.getMetaRoom() != world.map.metaRoomAt(targ->x, targ->y)) return;
	SoundSlot *s = g_backend->getAudioSlot(filename);
	if (s) {
		s->play();
		targ->positionAudio(s);
	}
}

/**
 SNDC (command) filename (string)
 %status maybe
*/
void caosVM::c_SNDC() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	caos_assert(targ);
	if (targ->soundslot)
		targ->soundslot->stop();
	if (world.camera.getMetaRoom() != world.map.metaRoomAt(targ->x, targ->y)) return;
	SoundSlot *s = g_backend->getAudioSlot(filename);
	if (s) {
		targ->soundslot = s;
		s->play();
		s->agent = targ.get();
		targ->positionAudio(s);
	}
}

/**
 SNDL (command) filename (string)
 %status maybe
*/
void caosVM::c_SNDL() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(filename)

	caos_assert(targ);
	if (targ->soundslot)
		targ->soundslot->stop();
	if (world.camera.getMetaRoom() != world.map.metaRoomAt(targ->x, targ->y)) return;
	SoundSlot *s = g_backend->getAudioSlot(filename);
	if (s) {
		targ->soundslot = s;
		s->playLooped();
		s->agent = targ.get();
		targ->positionAudio(s);
	}
}

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
*/
void caosVM::c_FADE() {
	VM_VERIFY_SIZE(0)
		
	caos_assert(targ);
	if (targ->soundslot)
		targ->soundslot->fadeOut();
}

/**
 STPC (command)
 %status maybe
*/
void caosVM::c_STPC() {
	caos_assert(targ);
	if (targ->soundslot)
		targ->soundslot->stop();
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

/* vim: set noet: */
