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

#include "caosVM.h"
#include "openc2e.h"
#include "World.h"
#include "Engine.h"
#include <iostream>

#include <fmt/printf.h>

using std::cout;
using std::cerr;

/**
 OUTX (command) val (string)
 %status maybe

 Prints the given string on the output stream, after first quoting it and transforming 
 escapes in the string to quoted escapes.
*/
void caosVM::c_OUTX() {
	VM_PARAM_STRING(val)

	if (!outputstream) return;

	std::string oh = "\"";
	
	for (unsigned int i = 0; i < val.size(); i++) {
		switch (val[i]) {
			case '\r': oh += "\\r"; break;
			case '\n': oh += "\\n"; break;
			case '\t': oh += "\\t"; break;
			case '\\': oh += "\\\\"; break;
			case '"': oh += "\\\""; break;
			case '\'': oh += "\\'"; break;
			default: oh += val[i];
		}
	}

	*outputstream << oh << "\"";
}

/**
 OUTS (command) val (string)
 %status maybe
 %pragma variants all

 Prints the given string to the output stream.  Does nothing when run inside a script.
*/

/**
 DDE: PUTS (command) val (bareword)
 %status maybe
 %pragma variants c1 c2
*/

void caosVM::c_OUTS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(val)

	if (!outputstream) return;
	
	*outputstream << val;
}

/**
 OUTV (command) val (decimal)
 %status maybe
 %pragma variants all

 Prints the given decimal value to the ouput stream.  Does nothing when run inside a script.
*/

/**
 DDE: PUTV (command) val (integer)
 %status maybe
 %pragma variants c1 c2
*/

void caosVM::c_OUTV() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VALUE(val)

	if (!outputstream) return;
	
	if (val.hasFloat()) {
		*outputstream << fmt::sprintf("%0.06f", val.getFloat());
	} else if (val.hasInt()) {
		*outputstream << val.getInt();
	} else if (val.hasVector()) {
		const Vector<float> &v = val.getVector();
		*outputstream << fmt::sprintf("(%0.6f, %0.6f)", v.x, v.y);
	} else throw badParamException();
}

/**
 GAME (variable) name (string)
 %status maybe

 Returns the game variable with the given name.
*/
CAOS_LVALUE(GAME, VM_PARAM_STRING(name),
		world.variables[name],
		world.variables[name] = newvalue
		)

/**
 EAME (variable) name (anything)
 %status maybe

 Returns the non-persistent game variable with the given name.
*/
// XXX: should this be a string argument?
CAOS_LVALUE(EAME, VM_PARAM_VALUE(name),
		engine.eame_variables[name],
		engine.eame_variables[name] = newvalue
		)

/**
 DELG (command) name (string)
 %status maybe

 Deletes the game variable with the given name.
*/
void caosVM::c_DELG() {
	VM_PARAM_STRING(name)

	std::map<std::string, caosVar>::iterator i = world.variables.find(name);
	if (i != world.variables.end())
		world.variables.erase(i);
}

/**
 SCRP (command) family (integer) genus (integer) species (integer) event (integer)
 %status done
 %pragma variants c1 c2 cv c3 sm

 Marks the beginning of a normal script applying to the agent with the given classifier 
 info.
*/
void caosVM::c_SCRP() {
	// handled elsewhere
}	

/**
 RSCR (command)
 %status done
 %pragma variants c1 c2 cv c3 sm

 Marks the beginning of a removal script.
*/
void caosVM::c_RSCR() {
	// handled elsewhere
}	

/**
 ISCR (command)
 %status stub
 %pragma variants c1 c2 cv c3 sm

 Marks the beginning of an installer script.
*/
void caosVM::c_ISCR() {
	VM_VERIFY_SIZE(0)
	// STOP
}

/**
 ENDM (command)
 %status done
 %pragma variants c1 c2 cv c3 sm

 Marks the end of a script.
*/
void caosVM::c_ENDM() {
	stop();
}

/**
 RGAM (command)
 %status stub

 Restore all game variables to their saved or default values.
*/
void caosVM::c_RGAM() {}

