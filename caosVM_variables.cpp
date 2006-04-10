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

#include "caosVM.h"
#include "Agent.h"
#include <stdlib.h> // rand()
#include <iostream>
#include <math.h> // abs()/fabs()
#include "openc2e.h"
#include "World.h"
#include <cctype> // toupper/tolower
#include <algorithm> // transform
#include <boost/format.hpp>

#include "Vehicle.h"
#include "PointerAgent.h"
#include "Creature.h"

/**
 VAxx (variable)
 %pragma noparse
 %status maybe

 Script-local variables (exist only in the current script) with xx being from 00 to 99.  Examples: VA01, VA45. 
 */

/**
 MVxx (variable)
 %pragma noparse
 %status maybe

 Like OVxx, only for OWNR, not TARG.
 */

/**
 ADDS (command) var (variable) value (string)
 %status maybe

 Append the given string to the given variable.
*/
void caosVM::c_ADDS() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_STRING(value)
	VM_PARAM_VARIABLE(variable)

	assert(variable->hasString());
	variable->setString(variable->getString() + value);
}

/**
 SETS (command) var (variable) value (string)
 %status maybe

 Sets the given variable to the given string.
*/
void caosVM::c_SETS() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_STRING(value)
	VM_PARAM_VARIABLE(var)
	var->reset();
	var->setString(value);
}

/**
 SETV (command) var (variable) value (decimal)
 %status maybe

 Sets the given variable to the given decimal value.
 */
void caosVM::c_SETV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(value)
	VM_PARAM_VARIABLE(var)
	var->reset();
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
void caosVM::c_SETA() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_AGENT(value)
	VM_PARAM_VARIABLE(var)
	var->reset();
	var->setAgent(value);
}

/**
 OVxx (variable)
 %pragma noparse
 %status maybe

 Agent-local variables (exist only in the current agent's VM) from TARG, with xx being from 00 to 99.  Examples: OV01, OV45.
 */
// TODO: OVxx parser

/**
 TYPE (integer) value (anything)
 %status maybe

 Returns a integer value representing the type of data in 'value'.  0 is integer, 1 is float, 2 is string, 3 is agent.
*/
void caosVM::v_TYPE() {
	VM_PARAM_VALUE(value)

	caos_assert(!value.isEmpty());

	if (value.hasInt())
		result.setInt(0);
	else if (value.hasFloat())
		result.setInt(1);
	else if (value.hasString())
		result.setInt(2);
	else if (value.hasAgent()) {
		boost::shared_ptr<Agent> a = value.getAgent();
		if (a == 0)
			result.setInt(-1);
		else if (typeid(*a) == typeid(SimpleAgent))
			result.setInt(3);
		else if (typeid(*a) == typeid(PointerAgent))
			result.setInt(4);
		else if (typeid(*a) == typeid(CompoundAgent))
			result.setInt(5);
		else if (typeid(*a) == typeid(Vehicle))
			result.setInt(6);
		else {
			Creature *c = dynamic_cast<Creature *>(a.get());
			if (c)
				result.setInt(7);
			else
				result.setInt(-2); // unknown agent
		}
	} else
		throw creaturesException("caosVar confused us terribly in TYPE");
}

/**
 MODV (command) var (variable) mod (integer)
 %status maybe

 Divides the given variable by the given integer, and returns the remainder (var % mod).
*/
void caosVM::c_MODV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(mod)
	VM_PARAM_VARIABLE(v) // integer
	if (!v->hasInt()) throw badParamException();
	v->setInt(v->getInt() % mod);
}

/**
 ANDV (command) var (variable) and (integer)
 %status maybe

 Returns the result of a bitwise AND comparison of the given variable and the given integer (var & and).
*/
void caosVM::c_ANDV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(andv)
	VM_PARAM_VARIABLE(v)
	if (!v->hasInt()) throw badParamException();
	v->setInt(v->getInt() & andv);
}

/**
 ORRV (command) var (variable) or (integer)
 %status maybe

 Returns the result of a bitwise OR comparison of the given variable and the given integer (var | or)
*/
void caosVM::c_ORRV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(orv)
	VM_PARAM_VARIABLE(v)
	if (!v->hasInt()) throw badParamException();
	v->setInt(v->getInt() | orv);
}


