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

 using the specified resource, returns the integer value associated with the given tag,
 or default if the tag doesn't exist
*/
void caosVM::v_PRAY_AGTI() {
}

/**
 PRAY AGTS (string) resource (string) tag (string) default (string)

 using the specified resource, returns the string value associated with the given tag,
 or default if the tag doesn't exist
*/
void caosVM::v_PRAY_AGTS() {
}

/**
 PRAY BACK (string) type (string) last (string)

 returns the name of the resource of the specified type which is immediately previous to last
 see PRAY PREV if you want to loop around
*/
void caosVM::v_PRAY_BACK() {
}

/**
 PRAY COUN (integer) type (string)

 return the number of resources of the specified type available
*/
void caosVM::v_PRAY_COUN() {
}

/**
 PRAY DEPS (integer) name (string) install (integer)
*/
void caosVM::v_PRAY_DEPS() {
}

/**
 PRAY EXPO (integer) type (string)
*/
void caosVM::v_PRAY_EXPO() {
}

/**
 PRAY FILE (integer) name (integer) type (integer) install (integer)

 install a file with given resource name and type
 if install is 0, the install doesn't actually happen, it's just tested
 returns 0 on success, 1 on failure
*/
void caosVM::v_PRAY_FILE() {
}

/**
 PRAY FORE (string) type (string) last (string)

 returns the name of the resource of the specified type which is immediately after last
 see PRAY NEXT if you don't want to loop around
*/
void caosVM::v_PRAY_FORE() {
}

/**
 PRAY GARB (command) force (integer)

 if force is 0, make the pray manager garbage-collect resources
 otherwise, make the pray manager empty its cache entirely

 recommended to be called after intensive PRAY usage, eg agent installation
*/
void caosVM::c_PRAY_GARB() {
}

/**
 PRAY IMPO (integer) moniker (string) doit (integer) keepfile (integer)
*/
void caosVM::v_PRAY_IMPO() {
}

/**
 PRAY INJT (integer) name (string) install (integer) report (variable)
*/
void caosVM::v_PRAY_INJT() {
}

/**
 PRAY KILL (integer) resource (string)

 deletes from disk the file containing the given resource

 returns 1 upon success, or 0 upon failure (typically no such resource)
*/
void caosVM::v_PRAY_KILL() {
}

/**
 PRAY MAKE (integer) journalspot (integer) journalname (string) prayspot (integer) name (string) report (variable)
*/
void caosVM::v_PRAY_MAKE() {
}

/**
 PRAY NEXT (string) type (string) last (string)

 returns the name of the resource of the specified type which is immediately after last
 see PRAY FORE if you don't want to loop around
*/
void caosVM::v_PRAY_NEXT() {
}

/**
 PRAY PREV (string) type (string) last (string)

 returns the name of the resource of the specified type which is immediately previous to last
 see PRAY BACK if you don't want to loop around
*/
void caosVM::v_PRAY_PREV() {
}

/**
 PRAY REFR (command)

 make the pray manager check for deleted/new files in the resource directory
*/
void caosVM::c_PRAY_REFR() {
}

/**
 PRAY TEST (integer) name (string)
*/
void caosVM::v_PRAY_TEST() {
}
