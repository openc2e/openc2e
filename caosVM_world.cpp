/*
 *  caosVM_world.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Aug 28 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
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

#include "caosVM.h"
#include "World.h"

/**
 LOAD (command) worldname (string)
 %status stub
*/
void caosVM::c_LOAD() {
	VM_PARAM_STRING(worldname)

	// TODO
}

/**
 SAVE (command)
 %status stub

 Save the world at the end of the current tick. Beware; if you don't put this
 in an INST, it might save directly after your SAVE call (meaning upon loading,
 the script will execute the next instruction, often QUIT or LOAD, which is
 bad).
*/
void caosVM::c_SAVE() {
	// TODO
}

/**
 QUIT (command)
 %status stub

 Quit the game engine at the end of the current tick.
*/
void caosVM::c_QUIT() {
	// TODO
}

/**
 WNAM (string)
 %status stub

 Returns the name of the current world.
*/
void caosVM::v_WNAM() {
	// result.setString(world.name);
	result.setString("oh"); // TODO
}

/* vim: set noet: */
