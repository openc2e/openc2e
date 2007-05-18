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

#include <boost/format.hpp>

using std::cout;
using std::cerr;

using boost::format;

/**
 OUTX (command) val (string)
 %status maybe

 Prints the given string on the output stream, after first quoting it and transforming 
 escapes in the string to quoted escapes.
*/
void caosVM::c_OUTX() {
	VM_PARAM_STRING(val)
	
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

 Prints the given string to the output stream.  Does nothing when run inside a script.
*/
void caosVM::c_OUTS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(val)

	*outputstream << val;
}

/**
 DDE: PUTS (command) val (bareword)
 %status maybe
 %pragma variants c1 c2
 %pragma implementation caosVM::c_OUTS
*/

/**
 OUTV (command) val (decimal)
 %status maybe

 Prints the given decimal value to the ouput stream.  Does nothing when run inside a script.
*/
void caosVM::c_OUTV() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VALUE(val)

	if (val.hasFloat()) {
		*outputstream << boost::format("%0.06f") % val.getFloat();
	} else if (val.hasInt()) {
		*outputstream << val.getInt();
	} else if (val.hasVector()) {
		const Vector<float> &v = val.getVector();
		*outputstream << boost::format("(%0.6f, %0.6f)") % v.x % v.y;
	} else throw badParamException();
}

/**
 DDE: PUTV (command) val (integer)
 %status maybe
 %pragma variants c1 c2
 %pragma implementation caosVM::c_OUTV
*/

/**
 GAME (variable) name (string)
 %status maybe

 Returns the game variable with the given name.
*/
void caosVM::v_GAME() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(name)

	caosVar &i = world.variables[name];
	valueStack.push_back(&i);
}

/**
 EAME (variable) name (anything)
 %status maybe

 Returns the temporary game variable with the given name.
*/
void caosVM::v_EAME() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VALUE(name)

	caosVar &i = engine.eame_variables[name];
	valueStack.push_back(&i);
}


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
 %pragma noparse
 %pragma variants c1 c2 cv c3

 Marks the beginning of a normal script applying to the agent with the given classifier 
 info.
*/

/**
 RSCR (command)
 %status done
 %pragma noparse
 %pragma variants c2 cv c3

 Marks the beginning of a removal script.
*/

/**
 ISCR (command)
 %status stub
 %pragma variants c2 cv c3

 Marks the beginning of an installer script.
*/
void caosVM::c_ISCR() {
	VM_VERIFY_SIZE(0)
	// STOP
}

/**
 ENDM (command)
 %status done
 %pragma variants c1 c2 cv c3

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
 WOLF (integer) andmask (integer) eormask (integer)
 %status stub
*/
void caosVM::v_WOLF() {
	VM_PARAM_INTEGER(eormask)
	VM_PARAM_INTEGER(andmask)

	result.setInt(0); // TODO
}

/**
 LANG (string)
 %status stub
*/
void caosVM::v_LANG() {
	result.setString("en");
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
 %pragma implementation caosVM::v_GAME_c2
*/
void caosVM::v_GAME_c2() {
	VM_PARAM_INTEGER(variable)
	VM_PARAM_INTEGER(category)

	// TODO
	static caosVar hack;
	vm->valueStack.push_back(&hack);
}

/* vim: set noet: */
