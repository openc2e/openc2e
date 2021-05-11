/*
 *  caosVM_variables.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon May 31 2004.
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
#include "Catalogue.h"
#include "Engine.h"
#include "PointerAgent.h"
#include "Vehicle.h"
#include "World.h"
#include "caosVM.h"
#include "caos_assert.h"
#include "creatures/CreatureAgent.h"

#include <algorithm> // transform
#include <cctype> // toupper/tolower
#include <fmt/core.h>
#include <math.h> // abs()/fabs()
#include <memory>
#include <stdlib.h> // rand()

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

/**
 VAxx (variable)
 %status maybe
 %stackdelta 0
 %variants c2 cv c3 sm

 Script-local variables (exist only in the current script) with xx being from 00 to 99.  Examples: VA01, VA45. 
 */
/**
 VARx (variable)
 %status maybe
 %variants c1 c2
 %stackdelta 0

 Like VAxx, but restricted to 0-9. Legacy from Creatures 1.
*/
CAOS_LVALUE(VAxx,
			VM_PARAM_INTEGER(index);
			caos_assert(index >= 0 && index < 100),
			vm->var[index],
			vm->var[index] = newvalue)

/**
 MVxx (variable)
 %stackdelta 0
 %status maybe

 Like OVxx, only for OWNR, not TARG.
 */
CAOS_LVALUE_WITH(MVxx, vm->owner,
				 VM_PARAM_INTEGER(index);
				 caos_assert(index >= 0 && index < 100),
				 vm->owner->var[index],
				 vm->owner->var[index] = newvalue)

/**
 ADDS (command) var (variable) value (string)
 %status maybe

 Append the given string to the given variable.
*/
void c_ADDS(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_STRING(value)
	VM_PARAM_VARIABLE(variable)

	caos_assert(variable->hasString());
	variable->setString(variable->getString() + value);
}

/**
 SETS (command) var (variable) value (string)
 %status maybe

 Sets the given variable to the given string.
*/
void c_SETS(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_STRING(value)
	VM_PARAM_VARIABLE(var)
	var->reset();
	var->setString(value);
}

/**
 SETV (command) var (variable) value (decimal)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0
 %% Don't enable c1 or c2 here; we activate them with horrible hacks later

 Sets the given variable to the given decimal value.
 */
void c_SETV(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	// TODO: hackery for c2
	//VM_PARAM_DECIMAL(value)
	VM_PARAM_VALUE(value)
	VM_PARAM_VARIABLE(var)
	var->reset();

	// TODO: hackery for c2
	if (value.hasAgent()) {
		var->setAgent(value.getAgent());
		return;
	} else
		caos_assert(value.hasDecimal());

	if (value.hasFloat()) {
		var->setFloat(value.getFloat());
	} else { // VM_PARAM_DECIMAL guarantees us float || int
		var->setInt(value.getInt());
	}
}

/**
 SETA (command) var (variable) value (agent)
 %status maybe

 Sets the given variable to the given agent.
*/
void c_SETA(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_AGENT(value)
	VM_PARAM_VARIABLE(var)
	var->reset();
	var->setAgent(value);
}

/**
 OVxx (variable)
 %status maybe
 %stackdelta 0
 %variants c2 cv c3 sm

 Agent-local variables (exist only in the current agent's VM) from TARG, with xx being from 00 to 99.  Examples: OV01, OV45.
 */
/**
 OBVx (variable)
 %status maybe
 %stackdelta 0
 %variants c1 c2

 Like OVxx, but restricted to 0-2 in C1, or 0-9 in C2. Legacy from Creatures 1.
*/
// TODO: restrict to 0-2 in C1?
CAOS_LVALUE_TARG(OVxx,
				 VM_PARAM_INTEGER(index);
				 caos_assert(index >= 0 && index < 100),
				 vm->targ->var[index],
				 vm->targ->var[index] = newvalue)

