/*
 *  caosVM_input.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on 28/07/2004.
 *  Copyright 2004 Alyssa Milburn. All rights reserved.
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

#include "World.h"
#include "PointerAgent.h"
#include "SDLBackend.h"
#include "caosVM.h"
#include <iostream>
using std::cerr;

/**
 CLAC (command) message (integer)
 %status maybe

 Sets the type of message sent to the TARG agent when clicked.  If set to -1, no message
 will be sent.  Using this command will override and reset the value set with CLIK.
*/
void caosVM::c_CLAC() {
  VM_VERIFY_SIZE(1)
  VM_PARAM_INTEGER(message)

  valid_agent(targ);

  targ->clac[0] = calculateScriptId(message);
  targ->clik = -1;
}

/**
 CLAC (integer)
 %status maybe
*/
void caosVM::v_CLAC() {
	valid_agent(targ);
	if (targ->clik != -1)
		result.setInt(targ->clac[0]);
	else
		result.setInt(-2);
}

/**
 CLIK (command) msg1 (integer) msg2 (integer) msg3 (integer)
 %status maybe

 Sets three different message types that will sent to the TARG agent when clicked, on a 
 rotating basis.  Setting any of the three types to -1 will cause it to be ignored. Using
 this command will override and reset the value set by CLAC.
*/
void caosVM::c_CLIK() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(msg3)
	VM_PARAM_INTEGER(msg2)
	VM_PARAM_INTEGER(msg1)

	valid_agent(targ);
	
	targ->clac[0] = calculateScriptId(msg1);
	targ->clac[1] = calculateScriptId(msg2);
	targ->clac[2] = calculateScriptId(msg3);
	targ->clik = 0;
}

/**
 CLIK (integer) data (integer)
 %status maybe
*/
void caosVM::v_CLIK() {
	VM_PARAM_INTEGER(data)
	
	valid_agent(targ);

	if (targ->clik == -1)
		result.setInt(-2);
	else switch (data) {
		case 0: result.setInt(targ->clik); break;
		case 1: result.setInt(targ->clac[0]); break;
		case 2: result.setInt(targ->clac[1]); break;
		case 3: result.setInt(targ->clac[2]); break;
	}
}

/**
 IMSK (command) flags (integer)
 %status stub

 Sets the input event flags for the target agent, which tell the engine which events the 
 agent requires scripts to be fired for. For example, setting the "key up" flag means the 
 target agent has the relevant script executed every time a key is released.

 Add the following values together to calculate the flags parameter: 1 for key down, 2 for key up, 4 for mouse move, 8 for mouse down, 16 for mouse up, 32 for mouse wheel movement and 64 for (translated) keypress.

 TODO: link to the script details (event numbers and parameters).
*/
void caosVM::c_IMSK() {
	VM_PARAM_INTEGER(flags)

	valid_agent(targ);
	targ->imsk_key_down = (flags & 1);
	targ->imsk_key_up = (flags & 2);
	targ->imsk_mouse_move = (flags & 4);
	targ->imsk_mouse_down = (flags & 8);
	targ->imsk_mouse_up = (flags & 16);
	targ->imsk_mouse_wheel = (flags & 32);
	targ->imsk_translated_char = (flags & 64);
	// TODO
}

/**
 IMSK (integer)
 %status stub

 Returns the input event flags for the target agent. See the IMSK command for details.
*/
void caosVM::v_IMSK() {
	valid_agent(targ);
	result.setInt(0); // TODO
}

/**
 KEYD (integer) keycode (integer)
 %status maybe

 Returns 1 if the specified key is held down, or 0 otherwise.
*/
void caosVM::v_KEYD() {
	VM_PARAM_INTEGER(keycode) // keycodes are crazy broken windows things

	if (world.backend->keyDown(keycode))
		result.setInt(1);
	else
		result.setInt(0);
}

/**
 HOTS (agent)
 %status maybe

 Returns the agent that is currently underneath the Hand.
*/
void caosVM::v_HOTS() {
	Agent *a = world.agentAt(world.hand()->x, world.hand()->y); // TODO: use hotspot
	
	result.setAgent(a);
}

/**
 HOTP (integer)
 %status maybe

 Returns the PART of the agent (given by HOTS) that is currently underneath the Hand.

 Transparency of the parts themselves is ignored.
*/
void caosVM::v_HOTP() {
	CompoundPart *a = world.partAt(world.hand()->x, world.hand()->y, false);
	if (a)
		result.setInt(a->id);
	else
		result.setInt(-1);
}

/**
 PURE (command) value (integer)
 %status maybe

 Turns the normal pointing and clicking behavior of the Hand on (0) and off (1).

 If set to off, CLIK and CLAC will not work, and clicking events must be handled by IMSK.
*/
void caosVM::c_PURE() {
	VM_PARAM_INTEGER(value)

	world.hand()->handle_events = !value;
}

/**
 PURE (integer)
 %status maybe

 Returns whether the normal pointing and clicking behavior of the Hand is on or off.
*/
void caosVM::v_PURE() {
	// TODO: alex claims PURE is inverse behaviour for the command, is this true for this function too? (assuming it is for now)
	if (world.hand()->handle_events)
		result.setInt(0);
	else
		result.setInt(1);
}

/**
 MOPX (integer)
 %status maybe

 Returns the current X coordinate of the Hand in the world.
*/
void caosVM::v_MOPX() {
	result.setInt(world.hand()->x);
}

/**
 MOPY (integer)
 %status maybe

 Returns the current Y coordinate of the Hand in the world.
*/
void caosVM::v_MOPY() {
	result.setInt(world.hand()->y);
}

/**
 SCOL (integer) andmask (integer) eormask (integer) upspeeds (bytestring) downspeeds (bytestring)
 %status stub
*/
void caosVM::v_SCOL() {
	VM_PARAM_BYTESTR(downspeeds)
	VM_PARAM_BYTESTR(upspeeds)
	VM_PARAM_INTEGER(eormask)
	VM_PARAM_INTEGER(andmask)

	result.setInt(0); // TODO
}

/**
 SCRL (command) enable (integer)
 %status stub

 Turns on (1) or off (0) keyboard/mouse scrolling.
*/
void caosVM::c_SCRL() {
	VM_PARAM_INTEGER(enable)

	// TODO
}

/* vim: set noet: */