/**
 ADDV (command) var (variable) add (integer)
 %status maybe

 Adds the given integer to the given variable and returns the result.
*/
void caosVM::c_ADDV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(add)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat())
		v->setFloat(v->getFloat() + (add.hasFloat() ? add.getFloat() : add.getInt()));
	else if (v->hasInt())
		v->setInt((int)(v->getInt() + (add.hasFloat() ? add.getFloat() : add.getInt())));
	else
		throw badParamException();
}

/**
 SUBV (command) var (variable) sub (integer)
 %status maybe

 Subtracts the given integer from the given variable and returns the result.
*/
void caosVM::c_SUBV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(sub)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat())
		v->setFloat(v->getFloat() - (sub.hasFloat() ? sub.getFloat() : sub.getInt()));
	else if (v->hasInt())
		v->setInt((int)(v->getInt() - (sub.hasFloat() ? sub.getFloat() : sub.getInt())));
	else
		throw badParamException();
}

/**
 NEGV (command) var (variable)
 %status maybe

 Returns the inverse of (negates) the given variable.  For example, 1 to -1, or -4 to 4.
*/
void caosVM::c_NEGV() {
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
 
 Divides the given variable by the given integer and returns the result.
*/
void caosVM::c_DIVV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(div)
	VM_PARAM_VARIABLE(v)
	if (v->hasInt() && div.hasInt()) {
		// integer division
		v->setInt(v->getInt() / div.getInt());
	} else if (v->hasInt() || v->hasFloat()) {
		// floating point division
		v->setFloat((v->hasFloat() ? v->getFloat() : v->getInt()) /
					(div.hasFloat() ? div.getFloat() : div.getInt()));
	} else
		throw badParamException();
}

/**
 MULV (command) var (variable) mul (decimal)
 %status maybe

 Multiplies the given variable by the given integer and returns the result.
*/
void caosVM::c_MULV() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_DECIMAL(mul)
	VM_PARAM_VARIABLE(v)
	if (v->hasFloat())
		v->setFloat(v->getFloat() * (mul.hasFloat() ? mul.getFloat() : mul.getInt()));
	else if (v->hasInt())
		v->setInt((int)(v->getInt() * (mul.hasFloat() ? mul.getFloat() : mul.getInt())));
	else
		throw badParamException();
}

/**
 RAND (integer) value1 (integer) value2 (integer)
 %status maybe

 Returns a random integer between 'value1' and 'value2', inclusive.
 */
void caosVM::v_RAND() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(value2)
	VM_PARAM_INTEGER(value1)

	int diff = value2 - value1 + 1;
	double r = (rand() / (double)((unsigned int)RAND_MAX + 1));

	result.setInt((int)((r * diff) + value1));

	// TODO: we should call srand at the start of our code somewhere
}

/**
 REAF (command)
 %status stub

 Re-reads all catalogue files.
 */
void caosVM::c_REAF() {
	VM_VERIFY_SIZE(0)
	// TODO
}

/**
 UFOS (string)
 %status stub
 
 Returns 'uname -a' on platforms which support it, or OS details in another format otherwise.
*/
void caosVM::v_UFOS() {
	VM_VERIFY_SIZE(0)
	result.setString("some random platform"); // TODO
}

/**
 MODU (string)
 %status stub
 
 Returns the Docking Station engine string for now.  Should return modules loaded and display engine type.
*/
void caosVM::v_MODU() {
	VM_VERIFY_SIZE(0)
	result.setString("OriginalDisplay SDL (netbabel 148)"); // TODO
}

/**
 GNAM (string)
 %status stub
 
 Returns the currently-running game (like "Docking Station").
*/
void caosVM::v_GNAM() {
	VM_VERIFY_SIZE(0)
	result.setString("Docking Station"); // TODO
}

/**
 ABSV (command) var (decimal variable)
 %status maybe
 
 Modifies the given variable, if negative, so that its value is positive (absolute value).
*/
void caosVM::c_ABSV() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VARIABLE(var)
	
	if (var->hasFloat()) var->setFloat(fabs(var->getFloat()));
	else if (var->hasInt()) var->setInt(abs(var->getInt()));
	else throw badParamException();
}