/**
 TYPE (integer) value (anything)
 %status maybe

 Returns a integer value representing the type of data in 'value'.  0 is integer, 1 is float, 2 is string, 3 is agent.
*/
void v_TYPE(caosVM* vm) {
	VM_PARAM_VALUE(value)

	caos_assert(!value.isEmpty());

	if (value.hasInt())
		vm->result.setInt(0);
	else if (value.hasFloat())
		vm->result.setInt(1);
	else if (value.hasString())
		vm->result.setInt(2);
	else if (value.hasAgent()) {
		Agent* a = value.getAgent().get();
		if (a == nullptr)
			vm->result.setInt(-1);
		else if (typeid(*a) == typeid(SimpleAgent))
			vm->result.setInt(3);
		else if (typeid(*a) == typeid(PointerAgent))
			vm->result.setInt(4);
		else if (typeid(*a) == typeid(CompoundAgent))
			vm->result.setInt(5);
		else if (typeid(*a) == typeid(Vehicle))
			vm->result.setInt(6);
		else {
			CreatureAgent* c = dynamic_cast<CreatureAgent*>(a);
			if (c)
				vm->result.setInt(7);
			else
				vm->result.setInt(-2); // unknown agent
		}
	} else
		throw creaturesException("caosValue confused us terribly in TYPE");
}

/**
 MODV (command) var (variable) mod (integer)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Divides the given variable by the given integer, and returns the remainder (var % mod).
*/
void c_MODV(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(mod)
	VM_PARAM_VARIABLE(v) // integer
	if (!v->hasInt())
		throw badParamException();
	v->setInt(v->getInt() % mod);
}

/**
 ANDV (command) var (variable) and (integer)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Returns the result of a bitwise AND comparison of the given variable and the given integer (var & and).
*/
void c_ANDV(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(andv)
	VM_PARAM_VARIABLE(v)
	if (!v->hasInt())
		throw badParamException();
	v->setInt(v->getInt() & andv);
}

/**
 ORRV (command) var (variable) or (integer)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Returns the result of a bitwise OR comparison of the given variable and the given integer (var | or)
*/
void c_ORRV(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(orv)
	VM_PARAM_VARIABLE(v)
	if (!v->hasInt())
		throw badParamException();
	v->setInt(v->getInt() | orv);
}

/**
 ADDV (command) var (variable) add (decimal)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Adds the given decimal to the given variable and returns the result.
*/
void c_ADDV(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(add)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat() || add.hasFloat())
		v->setFloat(v->getFloat() + add.getFloat());
	else if (v->hasInt() && add.hasInt())
		v->setInt(v->getInt() + add.getInt());
	else if (add.hasFloat())
		v->setFloat(add.getFloat()); // default back to zero
	else
		v->setInt(add.getInt()); // default back to zero
}

/**
 SUBV (command) var (variable) sub (integer)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Subtracts the given integer from the given variable and returns the result.
*/
void c_SUBV(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(sub)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat() || sub.hasFloat())
		v->setFloat(v->getFloat() - sub.getFloat());
	else if (v->hasInt() && sub.hasInt())
		v->setInt(v->getInt() - sub.getInt());
	else
		throw badParamException();
}

/**
 NEGV (command) var (variable)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Returns the inverse of (negates) the given variable.  For example, 1 to -1, or -4 to 4.
*/
void c_NEGV(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat())
		v->setFloat(-v->getFloat());
	else if (v->hasInt())
		v->setInt(-v->getInt());
	else
		throw badParamException();
}

/**
 DIVV (command) var (variable) div (decimal)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0
 
 Divides the given variable by the given integer and returns the result.
*/
void c_DIVV(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(div)
	VM_PARAM_VARIABLE(v)

	caos_assert(div.hasDecimal());
	if (div.getFloat() == 0.0f)
		throw caosException("attempt to divide by zero");

	if ((engine.version < 3 && v->hasDecimal() && div.hasDecimal()) || (v->hasInt() && div.hasInt())) {
		// integer division
		v->setInt(v->getInt() / div.getInt());
	} else if (v->hasInt() || v->hasFloat()) {
		// floating point division
		v->setFloat(v->getFloat() / div.getFloat());
	} else
		throw badParamException();
}

/**
 MULV (command) var (variable) mul (decimal)
 %status maybe
 %variants c1 c2 cv c3 sm
 %cost c1,c2 0

 Multiplies the given variable by the given integer and returns the result.
*/
void c_MULV(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(mul)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat() || mul.hasFloat())
		v->setFloat(v->getFloat() * mul.getFloat());
	else if (v->hasInt() && mul.hasInt())
		v->setInt(v->getInt() * mul.getInt());
	else
		throw badParamException();
}

