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
#include "Engine.h"
#include "PointerAgent.h"
#include "Backend.h"
#include "caosVM.h"
#include <iostream>
using std::cerr;

/**
 CLAC (command) message (integer)
 %status maybe

 Sets the type of message sent to the TARG agent when clicked.  If set to -1, no message
 will be sent.  Using this command will override and reset the value set with CLIK.
*/
void c_CLAC(caosVM *vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(message)

	valid_agent(vm->targ);

	vm->targ->clac[0] = message;
	vm->targ->clik = -1;
}

/**
 CLAC (integer)
 %status maybe
*/
void v_CLAC(caosVM *vm) {
	valid_agent(vm->targ);

	if (vm->targ->clik == -1)
		vm->result.setInt(vm->targ->clac[0]);
	else
		vm->result.setInt(-2);
}

/**
 CLIK (command) msg1 (integer) msg2 (integer) msg3 (integer)
 %status maybe

 Sets three different message types that will sent to the TARG agent when clicked, on a 
 rotating basis.  Setting any of the three types to -1 will cause it to be ignored. Using
 this command will override and reset the value set by CLAC.
*/
void c_CLIK(caosVM *vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(msg3)
	VM_PARAM_INTEGER(msg2)
	VM_PARAM_INTEGER(msg1)

	valid_agent(vm->targ);
	
	vm->targ->clac[0] = msg1;
	vm->targ->clac[1] = msg2;
	vm->targ->clac[2] = msg3;
	vm->targ->clik = 0;
}

/**
 CLIK (integer) data (integer)
 %status maybe
*/
void v_CLIK(caosVM *vm) {
	VM_PARAM_INTEGER(data)
	
	valid_agent(vm->targ);

	if (vm->targ->clik == -1)
		vm->result.setInt(-2);
	else switch (data) {
		case 0: vm->result.setInt(vm->targ->clik); break;
		case 1: vm->result.setInt(vm->targ->clac[0]); break;
		case 2: vm->result.setInt(vm->targ->clac[1]); break;
		case 3: vm->result.setInt(vm->targ->clac[2]); break;
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
void c_IMSK(caosVM *vm) {
	VM_PARAM_INTEGER(flags)

	valid_agent(vm->targ);
	vm->targ->imsk_key_down = (flags & 1);
	vm->targ->imsk_key_up = (flags & 2);
	vm->targ->imsk_mouse_move = (flags & 4);
	vm->targ->imsk_mouse_down = (flags & 8);
	vm->targ->imsk_mouse_up = (flags & 16);
	vm->targ->imsk_mouse_wheel = (flags & 32);
	vm->targ->imsk_translated_char = (flags & 64);
	// TODO
}

/**
 IMSK (integer)
 %status stub

 Returns the input event flags for the target agent. See the IMSK command for details.
*/
void v_IMSK(caosVM *vm) {
	valid_agent(vm->targ);
	vm->result.setInt(0); // TODO
}

/**
 KEYD (integer) keycode (integer)
 %status maybe

 Returns 1 if the specified key is held down, or 0 otherwise.
*/
void v_KEYD(caosVM *vm) {
	VM_PARAM_INTEGER(keycode) // keycodes are crazy broken windows things

	if (engine.backend->keyDown(keycode))
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 HOTS (agent)
 %status maybe
 %pragma variants all

 Returns the agent that is currently underneath the Hand.
 NB: this command is not a real c1/c2 command, backported for convenience
*/
void v_HOTS(caosVM *vm) {
	Agent *a = world.agentAt(world.hand()->pointerX(), world.hand()->pointerY()); // TODO: use hotspot
	
	vm->result.setAgent(a);
}

/**
 HOTP (integer)
 %status maybe

 Returns the PART of the agent (given by HOTS) that is currently underneath the Hand.

 Transparency of the parts themselves is ignored.
*/
void v_HOTP(caosVM *vm) {
	CompoundPart *a = world.partAt(world.hand()->pointerX(), world.hand()->pointerY(), false);
	if (a)
		vm->result.setInt(a->id);
	else
		vm->result.setInt(-1);
}

/**
 PURE (command) value (integer)
 %status maybe

 Turns the normal pointing and clicking behavior of the Hand on (0) and off (1).

 If set to off, CLIK and CLAC will not work, and clicking events must be handled by IMSK.
*/
void c_PURE(caosVM *vm) {
	VM_PARAM_INTEGER(value)

	world.hand()->handle_events = !value;
}

/**
 PURE (integer)
 %status maybe

 Returns whether the normal pointing and clicking behavior of the Hand is on or off.
*/
void v_PURE(caosVM *vm) {
	// TODO: alex claims PURE is inverse behaviour for the command, is this true for this function too? (assuming it is for now)
	if (world.hand()->handle_events)
		vm->result.setInt(0);
	else
		vm->result.setInt(1);
}

/**
 MOPX (integer)
 %status maybe

 Returns the current X coordinate of the Hand in the world.
*/
void v_MOPX(caosVM *vm) {
	vm->result.setInt((int)world.hand()->pointerX());
}

/**
 MOPY (integer)
 %status maybe

 Returns the current Y coordinate of the Hand in the world.
*/
void v_MOPY(caosVM *vm) {
	vm->result.setInt((int)world.hand()->pointerY());
}

/**
 SCOL (integer) andmask (integer) eormask (integer) upspeeds (bytestring) downspeeds (bytestring)
 %status stub
*/
void v_SCOL(caosVM *vm) {
	VM_PARAM_BYTESTR(downspeeds)
	VM_PARAM_BYTESTR(upspeeds)
	VM_PARAM_INTEGER(eormask)
	VM_PARAM_INTEGER(andmask)

	vm->result.setInt(0); // TODO
}

/**
 SCRL (command) enable (integer)
 %status stub

 Turns on (1) or off (0) keyboard/mouse scrolling.
*/
void c_SCRL(caosVM *vm) {
	VM_PARAM_INTEGER(enable)

	// TODO
}

/**
 MOUS (command) behaviour (integer)
 %status stub
*/
void c_MOUS(caosVM *vm) {
	VM_PARAM_INTEGER(behaviour)
	
	// TODO
}

/* vim: set noet: */