/**
 ACOS (float) x (float)
 %status maybe
 
 Returns the arccosine of x in degrees.
*/
void caosVM::v_ACOS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)

	double f = acos(x);
	f = f * 360;
	f = f / (M_PI * 2);
	result.setFloat(f);
}

/**
 ASIN (float) x (float)
 %status maybe
 
 Returns the arcsine of x in degrees.
*/
void caosVM::v_ASIN() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	double f = asin(x);
	f = f * 360;
	f = f / (M_PI * 2);
	result.setFloat(f);
}

/**
 ATAN (float) x (float)
 %status maybe
 
 Returns the arctangent of x in degrees.
*/
void caosVM::v_ATAN() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	double f = atan(x);
	f = f * 360;
	f = f / (M_PI * 2);
	result.setFloat(f);
}

/**
 COS_ (float) x (float)
 %status maybe
 
 Returns the cosine of x in degrees.
*/
void caosVM::v_COS_() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	double f = x * (M_PI * 2);	
	f = f / 360;
	result.setFloat(cos(f));
}

/**
 SIN_ (float) x (float)
 %status maybe
 
 Returns the sine of x in degrees.
*/
void caosVM::v_SIN_() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	double f = x * (M_PI * 2);	
	f = f / 360;
	
	result.setFloat(sin(f));
}

/**
 TAN_ (float) x (float)
 %status maybe
 
 Returns the tangent of x in degrees.
*/
void caosVM::v_TAN_() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)

	double f = x * (M_PI * 2);	
	f = f / 360;
	
	result.setFloat(tan(f));
}

/**
 SQRT (float) v (float)
 %status maybe
 
 Returns the square root of v.
*/
void caosVM::v_SQRT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(x)
	
	result.setFloat(sqrt(x));
}

/**
 _P1_ (variable)
 %pragma implementation caosVM::v_P1
 %status maybe

 The first argument given to the current script.
*/
void caosVM::v_P1() {
	VM_VERIFY_SIZE(0)

	vm->valueStack.push_back(&_p_[0]);
}

/**
 _P2_ (variable)
 %pragma implementation caosVM::v_P2
 %status maybe

 The second argument given to the current script.
*/
void caosVM::v_P2() {
	VM_VERIFY_SIZE(0)
		
	vm->valueStack.push_back(&_p_[1]);
}

/**
 AVAR (variable) agent (agent) index (integer)
 %status maybe

 Returns the value of OVxx for the given agent, where xx is equal to 'index'.
*/
void caosVM::v_AVAR() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(index)
	VM_PARAM_AGENT(agent)

	assert(index > -1);
	assert(index < 100);
	assert(agent);
	returnVariable(agent->var[index]);
}

/**
 VTOS (string) value (decimal)
 %status maybe

 Returns a string representation of the given value.
*/
void caosVM::v_VTOS() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_DECIMAL(value)

	if (value.hasInt()) {
		result.setString(boost::str(boost::format("%i") % value.getInt()));
	} else {
		// TODO: this format isn't right (see OUTS also)
		result.setString(boost::str(boost::format("%f") % value.getFloat()));
	}
}

/**
 CHAR (integer) str (string) index (integer)
 %status maybe

 Returns the character at position 'index' of the given string.
 Indexes start at 1.
*/
void caosVM::v_CHAR() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(index)
	VM_PARAM_STRING(str)

	caos_assert(str.size() >= index);
	
	result.setInt(str[index - 1]);
}

/**
 CHAR (command) string (variable) index (integer) character (integer)
 %status maybe

 Sets the character at position 'index' of the given string.
 Indexes start at 1. 
*/
void caosVM::c_CHAR() {
	VM_PARAM_INTEGER(character)
	VM_PARAM_INTEGER(index)
	VM_PARAM_VARIABLE(str)

	caos_assert(0 <= character && character <= 255);
	
	caos_assert(str->hasString());
	std::string mystr = str->getString();
	caos_assert(mystr.size() >= index);

	mystr[index - 1] = (unsigned char)character;
	str->setString(mystr);
}

/**
 ITOF (float) number (integer)
 %status maybe

 Converts the given integer to a float.
*/
void caosVM::v_ITOF() {
	VM_PARAM_FLOAT(number) // watson tells me this function is COMPLETELY pointless - fuzzie

	result.setFloat(number);
}