int calculateRand(int value1, int value2) {
	// TODO: i'm sure there's a better way to do this. tired. - fuzzie
	int diff;
	if (abs(value2) < abs(value1))
		diff = abs(value1 - value2) + 1;
	else
		diff = abs(value2 - value1) + 1;
	int val;
	if (value2 < value1)
		val = value2;
	else
		val = value1;
	double r = rand() / ((unsigned int)RAND_MAX + 1.0);

	return (int)(r * diff) + val;
}

/**
 RAND (integer) value1 (integer) value2 (integer)
 %status maybe

 Returns a random integer between 'value1' and 'value2', inclusive.
 */
void v_RAND(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(value2)
	VM_PARAM_INTEGER(value1)

	vm->result.setInt(calculateRand(value1, value2));
}

/**
 REAF (command)
 %status maybe

 Re-reads all catalogue files.
*/
void c_REAF(caosVM*) {
	VM_VERIFY_SIZE(0)

	catalogue.reset();
	world.initCatalogue();
}

/**
 UFOS (string)
 %status stub
 
 Returns 'uname -a' on platforms which support it, or OS details in another format otherwise.
*/
void v_UFOS(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	vm->result.setString("some random platform"); // TODO
}

/**
 MODU (string)
 %status stub
 %variants c3 cv sm openc2e

 Returns information about which modules are being used by the engine (for now, backend and audio backend names).
*/
void v_MODU(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	vm->result.setString(engine.getBackendName() + ", " + engine.getAudioBackendName());
	//result.setString("OriginalDisplay SDL (netbabel 148)"); // TODO
}

/**
 GNAM (string)
 %status maybe
 %variants c3 cv sm openc2e
 
 Returns the currently-running game (like "Creatures 1" or "Docking Station").
*/
void v_GNAM(caosVM* vm) {
	VM_VERIFY_SIZE(0)
	vm->result.setString(engine.getGameName());
}

/**
 ABSV (command) var (variable)
 %status maybe
 
 Modifies the given variable, if negative, so that its value is positive (absolute value).
*/
void c_ABSV(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VARIABLE(var)

	if (var->hasFloat())
		var->setFloat(fabs(var->getFloat()));
	else if (var->hasInt())
		var->setInt(abs(var->getInt()));
	else
		throw badParamException();
}

/**
 ACOS (float) x (float)
 %status maybe
 
 Returns the arccosine of x in degrees.
*/
void v_ACOS(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)

	double f = acos(x);
	f = f * 360;
	f = f / (M_PI * 2);
	vm->result.setFloat(f);
}

/**
 ASIN (float) x (float)
 %status maybe
 
 Returns the arcsine of x in degrees.
*/
void v_ASIN(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)

	double f = asin(x);
	f = f * 360;
	f = f / (M_PI * 2);
	vm->result.setFloat(f);
}

/**
 ATAN (float) x (float)
 %status maybe
 
 Returns the arctangent of x in degrees.
*/
void v_ATAN(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)

	double f = atan(x);
	f = f * 360;
	f = f / (M_PI * 2);
	vm->result.setFloat(f);
}

/**
 COS_ (float) x (float)
 %status maybe
 
 Returns the cosine of x in degrees.
*/
void v_COS_(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)

	double f = x * (M_PI * 2);
	f = f / 360;
	vm->result.setFloat(cos(f));
}

/**
 SIN_ (float) x (float)
 %status maybe
 
 Returns the sine of x in degrees.
*/
void v_SIN_(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)

	double f = x * (M_PI * 2);
	f = f / 360;

	vm->result.setFloat(sin(f));
}

/**
 TAN_ (float) x (float)
 %status maybe
 
 Returns the tangent of x in degrees.
*/
void v_TAN_(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)

	double f = x * (M_PI * 2);
	f = f / 360;

	vm->result.setFloat(tan(f));
}

/**
 SQRT (float) v (float)
 %status maybe
 
 Returns the square root of v.
*/
void v_SQRT(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)

	caos_assert(x >= 0); // no imaginary numbers for you!

	vm->result.setFloat(sqrt(x));
}

