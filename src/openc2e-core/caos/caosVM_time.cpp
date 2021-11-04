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

#include "Agent.h"
#include "Backend.h"
#include "Engine.h"
#include "World.h"
#include "caosVM.h"

#include <time.h> // gmtime and strftime

/**
 PACE (float)
 %status maybe

 return speed factor of last ten ticks.
 
 < 1.0 = engine is ticking faster than BUZZ rating. > 1.0 = engine is ticking slower.

 You might want to use this to reduce the amount of agents created or the amount of processing done if the speed factor is above 1.0.
*/
void v_PACE(caosVM* vm) {
	vm->result.setFloat(world.pace);
}

/**
 BUZZ (command) interval (integer)
 %status maybe
*/
void c_BUZZ(caosVM* vm) {
	VM_PARAM_INTEGER(interval)

	world.ticktime = interval;
}

/**
 BUZZ (integer)
 %status maybe
*/
void v_BUZZ(caosVM* vm) {
	vm->result.setInt(world.ticktime);
}

/**
 DATE (integer)
 %status maybe

 Returns the day in the season of the current game world, starting at 0.
*/
void v_DATE(caosVM* vm) {
	vm->result.setInt(world.dayofseason);
}

/**
 HIST DATE (integer) tick (integer)
 %status stub
 
 Returns the day in the season of the current game world at the specified world tick, starting at 0. See DATE.
*/
void v_HIST_DATE(caosVM* vm) {
	VM_PARAM_INTEGER(tick)

	vm->result.setInt(0); // TODO
}

/**
 SEAN (integer)
 %status maybe
 %variants c2 cv c3

 Returns the current game world season. 0 is spring, 1 is summer, 2 is autumn and 3 is winter.
*/
void v_SEAN(caosVM* vm) {
	vm->result.setInt(world.season);
}

/**
 HIST SEAN (integer) tick (integer)
 %status stub

 Returns the current game world season at the specified world tick. 0 is spring, 1 is summer, 2 is autumn and 3 is winter. See SEAN.
*/
void v_HIST_SEAN(caosVM* vm) {
	VM_PARAM_INTEGER(tick)

	vm->result.setInt(0); // TODO
}

/**
 TIME (integer)
 %status maybe

 Returns the time of day in the current game world. 0 is dawn, 1 is morning, 2 is afternoon, 3 is evening and 4 is night.
*/
void v_TIME(caosVM* vm) {
	vm->result.setInt(world.timeofday);
}

/**
 HIST TIME (integer) tick (integer)
 %status stub
 
 Returns the time of day in the current game world at the specified world tick. 0 is dawn, 1 is morning, 2 is afternoon, 3 is evening and 4 is night. See TIME.
*/
void v_HIST_TIME(caosVM* vm) {
	VM_PARAM_INTEGER(tick)

	vm->result.setInt(0); // TODO
}

/**
 YEAR (integer)
 %status maybe
 %variants c2 cv c3

 Returns the number of game years elapsed in the current world.
*/
void v_YEAR(caosVM* vm) {
	vm->result.setInt(world.year);
}

/**
 HIST YEAR (integer) tick (integer)
 %status stub

 Returns the number of game years elapsed at the specified world tick. See YEAR.
*/
void v_HIST_YEAR(caosVM* vm) {
	VM_PARAM_INTEGER(tick)

	vm->result.setInt(0); // TODO
}

/**
 MSEC (integer)
 %status maybe
*/
void v_MSEC(caosVM* vm) {
	vm->result.setInt(engine.backend->ticks());
}

/**
 WPAU (command) paused (integer)
 %status stub
 
 If paused is 0, enable world ticks, otherwise (1) disable them.
*/
void c_WPAU(caosVM* vm) {
	VM_PARAM_INTEGER(paused)

	// TODO
}

/**
 WPAU (integer)
 %status stub

 Returns 1 if world ticks are paused, or 0 otherwise.
*/
void v_WPAU(caosVM* vm) {
	vm->result.setInt(0); // TODO
}

/**
 PAUS (command) paused (integer)
 %status maybe

 If paused is 0, unpause target agent, otherwise (1) pause it.
*/
void c_PAUS(caosVM* vm) {
	VM_PARAM_INTEGER(paused)

	valid_agent(vm->targ);

	vm->targ->paused = paused;
}

/**
 PAUS (integer)
 %status maybe

 Returns 1 if target agent is paused, or 0 otherwise.
*/
void v_PAUS(caosVM* vm) {
	valid_agent(vm->targ);

	if (vm->targ->paused)
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 RTIF (string) timestamp (integer) format (string)
 %status maybe

 Returns the result of strftime with the current timestamp and format.
*/
void v_RTIF(caosVM* vm) {
	VM_PARAM_STRING(format)
	VM_PARAM_INTEGER(timestamp)

	char buffer[1000]; // no idea what maximum length should be.. we can't really do this dynamically
	time_t tstamp = timestamp;
	strftime(buffer, 1000, format.c_str(), gmtime(&tstamp)); // TODO: does gmtime return null?

	vm->result.setString(buffer);
}

/**
 RTIM (integer)
 %status maybe
*/
void v_RTIM(caosVM* vm) {
	vm->result.setInt(time(0));
}

/**
 WTIK (integer)
 %status maybe
*/
void v_WTIK(caosVM* vm) {
	vm->result.setInt(world.worldtickcount);
}

/**
 RACE (integer)
 %status maybe
 
 Returns the time in milliseconds which the last tick took overall.
*/
void v_RACE(caosVM* vm) {
	if (world.race < 1) {
		// When in fast ticks mode on a fast computer, it is possible that a tick
		// can take less than a millisecond. This causes the Wolf Control GUI to
		// divide by zero. This is not an openc2e bug — the real engine behaves
		// this way! But it is annoying and there's no benefit to keeping that
		// behavior, so always return at least 1.
		vm->result.setInt(1);
		return;
	}
	vm->result.setInt(world.race);
}

/**
 ETIK (integer)
 %status maybe
*/
void v_ETIK(caosVM* vm) {
	vm->result.setInt(world.tickcount);
}

/**
 ASEA (command)
 %status stub
 %variants c2
*/
void c_ASEA(caosVM*) {
	// TODO
}

/**
 TMOD (integer)
 %status maybe
 %variants c2
*/
void v_TMOD(caosVM* vm) {
	vm->result.setInt(world.timeofday); // TODO
}

/**
 SEAV (integer)
 %status maybe
 %variants c2
*/
void v_SEAV(caosVM* vm) {
	// TODO: hardcoding bad?
	vm->result.setInt((world.dayofseason * 4) + world.timeofday);
}

/**
 DAYT (integer)
 %status stub
*/
void v_DAYT(caosVM* vm) {
	vm->result.setInt(0); // TODO
}

/**
 MONT (integer)
 %status stub
*/
void v_MONT(caosVM* vm) {
	vm->result.setInt(0); // TODO
}

/* vim: set noet: */