/**
 FTOI (integer) number (float)
 %status maybe

 Converts the given float to an integer, by rounding.
*/
void caosVM::v_FTOI() {
	VM_PARAM_INTEGER(number)

	result.setInt(number);
}

/**
 STRL (integer) string (string)
 %status maybe

 Returns the length in characters of the given string.
*/
void caosVM::v_STRL() {
	VM_PARAM_STRING(string)

	result.setInt(string.size());
}

/**
 READ (string) tag (string) offset (integer)
 %status maybe

 Returns the value of the string at 'offset' inside the given catalogue tag.
 Offsets start at zero. Throws an error if tag doesn't exist.
*/
void caosVM::v_READ() {
	VM_PARAM_INTEGER(offset)
	VM_PARAM_STRING(tag)

	// TODO: i'm not sure if we're meant to throw errors here. - fuzzie
	caos_assert(world.catalogue.hasTag(tag));
	const std::vector<std::string> &t = world.catalogue.getTag(tag);
	caos_assert(offset < t.size());
	result.setString(t[offset]);
}

/**
 REAQ (integer) tag (string)
 %status maybe

 Returns 1 if the specified catalogue tag exists, or 0 otherwise.
*/
void caosVM::v_REAQ() {
	VM_PARAM_STRING(tag)

	if (world.catalogue.hasTag(tag))
		result.setInt(1);
	else
		result.setInt(0);
}

int findCategory(unsigned char family, unsigned char genus, unsigned short species) {
	std::string buffer =  boost::str(boost::format("%i %i 0") % family % genus); // TODO: 0 is a hack so we don't have to do any real work here
	
	caos_assert(world.catalogue.hasTag("Agent Classifiers"));
	const std::vector<std::string> &t = world.catalogue.getTag("Agent Classifiers");
	for (unsigned int i = 0; i < t.size(); i++)
		if (t[i] == buffer) {
			return i;
		}

	return -1;
}

/**
 CATA (integer)
 %status maybe

 Returns the agent category of the TARG agent.
*/
void caosVM::v_CATA() {
	caos_assert(targ);

	result.setInt(findCategory(targ->family, targ->genus, targ->species));
}

/**
 CATI (integer) family (integer) genus (integer) species (integer)
 %status maybe

 Returns the agent category for the given family/genus/species.
*/
void caosVM::v_CATI() {
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family) // TODO: check values are in range

	result.setInt(findCategory(family, genus, species));
}

/**
 CATX (string) category_id (integer)
 %status maybe

 Returns a string containing the name of the given category.
*/
void caosVM::v_CATX() {
	VM_PARAM_INTEGER(category_id)

	caos_assert(world.catalogue.hasTag("Agent Categories"));
	const std::vector<std::string> &t = world.catalogue.getTag("Agent Categories");
	if (-1 < category_id && category_id < t.size())
		result.setString(t[category_id]);
	else
		result.setString("");
		
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
void caosVM::v_WILD() {
	VM_PARAM_INTEGER(offset)
	VM_PARAM_STRING(tag)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)

	std::string searchstring = world.catalogue.calculateWildcardTag(tag, family, genus, species); // calculate tag name
	caos_assert(searchstring.size()); // check we found a tag

	const std::vector<std::string> &t = world.catalogue.getTag(searchstring); // retrieve tag
	caos_assert(offset < t.size()); // check the offset is useful for the tag we found

	result.setString(t[offset]);
}

/**
 NAME (variable) name (anything)
 %status maybe

 Named, agent-local variables (like OVxx) in the TARG agent.
*/
void caosVM::v_NAME() {
	VM_PARAM_VALUE(name)
		
	caos_assert(targ);
	valueStack.push_back(&targ->name_variables[name]);
}

/**
 MAME (variable) name (anything)
 %status maybe

 Like NAME variables, except for OWNR rather than TARG.
*/
void caosVM::v_MAME() {
	VM_PARAM_VALUE(name)

	caos_assert(owner);
	valueStack.push_back(&owner->name_variables[name]);
}

/**
 SUBS (string) value (string) start (integer) count (integer)
 %status maybe

 Returns the text in a string starting at 'start' into the string (starting at 1), and with 'count' characters.
*/
void caosVM::v_SUBS() {
	VM_PARAM_INTEGER(count)
	VM_PARAM_INTEGER(start)
	VM_PARAM_STRING(value)

	result.setString(value.substr(start - 1, count)); // TODO: check start/count are valid?
}