/**
 _P1_ (variable)
 %status maybe
 %variants c2 cv c3 sm

 The first argument given to the current script.
*/
CAOS_LVALUE_SIMPLE(P1, vm->_p_[0])

/**
 _P2_ (variable)
 %status maybe
 %variants c2 cv c3 sm

 The second argument given to the current script.
*/
CAOS_LVALUE_SIMPLE(P2, vm->_p_[1])

/**
 AVAR (variable) agent (agent) index (integer)
 %status maybe

 Returns the value of OVxx for the given agent, where xx is equal to 'index'.
*/
CAOS_LVALUE(AVAR,
			VM_VERIFY_SIZE(2)
				VM_PARAM_INTEGER(index)
					VM_PARAM_AGENT(agent)
						caos_assert(index >= 0 && index < 100);
			valid_agent(agent),
			agent->var[index],
			agent->var[index] = newvalue)

/**
 VTOS (string) value (decimal)
 %status maybe

 Returns a string representation of the given value.
*/
void v_VTOS(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_DECIMAL(value)

	if (value.hasInt()) {
		vm->result.setString(std::to_string(value.getInt()));
	} else {
		// TODO: this format isn't right (see OUTS also)
		vm->result.setString(fmt::format("{:0.06f}", value.getFloat()));
	}
}

/**
 CHAR (integer) str (string) index (integer)
 %status maybe

 Returns the character at position 'index' of the given string.
 Indexes start at 1.
*/
void v_CHAR(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(index)
	VM_PARAM_STRING(str)

	caos_assert(index >= 1);
	caos_assert(str.size() >= (unsigned int)index);

	vm->result.setInt(str[index - 1]);
}

/**
 CHAR (command) string (variable) index (integer) character (integer)
 %status maybe

 Sets the character at position 'index' of the given string.
 Indexes start at 1. 
*/
void c_CHAR(caosVM* vm) {
	VM_PARAM_INTEGER(character)
	VM_PARAM_INTEGER(index)
	VM_PARAM_VARIABLE(str)

	caos_assert(0 <= character && character <= 255);

	caos_assert(str->hasString());
	std::string mystr = str->getString();
	caos_assert(index >= 1);
	caos_assert(mystr.size() >= (unsigned int)index);

	mystr[index - 1] = (unsigned char)character;
	str->setString(mystr);
}

/**
 ITOF (float) number (integer)
 %status maybe

 Converts the given integer to a float.
*/
void v_ITOF(caosVM* vm) {
	VM_PARAM_FLOAT(number) // watson tells me this function is COMPLETELY pointless - fuzzie

	vm->result.setFloat(number);
}

/**
 FTOI (integer) number (float)
 %status maybe

 Converts the given float to an integer, by rounding.
*/
void v_FTOI(caosVM* vm) {
	VM_PARAM_INTEGER(number)

	vm->result.setInt(number);
}

/**
 STRL (integer) string (string)
 %status maybe

 Returns the length in characters of the given string.
*/
void v_STRL(caosVM* vm) {
	VM_PARAM_STRING(string)

	vm->result.setInt(string.size());
}

/**
 READ (string) tag (string) offset (integer)
 %status maybe

 Returns the value of the string at 'offset' inside the given catalogue tag.
 Offsets start at zero. Throws an error if tag doesn't exist.
*/
void v_READ(caosVM* vm) {
	VM_PARAM_INTEGER(offset)
	VM_PARAM_STRING(tag)

	// TODO: i'm not sure if we're meant to throw errors here. - fuzzie
	caos_assert(catalogue.hasTag(tag));
	const std::vector<std::string>& t = catalogue.getTag(tag);
	caos_assert(offset >= 0);
	caos_assert((unsigned int)offset < t.size());
	vm->result.setString(t[offset]);
}

/**
 REAQ (integer) tag (string)
 %status maybe

 Returns 1 if the specified catalogue tag exists, or 0 otherwise.
*/
void v_REAQ(caosVM* vm) {
	VM_PARAM_STRING(tag)

	if (catalogue.hasTag(tag))
		vm->result.setInt(1);
	else
		vm->result.setInt(0);
}

/**
 CATA (integer)
 %status maybe

 Returns the agent category of the TARG agent.
*/
void v_CATA(caosVM* vm) {
	valid_agent(vm->targ);

	vm->result.setInt(vm->targ->category);
}

