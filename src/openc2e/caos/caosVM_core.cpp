/*
 *  caosVM_core.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
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

#include "Engine.h"
#include "PathResolver.h"
#include "World.h"
#include "caosVM.h"
#include "common/throw_ifnot.h"

#include <fmt/core.h>
#include <fmt/ostream.h>
#include <ghc/filesystem.hpp>

namespace fs = ghc::filesystem;

/**
 OUTX (command) val (string)
 %status maybe

 Prints the given string on the output stream, after first quoting it and transforming 
 escapes in the string to quoted escapes.
*/
void c_OUTX(caosVM* vm) {
	VM_PARAM_STRING(val)

	if (!vm->outputstream)
		return;

	std::string oh = "\"";

	for (char i : val) {
		switch (i) {
			case '\r': oh += "\\r"; break;
			case '\n': oh += "\\n"; break;
			case '\t': oh += "\\t"; break;
			case '\\': oh += "\\\\"; break;
			case '"': oh += "\\\""; break;
			case '\'': oh += "\\'"; break;
			default: oh += i;
		}
	}

	oh += "\"";
	fmt::print(*vm->outputstream, "{}", oh);
}

/**
 OUTS (command) val (string)
 %status maybe
 %variants c3 cv sm openc2e

 Prints the given string to the output stream.  Does nothing when run inside a script.
*/

/**
 DDE: PUTS (command) val (bareword)
 %status maybe
 %variants c1 c2
*/

void c_OUTS(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(val)

	if (!vm->outputstream)
		return;

	fmt::print(*vm->outputstream, "{}", val);
}

/**
 OUTV (command) val (decimal)
 %status maybe
 %variants c3 cv sm openc2e

 Prints the given decimal value to the ouput stream.  Does nothing when run inside a script.
*/

/**
 DDE: PUTV (command) val (integer)
 %status maybe
 %variants c1 c2
*/

void c_OUTV(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VALUE(val)

	if (!vm->outputstream)
		return;

	if (val.hasFloat()) {
		fmt::print(*vm->outputstream, "{:0.06f}", val.getFloat());
	} else if (val.hasInt()) {
		fmt::print(*vm->outputstream, "{}", val.getInt());
	} else if (val.hasVector()) {
		const Vector<float>& v = val.getVector();
		fmt::print(*vm->outputstream, "({:0.6f}, {:%0.6f})", v.x, v.y);
	} else
		throw badParamException();
}

/**
 GAME (variable) name (string)
 %status maybe

 Returns the game variable with the given name.
*/
CAOS_LVALUE(GAME, VM_PARAM_STRING(name),
	world.variables[name],
	world.variables[name] = newvalue)

/**
 GAME (variable) category (integer) variable (integer)
 %status stub
 %variants c2
*/
CAOS_LVALUE(GAME_c2,
	VM_PARAM_INTEGER_UNUSED(variable) VM_PARAM_INTEGER_UNUSED(category),
	caosValue(),
	(void)0) // TODO

/**
 RGAM (command)
 %status stub

 Refresh all settings that are read from game variables at startup.
*/
void c_RGAM(caosVM*) {
}

/**
 GAMN (string) previous (string)
 %status maybe
 
 Enumerates through game variable names, starting and ending with an empty string.
*/
void v_GAMN(caosVM* vm) {
	VM_PARAM_STRING(previous)

	// TODO: we assume that GAME variables don't have an empty string
	if (previous.empty()) {
		if (world.variables.size() == 0)
			vm->result.setString("");
		else
			vm->result.setString(world.variables.begin()->first);
	} else {
		std::map<std::string, caosValue>::iterator i = world.variables.find(previous);
		THROW_IFNOT(i != world.variables.end()); // TODO: this probably isn't correct behaviour
		i++;
		if (i == world.variables.end())
			vm->result.setString("");
		else
			vm->result.setString(i->first);
	}
}

/**
 DELG (command) name (string)
 %status maybe

 Deletes the game variable with the given name.
*/
void c_DELG(caosVM* vm) {
	VM_PARAM_STRING(name)
	world.variables.erase(name);
}

/**
 EAME (variable) name (string)
 %status maybe

 Returns the non-persistent game variable with the given name.
*/
CAOS_LVALUE(EAME, VM_PARAM_STRING(name),
	engine.eame_variables[name],
	engine.eame_variables[name] = newvalue)

/**
 EAMN (string) previous (string)
 %status maybe
 
 Enumerates through engine variable names, starting and ending with an empty string.
*/
void v_EAMN(caosVM* vm) {
	VM_PARAM_STRING(previous)

	// TODO: we assume that EAME variables don't have an empty string
	if (previous.empty()) {
		if (engine.eame_variables.size() == 0)
			vm->result.setString("");
		else
			vm->result.setString(engine.eame_variables.begin()->first);
	} else {
		std::map<std::string, caosValue>::iterator i = engine.eame_variables.find(previous);
		THROW_IFNOT(i != engine.eame_variables.end()); // TODO: this probably isn't correct behaviour
		i++;
		if (i == engine.eame_variables.end())
			vm->result.setString("");
		else
			vm->result.setString(i->first);
	}
}

/**
 DELE (command) name (string)
 %status maybe
 
 Deletes the engine variable with the given name.
*/
void c_DELE(caosVM* vm) {
	VM_PARAM_STRING(name);
	engine.eame_variables.erase(name);
}

