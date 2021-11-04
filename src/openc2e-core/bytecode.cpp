/*
 *  bytecode.cpp
 *  openc2e
 *
 *  Created by Bryan Donlan on Wed 07 Dec 2005.
 *  Copyright (c) 2005 Bryan Donlan. All rights reserved.
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
#include "bytecode.h"

#include "cmddata.h"
#include "dialect.h"

#include <fmt/core.h>

const char* cnams[] = {
	NULL,
	"EQ",
	"LT",
	"LE",
	"GT",
	"GE",
	"NE",
	NULL};

static std::string try_lookup(const Dialect* d, int idx) {
	if (d)
		return std::string(d->getcmd(idx)->fullname);
	return std::to_string(idx);
}

std::string dumpOp(const Dialect* d, caosOp op) {
	int arg = op.argument; // weird C++ issues
	switch (op.opcode) {
		case CAOS_NOP:
			return "NOP";
		case CAOS_DIE:
			return fmt::format("DIE {}", arg);
		case CAOS_STOP:
			return "STOP";
		case CAOS_CMD:
			return fmt::format("CMD {}", try_lookup(d, arg));
		case CAOS_COND:
			return fmt::format("COND {} {}", (arg & CAND ? "AND" : "OR"), cnams[arg & CONDITIONMASK]);
		case CAOS_CONST:
			return fmt::format("CONST {}", arg);
		case CAOS_CONSTINT:
			return fmt::format("CONSTINT {}", arg);
		case CAOS_PUSH_AUX:
			return fmt::format("PUSH AUX {}", arg);
		case CAOS_RESTORE_AUX:
			return fmt::format("RESTORE AUX {}", arg);
		case CAOS_SAVE_CMD:
			return fmt::format("CMD SAVE {}", try_lookup(d, arg));
		case CAOS_YIELD:
			return "YIELD";
		case CAOS_STACK_ROT:
			return fmt::format("STACK ROT {}", arg);

		case CAOS_CJMP:
			return fmt::format("CJMP {:08d}", arg);
		case CAOS_JMP:
			return fmt::format("JMP {:08d}", arg);
		case CAOS_DECJNZ:
			return fmt::format("DECJNZ {:08d}", arg);
		case CAOS_GSUB:
			return fmt::format("GSUB {:08d}", arg);
		case CAOS_ENUMPOP:
			return fmt::format("ENUMPOP {:08d}", arg);
		default:
			return fmt::format("UNKNOWN {:02x} {:06x}", arg);
	}
}