/**
 CATI (integer) family (integer) genus (integer) species (integer)
 %status maybe

 Returns the agent category for the given family/genus/species.
*/
void v_CATI(caosVM* vm) {
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family) // TODO: check values are in range

	vm->result.setInt(world.findCategory(family, genus, species));
}

/**
 CATX (string) category_id (integer)
 %status maybe

 Returns a string containing the name of the given category.
*/
void v_CATX(caosVM* vm) {
	VM_PARAM_INTEGER(category_id)

	caos_assert(catalogue.hasTag("Agent Categories"));
	const std::vector<std::string>& t = catalogue.getTag("Agent Categories");
	if (category_id >= 0 && (unsigned int)category_id < t.size())
		vm->result.setString(t[category_id]);
	else
		vm->result.setString("");
}

/**
 CATO (command) category_id (integer)
 %status maybe

 Sets the agent category of the TARG agent. If the specified category is -1, sets the category based on the family/genus/species of the agent (see CATI).
*/
void c_CATO(caosVM* vm) {
	VM_PARAM_INTEGER(category_id)

	valid_agent(vm->targ);
	if (category_id == -1)
		vm->targ->category = world.findCategory(vm->targ->family, vm->targ->genus, vm->targ->species);
	else
		vm->targ->category = category_id;
}

/**
 WILD (string) family (integer) genus (integer) species (integer) tag (string) offset (integer)
 %status maybe

 Searches for a catalogue tag starting with 'tag' and matching the given
 family/genus/species.  For instance, 'Agent Help 2 3 4' where family is 2,
 genus is 3 and species is 4.  If it doesn't find it, it sets the minor value
 to 0 repeatedly until it either finds one, or discovers 'tag 0 0 0' doesn't
 exist, at which point an error is thrown.  If it does find a suitable one, it
 returns the string at offset inside the tag. See READ.
*/
void v_WILD(caosVM* vm) {
	VM_PARAM_INTEGER(offset)
	VM_PARAM_STRING(tag)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	std::string searchstring = catalogue.calculateWildcardTag(tag, family, genus, species); // calculate tag name
	caos_assert(searchstring.size()); // check we found a tag

	const std::vector<std::string>& t = catalogue.getTag(searchstring); // retrieve tag
	caos_assert(offset >= 0);
	caos_assert((unsigned int)offset < t.size()); // check the offset is useful for the tag we found

	vm->result.setString(t[offset]);
}

/**
 NAME (variable) name (anything)
 %status maybe

 Named, agent-local variables (like OVxx) in the TARG agent.
*/
CAOS_LVALUE_TARG(NAME, VM_PARAM_VALUE(name),
	vm->targ->name_variables[name],
	vm->targ->name_variables[name] = newvalue)

/**
 MAME (variable) name (anything)
 %status maybe

 Like NAME variables, except for OWNR rather than TARG.
*/
CAOS_LVALUE_WITH(MAME, vm->owner,
	VM_PARAM_VALUE(name),
	vm->owner->name_variables[name],
	vm->owner->name_variables[name] = newvalue)

/**
 SUBS (string) value (string) start (integer) count (integer)
 %status maybe

 Returns the text in a string starting at 'start' into the string (starting at 1), and with 'count' characters.
*/
void v_SUBS(caosVM* vm) {
	VM_PARAM_INTEGER(count)
	VM_PARAM_INTEGER(start)
	VM_PARAM_STRING(value)

	vm->result.setString(value.substr(start - 1, count)); // TODO: check start/count are valid?
}

/**
 STOI (integer) string (string)
 %status maybe

 Returns the provided string as an integer, or 0 if it can't be converted.
*/
void v_STOI(caosVM* vm) {
	VM_PARAM_STRING(string)

	vm->result.setInt(atoi(string.c_str()));
}

/**
 STOF (float) string (string)
 %status maybe

 Returns the provided string as a float, or 0 if it can't be converted.
*/
void v_STOF(caosVM* vm) {
	VM_PARAM_STRING(string)

	vm->result.setFloat(atof(string.c_str()));
}

/**
 LOWA (string) string (string)
 %status maybe

 Return a lower-cased version of a string.
*/
void v_LOWA(caosVM* vm) {
	VM_PARAM_STRING(string)

	std::transform(string.begin(), string.end(), string.begin(), (int (*)(int))tolower);
	vm->result.setString(string);
}