/**
 SCRP (command) family (integer) genus (integer) species (integer) event (integer)
 %status done
 %variants c1 c2 cv c3 sm

 Marks the beginning of a normal script applying to the agent with the given classifier 
 info.
*/
void c_SCRP(caosVM*) {
	// handled elsewhere
}

/**
 RSCR (command)
 %status done
 %variants c1 c2 cv c3 sm

 Marks the beginning of a removal script.
*/
void c_RSCR(caosVM*) {
	// handled elsewhere
}

/**
 ISCR (command)
 %status stub
 %variants c1 c2 cv c3 sm

 Marks the beginning of an installer script.
*/
void c_ISCR(caosVM*) {
	VM_VERIFY_SIZE(0)
	// STOP
}

/**
 ENDM (command)
 %status done
 %variants c1 c2 cv c3 sm

 Marks the end of a script.
*/
void c_ENDM(caosVM* vm) {
	vm->stop();
}

/**
 MOWS (integer)
 %status stub

 Returns whether the lawn was cut last Sunday or not, in theory.
 How the C2E engine determines this, and whose lawn, exactly, and whether or not it takes into account the fact that the lawn may have been mown on Saturday or Friday, and whether it will cut you any slack if it's winter and the grass isn't growing much, is currently unknown.

 In openc2e, currently a no-op (ie, the lawn is never, ever cut properly).
*/
void v_MOWS(caosVM* vm) {
	vm->result.setInt(0); // We're too busy coding to mow the lawn.
}

/**
 VMNR (integer)
 %status maybe

 Returns the minor version number of the engine.
*/
void v_VMNR(caosVM* vm) {
	vm->result.setInt(1);
}

/**
 VMJR (integer)
 %status maybe

 Returns the major version number of the engine.
*/
void v_VMJR(caosVM* vm) {
	vm->result.setInt(0);
}

/**
 VRSN (command) required (integer)
 %status maybe
 %variants c1 c2

 Stop running this script unless VRSN is equal to or greater than the specified value.
*/
void c_VRSN(caosVM* vm) {
	VM_PARAM_INTEGER(required)

	// TODO: is this good for c1? which version is c2?
	int thisversion = (engine.version == 1) ? 2 : 0;

	if (thisversion < required) {
		fmt::print("Warning: stopping script due to version requirement of {} (we are reporting a version of {})\n", required, thisversion);
		vm->stop();
	}
}

/**
 VRSN (integer)
 %status maybe
 %variants c1 c2

 Return the build version number of the engine.
*/
void v_VRSN(caosVM* vm) {
	// TODO: is this good for c1? which version is c2?
	int thisversion = (engine.version == 1) ? 2 : 0;

	vm->result.setInt(thisversion);
}

/**
 WOLF (integer) andmask (integer) eormask (integer)
 %status maybe

 This returns/sets some engine settings which are useful for 'wolfing runs', among other things.
 Set andmask for the information you want returned, and eormask for the information you want changed.
 
 1 is for display rendering (turn it off to speed up the game)
 2 is for running ticks as fast as possible, rather than according to BUZZ
 4 is for refreshing the display (when display rendering is turned off, this will update the display at the end of the tick)
 (note that 4 is unset by the engine when the display is refreshed)
 8 is autokill
*/
void v_WOLF(caosVM* vm) {
	VM_PARAM_INTEGER(eormask)
	VM_PARAM_INTEGER(andmask)

	if (eormask & 1)
		engine.dorendering = !engine.dorendering;
	if (eormask & 2)
		engine.fastticks = !engine.fastticks;
	if (eormask & 4)
		engine.refreshdisplay = !engine.refreshdisplay;
	if (eormask & 8)
		world.autokill = !world.autokill;

	int r = 0;
	if (andmask & 1 && engine.dorendering)
		r += 1;
	if (andmask & 2 && engine.fastticks)
		r += 2;
	if (andmask & 4 && engine.refreshdisplay)
		r += 4;
	if (andmask & 8 && world.autokill)
		r += 8;
	vm->result.setInt(r);
}

/**
 LANG (string)
 %status done
*/
void v_LANG(caosVM* vm) {
	vm->result.setString(engine.language);
}

/**
 TOKN (integer) token (bareword)
 %status maybe
 %variants c1 c2
*/
void v_TOKN(caosVM* vm) {
	VM_PARAM_STRING(token)

	THROW_IFNOT(token.size() == 4);

	int* data = (int*)token.c_str();
	vm->result.setInt(*data);
}

/**
 OC2E DDIR (string)
 %status ok
 %variants openc2e

 Returns a list of the data directories available, separated with \n. Remember that the last one is the working directory.
*/
void v_OC2E_DDIR(caosVM* vm) {
	std::string d;

	for (auto p : getMainDirectories()) {
		d += fs::absolute(p).string() + "\n";
	}

	vm->result.setString(d);
}

/**
 SYS: CMND (command) menuid (integer)
 %status stub
 %variants c1 c2

 Do something by providing a menu ID from the original Creatures 1 or Creatures 2 engines. This is obviously limited to the IDs that openc2e is aware of.
*/
void c_SYS_CMND(caosVM* vm) {
	VM_PARAM_INTEGER_UNUSED(menuid)

	// TODO
}

/* vim: set noet: */