/**
 STOI (integer) string (string)
 %status maybe

 Returns the provided string as an integer, or 0 if it can't be converted.
*/
void caosVM::v_STOI() {
	VM_PARAM_STRING(string)

	result.setInt(atoi(string.c_str()));
}

/**
 STOF (float) string (string)
 %status maybe

 Returns the provided string as a float, or 0 if it can't be converted.
*/
void caosVM::v_STOF() {
	VM_PARAM_STRING(string)
		
	result.setFloat(atof(string.c_str()));
}

/**
 LOWA (string) string (string)
 %status maybe

 Return a lower-cased version of a string.
*/
void caosVM::v_LOWA() {
	VM_PARAM_STRING(string)

	std::transform(string.begin(), string.end(), string.begin(), (int(*)(int))tolower);
	result.setString(string);
}

/**
 UPPA (string) string (string)
 %status maybe

 Return an upper-cased version of a string.
*/
void caosVM::v_UPPA() {
	VM_PARAM_STRING(string)

	std::transform(string.begin(), string.end(), string.begin(), (int(*)(int))toupper);
	result.setString(string);
}

/**
 SINS (integer) string (string) index (integer) searchstring (string)
 %status maybe

 Searches for an occurance of 'searchstring' inside 'string', starting at the given index inside 'string' (first character is 1).
 The index of the 'searchstring' inside the string is returned, or -1 if the searchstring wasn't found.
*/
void caosVM::v_SINS() {
	VM_PARAM_STRING(searchstring)
	VM_PARAM_INTEGER(index) caos_assert(index > 0);
	VM_PARAM_STRING(string)

	// TODO: check for string having a size, perhaps?
	std::string::size_type i = string.find(searchstring, index - 1);

	if (i == string.npos)
		result.setInt(-1);
	else
		result.setInt(i + 1);
}

/**
 REAN (integer) tag (string)
 %status maybe
 
 Return number of strings associated with the catalogue tag specified.
*/
void caosVM::v_REAN() {
	VM_PARAM_STRING(tag)
		
	if (!world.catalogue.hasTag(tag))
		result.setInt(0);
	else {
		const std::vector<std::string> &t = world.catalogue.getTag(tag);
		result.setInt(t.size());
	}
}

/**
 DELN (command) name (anything)
 %status maybe

 Delete the specified NAME variable on the target agent.
*/
void caosVM::c_DELN() {
	VM_PARAM_VALUE(name)

	caos_assert(targ);
	std::map<caosVar, caosVar, caosVarCompare>::iterator i = targ->name_variables.find(name);
	caos_assert(i != targ->name_variables.end()); // TODO: correct behaviour, or should we just return?
	targ->name_variables.erase(i);
}

/**
 GAMN (string) previous (string)
 %status maybe
*/
void caosVM::v_GAMN() {
	VM_PARAM_STRING(previous)

	// TODO: we assume that GAME variables don't have an empty string
	if (previous.empty()) {
		if (world.variables.size() == 0)
			result.setString("");
		else
			result.setString(world.variables.begin()->first);
	} else {
		std::map<std::string, caosVar>::iterator i = world.variables.find(previous);
		caos_assert(i != world.variables.end()); // TODO: this probably isn't correct behaviour
		i++;
		if (i == world.variables.end())
			result.setString("");
		else
			result.setString(i->first);
	}
}

/**
 NAMN (command) previous (variable)
 %status maybe
*/
void caosVM::c_NAMN() {
	VM_PARAM_VARIABLE(previous)

	caos_assert(targ);
	// TODO: we assume that NAME variables don't have an empty string
	if (previous->hasString() && previous->getString().empty()) {
		if (targ->name_variables.size() == 0)
			previous->setString("");
		else
			*previous = targ->name_variables.begin()->first;
	} else {
		std::map<caosVar, caosVar, caosVarCompare>::iterator i = targ->name_variables.find(*previous);
		caos_assert(i != targ->name_variables.end()); // TODO: this probably isn't correct behaviour
		i++;
		if (i == targ->name_variables.end())
			previous->setString("");
		else
			*previous = i->first;
	}
}

/* vim: set noet: */
