/*
 *  caosVM_cmdinfo.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun May 30 2004.
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

#define CMDDEF(x, y) { cmdinfo *tmp = &cmds[phash_cmd(*((int *)#x))]; \
	tmp->notokens = y; tmp->name = #x; tmp->method = &caosVM::c_ ## x; }
#define DBLCMDDEF(x) { cmdinfo *tmp = &cmds[phash_cmd(*(int *)x)]; \
	tmp->name = x; tmp->twotokens = true; }
#define DBLFUNCDEF(x) { cmdinfo *tmp = &funcs[phash_func(*(int *)x)]; \
	tmp->name = x; tmp->twotokens = true; }
/*#define CMDDEFCOLON(x, y) { std::string cmdname = std::string(#x) + ":"; \
	cmdinfo *tmp = &cmds[phash_cmd(*((int *)(cmdname.c_str())))]; \
	tmp->notokens = y; tmp->name = cmdname; tmp->method = &caosVM::c_ ## x; } */
#define FUNCDEF(x, y) { cmdinfo *tmp = &funcs[phash_func(*((int *)#x))]; \
	tmp->notokens = y; tmp->name = #x; tmp->method = &caosVM::v_ ## x; }
#define DBLCMD(x, y, z) { cmdinfo n(std::string(x), z, false, &caosVM::c_ ## y); \
	doublecmds.push_back(n); }
#define DBLFUNC(x, y, z) { cmdinfo n(std::string(x), z, false, &caosVM::v_ ## y); \
	doublefuncs.push_back(n); }

extern cmdinfo *cmds, *funcs;
extern std::vector<cmdinfo> doublecmds, doublefuncs;

// from the (soon to be) auto-generated caosdata.cpp
unsigned int phash_cmd(unsigned int val);
unsigned int phash_func(unsigned int val);
