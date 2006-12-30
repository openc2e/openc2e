/*
 *  caosVM_variables.cpp
 *  openc2e
 *
 *  Created by Bryan Donlan on Sun Jul 23 2006
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
#include <cmath>
#include "caosVar.h"
#include "physics.h"

#define PI (atanf(1)*4)

/**
 * VEC: MAKE (vector) x (float) y (float)
 * %status maybe
 *
 * Creates and returns a vector with the given components.
 *
 * Openc2e-only command
 */

	void caosVM::v_VEC_MAKE() {
		VM_PARAM_FLOAT(y)
		VM_PARAM_FLOAT(x)

		result.setVector(Vector<float>(x, y));
	}

/**
 * VEC: GETC (command) vec (vector) x (variable) y (variable)
 * %status maybe
 *
 * Extracts the components of vector vec and places them in x and y.
 *
 * Openc2e-only command
 */

	void caosVM::c_VEC_GETC() {
		VM_PARAM_VARIABLE(y)
		VM_PARAM_VARIABLE(x)
		VM_PARAM_VECTOR(vec)

		x->setFloat(vec.x);
		y->setFloat(vec.y);
	}

/**
 * VEC: ANGL (float) vec (vector)
 * %status maybe
 *
 * Find and return the angle from the X-axis of the given vector.
 * This is computed using atan(y/x) if X is nonzero, and a hard-coded
 * return if X is nonzero. If the input vector is the null vector (0,0),
 * zero will be returned.
 *
 * The returned angle is in degrees, and in the range -180 to 180 degrees.
 *
 * Openc2e-only command.
 */

void caosVM::v_VEC_ANGL() {
	float ret = 0;
	VM_PARAM_VECTOR(vec)

	if (vec.x != 0) {
		ret = atanf(fabsf(vec.y/vec.x))*180/PI;
		if (vec.x < 0)
			ret = 180 - ret;
		if (vec.y < 0)
			ret = -ret;
	}
	else if (vec.y > 0)
		ret = 90;
	else if (vec.y < 0)
		ret = -90;
	else if (vec.y == 0)
		ret = 0;
	if (ret == -180)
		ret = 180; // hacky -_-;;

	result.setFloat(ret);
}

/**
 * VEC: SUBV (command) vec1 (variable) vec2 (vector)
 * %status maybe
 *
 * Subtracts vec2 from vec1 and stores the result in vec1.
 *
 * Openc2e-only command.
 */

	void caosVM::c_VEC_SUBV() {
		VM_PARAM_VECTOR(vec2)
		VM_PARAM_VARIABLE(vec1)

		if (!vec1->hasVector())
			throw badParamException();

		vec1->setVector(vec1->getVector() - vec2);
	}

/**
 * VEC: ADDV (command) vec1 (variable) vec2 (vector)
 * %status maybe
 *
 * Adds vec1 to vec2 and stores the result in vec1.
 *
 * Openc2e-only command.
 */

	void caosVM::c_VEC_ADDV() {
		VM_PARAM_VECTOR(vec2)
		VM_PARAM_VARIABLE(vec1)

		if (!vec1->hasVector())
			throw badParamException();

		vec1->setVector(vec1->getVector() + vec2);
	}

/**
 * VEC: MULV (command) vec (variable) mag (decimal)
 * %status maybe
 *
 * Multiplies the magnitude of the vector vec by mag, and stores the result
 * in vec.
 *
 * Openc2e-only command
 */

	void caosVM::c_VEC_MULV() {
		VM_PARAM_FLOAT(mag)
		VM_PARAM_VARIABLE(vec)

		if (!vec->hasVector())
			throw badParamException();
		vec->setVector(vec->getVector().scale(mag));
	}

/**
 * VEC: UNIT (vector) angle (decimal)
 * %status maybe
 *
 * Constructs and returns a unit vector with angle angle.
 *
 * Openc2e-only command,
 */

	void caosVM::v_VEC_UNIT() {
		VM_PARAM_FLOAT(angle)

		result.setVector(Vector<float>::unitVector(angle * PI / 180));
	}

/**
 * VEC: NULL (vector)
 * %status maybe
 *
 * Returns the nullary (0,0) vector.
 *
 * Openc2e-only command
 */

	void caosVM::v_VEC_NULL() {
		result.setVector(Vector<float>(0,0));
	}

/**
 * VEC: MAGN (float) vec (vector)
 * %status maybe
 *
 * Returns the magnitude of the passed vector.
 *
 * Openc2e-only command.
 */
	void caosVM::v_VEC_MAGN() {
		VM_PARAM_VECTOR(vec)

		result.setFloat(vec.getMagnitude());
	}

/**
 * VEC: SETV (command) dest (variable) src (vector)
 * %status maybe
 *
 * Sets the variable passed in dest to the vector in src
 *
 * Openc2e-only command
 */
	void caosVM::c_VEC_SETV() {
		VM_PARAM_VECTOR(src)
		VM_PARAM_VARIABLE(dest)

		dest->setVector(src);
	}

/* vim: set noet: */
