/*
 *  caosVM_ser_stub.cpp
 *  openc2e
 *
 *  Created by Bryan Donlan on Sun 21 May 2006
 *  Copyright (c) 2006 Bryan Donlan. All rights reserved.
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
 * SERS MAPP (command) filename (string)
 * %status maybe
 *
 * Saves the map to the specified file.
 */

	void c_SERS_MAPP(caosVM *vm) {
		VM_PARAM_STRING(filename)
		STUB;
	}

/**
 * SERL MAPP (command) filename (string)
 * %status maybe
 *
 * Loads the map from the specified file
 *
 * May explode if anything at all exists.
 */
	void c_SERL_MAPP(caosVM *vm) {
		VM_PARAM_STRING(filename)
		STUB;
	}

/**
 * SERS SCRP (command) filename (string)
 * %status maybe
 *
 * Saves the scriptorium to the specified file
 */
	void c_SERS_SCRP(caosVM *vm) {
		VM_PARAM_STRING(filename)
		STUB;
	}

/**
 * SERL SCRP (command) filename (string)
 * %status maybe
 *
 * Loads the scriptorium from the specified file
 */
	void c_SERL_SCRP(caosVM *vm) {
		VM_PARAM_STRING(filename)
		STUB;
	}
