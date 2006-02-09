/*
 *  caosVM_history.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Mon Dec 19 2005.
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

/**
 HIST CAGE (integer) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_CAGE() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setInt(0); // TODO
}

/**
 HIST COUN (integer) moniker (string)
 %status stub
*/
void caosVM::v_HIST_COUN() {
	VM_PARAM_STRING(moniker)

	result.setInt(0); // TODO
}

/**
 HIST CROS (integer) moniker (string)
 %status stub
*/
void caosVM::v_HIST_CROS() {
	VM_PARAM_STRING(moniker)

	result.setInt(0); // TODO
}

/**
 HIST EVNT (command) moniker (string) type (integer) relatedmoniker1 (string) relatedmoniker2 (string)
 %status stub

 Fire a life event of the specified type with the specified moniker.
*/
void caosVM::c_HIST_EVNT() {
	VM_PARAM_STRING(relatedmoniker2)
	VM_PARAM_STRING(relatedmoniker1)
	VM_PARAM_INTEGER(type)
	VM_PARAM_STRING(moniker)

	// TODO
}

/**
 HIST FIND (integer) moniker (string) event (integer) from (integer)
 %status stub
*/
void caosVM::v_HIST_FIND() {
	VM_PARAM_INTEGER(from)
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setInt(-1); // TODO
}

/**
 HIST FINR (integer) moniker (string) event (integer) from (integer)
 %status stub
*/
void caosVM::v_HIST_FINR() {
	VM_PARAM_INTEGER(from)
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setInt(-1); // TODO
}

/**
 HIST FOTO (string) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_FOTO() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setString(""); // TODO
}

/**
 HIST FOTO (command) moniker (string) event (integer) photo (string)
 %status stub
*/
void caosVM::c_HIST_FOTO() {
	VM_PARAM_STRING(photo)
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	// TODO
}

/**
 HIST GEND (integer) moniker (string)
 %status stub
*/
void caosVM::v_HIST_GEND() {
	VM_PARAM_STRING(moniker)
	
	result.setInt(-1); // TODO
}

/**
 HIST GNUS (integer) moniker (string)
 %status stub
*/
void caosVM::v_HIST_GNUS() {
	VM_PARAM_STRING(moniker)

	result.setInt(1); // TODO
}

/**
 HIST MON1 (string) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_MON1() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setString(""); // TODO
}

/**
 HIST MON2 (string) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_MON2() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setString(""); // TODO
}

/**
 HIST MUTE (integer) moniker (string)
 %status stub
*/
void caosVM::v_HIST_MUTE() {
	VM_PARAM_STRING(moniker)

	result.setInt(0); // TODO
}

/**
 HIST NAME (string) moniker (string)
 %status stub

 Return the name of the creature with the given moniker.
*/
void caosVM::v_HIST_NAME() {
	VM_PARAM_STRING(moniker)

	result.setString(""); // TODO
}

/**
 HIST NAME (command) moniker (string) name (string)
 %status stub
*/
void caosVM::c_HIST_NAME() {
	VM_PARAM_STRING(name)
	VM_PARAM_STRING(moniker)

	// TODO
}

/**
 HIST NETU (string) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_NETU() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setString(""); // TODO
}

/**
 HIST NEXT (string) moniker (string)
 %status stub
*/
void caosVM::v_HIST_NEXT() {
	VM_PARAM_STRING(moniker)
	
	result.setString(""); // TODO
}

/**
 HIST PREV (string) moniker (string)
 %status stub
*/
void caosVM::v_HIST_PREV() {
	VM_PARAM_STRING(moniker)

	result.setString(""); // TODO
}

/**
 HIST RTIM (integer) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_RTIM() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setInt(0); // TODO
}

/**
 HIST TAGE (integer) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_TAGE() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setInt(0); // TODO
}

/**
 HIST TYPE (integer) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_TYPE() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setInt(0); // TODO
}

/**
 HIST UTXT (command) moniker (string) event (integer) value (string)
 %status stub
*/
void caosVM::c_HIST_UTXT() {
	VM_PARAM_STRING(value)
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	// TODO
}

/**
 HIST UTXT (string) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_UTXT() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setString(""); // TODO
}

/**
 HIST VARI (integer) moniker (string)
 %status stub
*/
void caosVM::v_HIST_VARI() {
	VM_PARAM_STRING(moniker)

	result.setInt(0); // TODO
}

/**
 HIST WIPE (command) moniker (string)
 %status stub
*/
void caosVM::c_HIST_WIPE() {
	VM_PARAM_STRING(moniker)

	// TODO
}

/**
 HIST WNAM (string) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_WNAM() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setString(""); // TODO
}

/**
 HIST WTIK (integer) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_WTIK() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setInt(0); // TODO
}

/**
 HIST WUID (string) moniker (string) event (integer)
 %status stub
*/
void caosVM::v_HIST_WUID() {
	VM_PARAM_INTEGER(event)
	VM_PARAM_STRING(moniker)

	result.setString(""); // TODO
}

/**
 HIST WVET (integer) moniker (string)
 %status stub
*/
void caosVM::v_HIST_WVET() {
	VM_PARAM_STRING(moniker)
	
	result.setInt(0); // TODO
}

/**
 OOWW (integer) moniker (string)
 %status stub

 Return the current status of the given moniker.
 0 is unknown (never existed?), 1 is in slot, 2 is NEW: CREA creature, 3 is BORN creature, 4 is exported, 5 is dead, 6 is gone (no body), 7 is unreferenced.
*/
void caosVM::v_OOWW() {
	VM_PARAM_STRING(moniker)

	result.setInt(0); // TODO
}

/* vim: set noet: */
