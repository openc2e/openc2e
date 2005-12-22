/*
 *  caosVM_resources.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun Jun 13 2004.
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

/**
 PRAY AGTI (integer) resource (string) tag (string) default (integer)
 %status stub

 using the specified resource, returns the integer value associated with the given tag,
 or default if the tag doesn't exist
*/
void caosVM::v_PRAY_AGTI() {
	VM_PARAM_INTEGER(_default)
	VM_PARAM_STRING(tag)
	VM_PARAM_STRING(resource)

	result.setInt(0); // TODO
}

/**
 PRAY AGTS (string) resource (string) tag (string) default (string)
 %status stub

 using the specified resource, returns the string value associated with the given tag,
 or default if the tag doesn't exist
*/
void caosVM::v_PRAY_AGTS() {
	VM_PARAM_INTEGER(_default)
	VM_PARAM_STRING(tag)
	VM_PARAM_STRING(resource)

	result.setInt(0); // TODO
}

/**
 PRAY BACK (string) type (string) last (string)
 %status stub

 returns the name of the resource of the specified type which is immediately previous to last
 see PRAY PREV if you want to loop around
*/
void caosVM::v_PRAY_BACK() {
	VM_PARAM_STRING(last)
	VM_PARAM_STRING(type)

	result.setString(""); // TODO
}

/**
 PRAY COUN (integer) type (string)
 %status stub

 return the number of resources of the specified type available
*/
void caosVM::v_PRAY_COUN() {
	VM_PARAM_STRING(type)

	result.setInt(0); // TODO
}

/**
 PRAY DEPS (integer) name (string) install (integer)
 %status stub
*/
void caosVM::v_PRAY_DEPS() {
	VM_PARAM_INTEGER(install)
	VM_PARAM_STRING(name)

	result.setInt(0); // TODO
}

/**
 PRAY EXPO (integer) type (string)
 %status stub
*/
void caosVM::v_PRAY_EXPO() {
	VM_PARAM_STRING(type)

	result.setInt(0); // TODO
}

/**
 PRAY FILE (integer) name (integer) type (integer) install (integer)
 %status stub

 install a file with given resource name and type
 if install is 0, the install doesn't actually happen, it's just tested
 returns 0 on success, 1 on failure
*/
void caosVM::v_PRAY_FILE() {
	VM_PARAM_INTEGER(install)
	VM_PARAM_INTEGER(type)
	VM_PARAM_STRING(name)

	result.setInt(1); // TODO
}

/**
 PRAY FORE (string) type (string) last (string)
 %status stub

 returns the name of the resource of the specified type which is immediately after last
 see PRAY NEXT if you don't want to loop around
*/
void caosVM::v_PRAY_FORE() {
	VM_PARAM_STRING(last)
	VM_PARAM_STRING(type)

	result.setString(""); // TODO
}

/**
 PRAY GARB (command) force (integer)
 %status stub

 if force is 0, make the pray manager garbage-collect resources
 otherwise, make the pray manager empty its cache entirely

 recommended to be called after intensive PRAY usage, eg agent installation
*/
void caosVM::c_PRAY_GARB() {
	VM_PARAM_INTEGER(force)

	// TODO
}

/**
 PRAY IMPO (integer) moniker (string) doit (integer) keepfile (integer)
 %status stub
*/
void caosVM::v_PRAY_IMPO() {
	VM_PARAM_INTEGER(keepfile)
	VM_PARAM_INTEGER(doit)
	VM_PARAM_STRING(moniker)

	result.setInt(4); // TODO
}

/**
 PRAY INJT (integer) name (string) install (integer) report (variable)
 %status stub
*/
void caosVM::v_PRAY_INJT() {
	VM_PARAM_VARIABLE(report)
	VM_PARAM_INTEGER(install)
	VM_PARAM_STRING(name)

	result.setInt(-1); // TODO
}

/**
 PRAY KILL (integer) resource (string)
 %status stub

 deletes from disk the file containing the given resource

 returns 1 upon success, or 0 upon failure (typically no such resource)
*/
void caosVM::v_PRAY_KILL() {
	VM_PARAM_STRING(resource)

	result.setInt(0); // TODO
}

/**
 PRAY MAKE (integer) journalspot (integer) journalname (string) prayspot (integer) name (string) report (variable)
 %status stub
*/
void caosVM::v_PRAY_MAKE() {
	VM_PARAM_VARIABLE(report)
	VM_PARAM_STRING(name)
	VM_PARAM_INTEGER(prayspot)
	VM_PARAM_STRING(journalname)
	VM_PARAM_INTEGER(journalspot)

	result.setInt(1); // TODO
	report->setString("hat u");
}

/**
 PRAY NEXT (string) type (string) last (string)
 %status stub

 returns the name of the resource of the specified type which is immediately after last
 see PRAY FORE if you don't want to loop around
*/
void caosVM::v_PRAY_NEXT() {
	VM_PARAM_STRING(last)
	VM_PARAM_STRING(type)

	result.setString(""); // TODO
}

/**
 PRAY PREV (string) type (string) last (string)
 %status stub

 returns the name of the resource of the specified type which is immediately previous to last
 see PRAY BACK if you don't want to loop around
*/
void caosVM::v_PRAY_PREV() {
	VM_PARAM_STRING(last)
	VM_PARAM_STRING(type)

	result.setString(""); // TODO
}

/**
 PRAY REFR (command)
 %status stub

 make the pray manager check for deleted/new files in the resource directory
*/
void caosVM::c_PRAY_REFR() {
	// TODO
}

/**
 PRAY TEST (integer) name (string)
 %status stub
*/
void caosVM::v_PRAY_TEST() {
	VM_PARAM_STRING(name)
		
	result.setInt(0); // TODO
}

/* vim: set noet: */