/**
 UPPA (string) string (string)
 %status maybe

 Return an upper-cased version of a string.
*/
void v_UPPA(caosVM* vm) {
	VM_PARAM_STRING(string)

	std::transform(string.begin(), string.end(), string.begin(), (int (*)(int))toupper);
	vm->result.setString(string);
}

/**
 SINS (integer) string (string) index (integer) searchstring (string)
 %status maybe

 Searches for an occurance of 'searchstring' inside 'string', starting at the given index inside 'string' (first character is 1).
 The index of the 'searchstring' inside the string is returned, or -1 if the searchstring wasn't found.
*/
void v_SINS(caosVM* vm) {
	VM_PARAM_STRING(searchstring)
	VM_PARAM_INTEGER(index)
	caos_assert(index > 0);
	VM_PARAM_STRING(string)

	// TODO: check for string having a size, perhaps?
	std::string::size_type i = string.find(searchstring, index - 1);

	if (i == string.npos)
		vm->result.setInt(-1);
	else
		vm->result.setInt(i + 1);
}

/**
 REAN (integer) tag (string)
 %status maybe
 
 Return number of strings associated with the catalogue tag specified.
*/
void v_REAN(caosVM* vm) {
	VM_PARAM_STRING(tag)

	if (!catalogue.hasTag(tag))
		vm->result.setInt(0);
	else {
		const std::vector<std::string>& t = catalogue.getTag(tag);
		vm->result.setInt(t.size());
	}
}

/**
 DELN (command) name (anything)
 %status maybe

 Delete the specified NAME variable on the target agent.
*/
void c_DELN(caosVM* vm) {
	VM_PARAM_VALUE(name)

	valid_agent(vm->targ);
	std::map<caosValue, caosValue, caosValueCompare>::iterator i = vm->targ->name_variables.find(name);
	if (i == vm->targ->name_variables.end())
		return;
	vm->targ->name_variables.erase(i);
}

/**
 NAMN (command) previous (variable)
 %status maybe
*/
void c_NAMN(caosVM* vm) {
	VM_PARAM_VARIABLE(previous)

	valid_agent(vm->targ);
	// TODO: we assume that NAME variables don't have an empty string
	if (previous->hasString() && previous->getString().empty()) {
		if (vm->targ->name_variables.size() == 0)
			previous->setString("");
		else
			*previous = vm->targ->name_variables.begin()->first;
	} else {
		std::map<caosValue, caosValue, caosValueCompare>::iterator i = vm->targ->name_variables.find(*previous);
		caos_assert(i != vm->targ->name_variables.end()); // TODO: this probably isn't correct behaviour
		i++;
		if (i == vm->targ->name_variables.end())
			previous->setString("");
		else
			*previous = i->first;
	}
}

/**
 * POWV (command) value (variable) exponent (decimal)
 * %status ok
 * %variants openc2e
 *
 * Raises value to the exponent power, and saves the result back in value.
 * Results are undefined if value is negative.
 *
 * Openc2e-only command
 */

void c_POWV(caosVM* vm) {
	VM_PARAM_FLOAT(exponent)
	VM_PARAM_VARIABLE(value)

	value->setFloat(powf(value->getFloat(), exponent));
}

/**
 RNDV (command) var (variable) value1 (integer) value (integer)
 %status maybe
 %variants c1 c2
 %cost c1,c2 0
*/
void c_RNDV(caosVM* vm) {
	VM_PARAM_INTEGER(value2)
	VM_PARAM_INTEGER(value1)
	VM_PARAM_VARIABLE(var)

	var->setInt(calculateRand(value1, value2));
}

/**
 EGGL (integer)
 %status stub
 %variants c2

 Returns the egg limit - when there are more than this many norns in the world, eggs should not hatch.
*/
void v_EGGL(caosVM* vm) {
	vm->result.setInt(16); // TODO
}

/**
 HATL (integer)
 %status stub
 %variants c2
 
 Returns the hatchery limit - when there are more than this many norns in the world, the hatchery should shut down.
*/
void v_HATL(caosVM* vm) {
	vm->result.setInt(12); // TODO
}

/* vim: set noet: */
