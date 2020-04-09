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
#include "dialect.h"
#include "cmddata.h"
#include <fmt/printf.h>

const char *cnams[] = {
	NULL,
	"EQ",
	"LT",
	"LE",
	"GT",
	"GE",
	"NE",
	NULL
};

static std::string try_lookup(const Dialect *d, int idx) {
	if (d)
		return std::string(d->getcmd(idx)->fullname);
	return fmt::sprintf("%d", idx);
}

std::string dumpOp(const Dialect *d, caosOp op) {
	int arg = op.argument; // weird C++ issues
	switch (op.opcode) {
		case CAOS_NOP:
			return std::string("NOP");
		case CAOS_DIE:
			return fmt::sprintf("DIE %d", arg);
		case CAOS_STOP:
			return std::string("STOP");
		case CAOS_CMD:
			return fmt::sprintf("CMD %s", try_lookup(d, arg));
		case CAOS_COND:
			return fmt::sprintf("COND %s %s", (arg & CAND ? "AND" : "OR"), cnams[arg & CMASK]);
		case CAOS_CONST:
			return fmt::sprintf("CONST %d", arg);
		case CAOS_CONSTINT:
			return fmt::sprintf("CONSTINT %d", arg);
		case CAOS_BYTESTR:
			return fmt::sprintf("BYTESTR %d", arg);
		case CAOS_PUSH_AUX:
			return fmt::sprintf("PUSH AUX %d", arg);
		case CAOS_RESTORE_AUX:
			return fmt::sprintf("RESTORE AUX %d", arg);
		case CAOS_SAVE_CMD:
			return fmt::sprintf("CMD SAVE %s", try_lookup(d, arg));
		case CAOS_YIELD:
			return fmt::sprintf("YIELD %d", arg);
		case CAOS_STACK_ROT:
			return fmt::sprintf("STACK ROT %d", arg);

		case CAOS_CJMP:
			return fmt::sprintf("CJMP %08d", arg);
		case CAOS_JMP:
			return fmt::sprintf("JMP %08d", arg);
		case CAOS_DECJNZ:
			return fmt::sprintf("DECJNZ %08d", arg);
		case CAOS_GSUB:
			return fmt::sprintf("GSUB %08d", arg);
		case CAOS_ENUMPOP:
			return fmt::sprintf("ENUMPOP %08d", arg);
		default:
			return fmt::sprintf("UNKNOWN %02x %06x", arg);
	}
}
