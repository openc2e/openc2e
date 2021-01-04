/*
 *  bytecode.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Thu 11 Aug 2005.
 *  Copyright (c) 2005-2006 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005-2006 Bryan Donlan. All rights reserved.
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
#ifndef BYTECODE_H
#define BYTECODE_H 1

#include "serfwd.h"
#include <stdlib.h> // for NULL
#include <assert.h>
#include <string>

enum opcode_t {
	/* IMPORTANT! The order of these elements must not change, or save
	 * compatibility will be broken.
	 *
	 * Note that relocated and non-relocated bytecode elements are treated
	 * seperately; you may add additional ones before CAOS_RELOCATABLE_BEGIN
	 * and before CAOS_INVALID.
	 */
	/* Do nothing.
	 * Argument: (ignored)
	 */
	CAOS_NOP = 0,
	/* Abort the script.
	 * Argument: An index into the constants table, with a description of the
	 *           error.
	 */
	CAOS_DIE,
	/* End the script gracefully.
	 * Argument: ignored
	 */
	CAOS_STOP,
	/* Invoke a caos command.
	 * Argument: An index into the commands table.
	 */
	CAOS_CMD,
	/* Pop two values A and B off the stack, as well as an integer flag C.
	 * Compare A and B with the given comparison mask, then push back C AND/OR
	 * the result, in accordance with the flag.
	 * Argument: A comparison flag
	 */
	CAOS_COND,
	/* Push a constant onto the stack.
	 * Argument: An index into the constants table
	 */
	CAOS_CONST,
	/* Push a constant integer in the range of a 24-bit int onto the stack.
	 * (ie, -2^24 <= x <= 2^24 - 1)
	 * Argument: An integer
	 */
	CAOS_CONSTINT,
	/* Copies the element (argument) elements from the top of the 
	 * argument stack into the top of the aux stack. The values remain on
	 * the argument stack.
	 * Argument: A zero-based index into the argument stack
	 */
	CAOS_PUSH_AUX,
	/* Moves the top (argument) elements from the top of the aux
	 * stack into the top of the argument stack. This effectively reverses
	 * their order. The values are removed from the aux stack.
	 * Argument: The number of elements to move
	 */
	CAOS_RESTORE_AUX,
	/* Invokes the writeback handler for the given CAOS command.
	 * Argument: An index into the commands table.
	 */
	CAOS_SAVE_CMD,
	/* Exhausts the specified number of time slices.
	 * Argument: A number of time slices to spend. Can be negative or zero.
	 */
	CAOS_YIELD,
	/* Moves the top element on the value stack down (argument) places.
	 * Argument: How many places to move it down
	 */
	CAOS_STACK_ROT,
	/* Pseudo-instructions; marks the beginning of relocated ops. */
	CAOS_NONRELOC_END,
	CAOS_RELOCATABLE_BEGIN = 0x40,
	/* Pop an integer off the stack. Jump to the given location if it's nonzero.
	 * Argument: A bytecode location (relocated).
	 * Cost: 0
	 */
	CAOS_CJMP,
	/* Jump to another location in the script.
	 * Argument: A bytecode location (relocated).
	 * Cost: 0
	 */
	CAOS_JMP,
	/* Pop a value off the stack. If it's nonzero, decrement, push back, 
	 * and jump to the location in the argument.
	 * Argument: A bytecode location (relocated).
	 * Cost: 0
	 */
	CAOS_DECJNZ,
	/* Push the instruction pointer and value stack into the call stack,
	 * then jump to the given location.
	 * Argument: A bytecode location (relocated).
	 * Cost: 0
	 */
	CAOS_GSUB,
	/* Pop a value off the stack. If non-null, set targ to it, and go to the
	 * given address. Otherwise, set targ to ownr, and continue.
	 * Argument: A bytecode location (relocated).
	 * Cost: 0
	 */
	CAOS_ENUMPOP,

	CAOS_INVALID
}; 

static inline bool op_is_valid(opcode_t opcode) {
	return (opcode >= 0 && opcode < CAOS_NONRELOC_END)
		|| (opcode >  CAOS_RELOCATABLE_BEGIN && opcode < CAOS_INVALID);
}

static inline bool op_is_relocatable(opcode_t opcode) {
	return (opcode >  CAOS_RELOCATABLE_BEGIN && opcode < CAOS_INVALID);
}

struct caosOp {
	enum opcode_t opcode : 8;
	int argument : 24;
	int traceindex; // -1 if unknown

	caosOp(enum opcode_t oc, int arg, int ti){
		assert(op_is_valid(oc));
		assert(arg >= -(1 << 24) && arg < (1 << 24));
		assert(ti >= -1);
		opcode = oc;
		argument = arg;
		traceindex = ti;
	}
private:
	FRIEND_SERIALIZE(caosOp)
	caosOp() { }
};

std::string dumpOp(const class Dialect *d, caosOp op);

// Condition classes
#define CEQ 1
#define CLT 2
#define CGT 4
#define CBT 8
#define CBF 16
#define CAND 32
#define COR  0
#define CMASK (CEQ | CLT | CGT | CBT | CBF)
#define CLE (CEQ | CLT)
#define CGE (CEQ | CGT)
#define CNE (CLT | CGT)

extern const char *cnams[];

#endif
/* vim: set noet: */
