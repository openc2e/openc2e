/*
 *  caosVM_time.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat Jun 05 2004.
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

#include "caosVM.h"
#include "World.h"
#include "SDLBackend.h"

/**
 PACE (float)
 %status maybe

 return speed factor of last ten ticks.
 
 < 1.0 = engine is ticking slower than BUZZ rating. > 1.0 = engine is ticking faster.

 You might want to use this to reduce the amount of agents created or the amount of processing done if the speed factor is below 1.0.
*/
void caosVM::v_PACE() {
	result.setFloat(world.pace);
}

/**
 BUZZ (command) interval (integer)
 %status maybe
*/
void caosVM::c_BUZZ() {
	VM_PARAM_INTEGER(interval)

	world.ticktime = interval;
}

/**
 BUZZ (integer)
 %status maybe
*/
void caosVM::v_BUZZ() {
	result.setInt(world.ticktime);
}

/**
 DATE (integer)
 %status stub

 Returns the day in the season of the current game world, starting at 0.
*/
void caosVM::v_DATE() {
	result.setInt(0); // TODO
}

/**
 HIST DATE (integer)
 %pragma implementation caosVM::v_DATE
 %status stub
 
 Returns the day in the season of the current game world, starting at 0. Identical to DATE.
*/

/**
 SEAN (integer)
 %status stub

 Returns the current game world season. 0 is spring, 1 is summer, 2 is autumn and 3 is winter.
*/
void caosVM::v_SEAN() {
	result.setInt(0); // TODO
}

/**
 HIST SEAN (integer)
 %pragma implementation caosVM::v_SEAN
 %status stub

 Returns the current game world season. 0 is spring, 1 is summer, 2 is autumn and 3 is winter. Identical to SEAN.
*/

/**
 TIME (integer)
 %status stub

 Returns the time of day in the current game world. 0 is dawn, 1 is morning, 2 is afternoon, 3 is evening and 4 is night.
*/
void caosVM::v_TIME() {
	result.setInt(0); // TODO
}

/**
 HIST TIME (integer)
 %pragma implementation caosVM::v_TIME
 %status stub
 
 Returns the time of day in the current game world. 0 is dawn, 1 is morning, 2 is afternoon, 3 is evening and 4 is night. Identical to TIME.
*/

/**
 YEAR (integer)
 %status stub

 Returns the number of game years elapsed in the current world.
*/
void caosVM::v_YEAR() {
	result.setInt(0); // TODO
}

/**
 HIST YEAR (integer)
 %pragma implementation caosVM::v_YEAR
 %status stub

 Returns the number of game years elapsed. Identical to YEAR.
*/

/**
 MSEC (integer)
 %status maybe
*/
void caosVM::v_MSEC() {
	result.setInt(g_backend->ticks());
}

/**
 WPAU (command) paused (integer)
 %status stub
 
 If paused is 0, enable world ticks, otherwise (1) disable them.
*/
void caosVM::c_WPAU() {
	VM_PARAM_INTEGER(paused)

	// TODO
}

/**
 WPAU (integer)
 %status stub

 Returns 1 if world ticks are paused, or 0 otherwise.
*/
void caosVM::v_WPAU() {
	result.setInt(0); // TODO
}

/**
 PAUS (command) paused (integer)
 %status stub

 If paused is 0, unpause target agent, otherwise (1) pause it.
*/
void caosVM::c_PAUS() {
	VM_PARAM_INTEGER(paused)

	caos_assert(targ);

	// TODO
}

/**
 PAUS (integer)
 %status stub

 Returns 1 if target agent is paused, or 0 otherwise.
*/
void caosVM::v_PAUS() {
	result.setInt(0); // TODO
}

/**
 RTIF (string) timestamp (integer) format (string)
 %status maybe

 Returns the result of strftime with the current timestamp and format.
*/
void caosVM::v_RTIF() {
	VM_PARAM_STRING(format)
	VM_PARAM_INTEGER(timestamp)
	
	char buffer[1000]; // no idea what maximum length should be.. we can't really do this dynamically
	time_t tstamp = timestamp;
	strftime(buffer, 1000, format.c_str(), gmtime(&tstamp)); // TODO: does gmtime return null?

	result.setString(buffer);
}

/* vim: set noet: */
