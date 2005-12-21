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
 OOWW (integer) moniker (string)
 %status stub

 Return the current status of the given moniker.
 0 is unknown (never existed?), 1 is in slot, 2 is NEW: CREA creature, 3 is BORN creature, 4 is exported, 5 is dead, 6 is gone (no body), 7 is unreferenced.
*/
void caosVM::v_OOWW() {
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

/* vim: set noet: */