/**
 MOWS (integer)
 %status stub

 Returns whether the lawn was cut last Sunday or not, in theory.
 How the C2E engine determines this, and whose lawn, exactly, and whether or not it takes into account the fact that the lawn may have been mown on Saturday or Friday, and whether it will cut you any slack if it's winter and the grass isn't growing much, is currently unknown.

 In openc2e, currently a no-op (ie, the lawn is never, ever cut properly).
*/
void caosVM::v_MOWS() {
	result.setInt(0); // We're too busy coding to mow the lawn.
}

/**
 VMNR (integer)
 %status maybe

 Returns the minor version number of the engine.
*/
void caosVM::v_VMNR() {
	result.setInt(1);
}

/**
 VMJR (integer)
 %status maybe

 Returns the major version number of the engine.
*/
void caosVM::v_VMJR() {
	result.setInt(0);
}

/**
 VRSN (command) required (integer)
 %status maybe
 %pragma variants c1 c2

 Stop running this script unless VRSN is equal to or greater than the specified value.
*/
void caosVM::c_VRSN() {
	VM_PARAM_INTEGER(required)

	// TODO: is this good for c1? which version is c2?
	int thisversion = (engine.version == 1) ? 2 : 0;

	if (thisversion < required) {
		std::cout << "Warning: stopping script due to version requirement of " << required << " (we are reporting a version of " << thisversion << ")" << std::endl;
		stop();
	}
}

/**
 VRSN (integer)
 %status maybe
 %pragma variants c1 c2

 Return the build version number of the engine.
*/
void caosVM::v_VRSN() {
	// TODO: is this good for c1? which version is c2?
	int thisversion = (engine.version == 1) ? 2 : 0;

	result.setInt(thisversion);
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
void caosVM::v_WOLF() {
	VM_PARAM_INTEGER(eormask)
	VM_PARAM_INTEGER(andmask)

	if (eormask & 1) engine.dorendering = !engine.dorendering;
	if (eormask & 2) engine.fastticks = !engine.fastticks;
	if (eormask & 4) engine.refreshdisplay = !engine.refreshdisplay;
	if (eormask & 8) world.autokill = !world.autokill;

	int r = 0;
	if (andmask & 1 && engine.dorendering) r += 1;
	if (andmask & 2 && engine.fastticks) r += 2;
	if (andmask & 4 && engine.refreshdisplay) r += 4;
	if (andmask & 8 && world.autokill) r += 8;
	result.setInt(r);
}

/**
 LANG (string)
 %status done
*/
void caosVM::v_LANG() {
	result.setString(engine.language);
}

/**
 TOKN (integer) token (bareword)
 %status maybe
 %pragma variants c1 c2
*/
void caosVM::v_TOKN() {
	VM_PARAM_STRING(token)

	caos_assert(token.size() == 4);

	int *data = (int *)token.c_str();
	result.setInt(*data);
}

/**
 GAME (variable) category (integer) variable (integer)
 %status stub
 %pragma variants c2
*/
CAOS_LVALUE(GAME_c2,
	VM_PARAM_INTEGER(variable) VM_PARAM_INTEGER(category),
	caosVar(),
	(void)0) // TODO

#include <ghc/filesystem.hpp>

/**
 OC2E DDIR (string)
 %status maybe
 %pragma variants all

 Returns a list of the data directories available, separated with \n. Remember that the last one is the working directory.
*/
void caosVM::v_OC2E_DDIR() {
	std::string d;

	for (std::vector<ghc::filesystem::path>::iterator i = world.data_directories.begin(); i != world.data_directories.end(); i++) {
		ghc::filesystem::path &p = *i;
		d = d + ghc::filesystem::absolute(p).string() + "\n";
	}
	
	result.setString(d);
}

/**
 SYS: CMND (command) menuid (integer)
 %status stub
 %pragma variants c1 c2

 Do something by providing a menu ID from the original Creatures 1 or Creatures 2 engines. This is obviously limited to the IDs that openc2e is aware of.
*/
void caosVM::c_SYS_CMND() {
	VM_PARAM_INTEGER(menuid)

	// TODO
}

/* vim: set noet: */
