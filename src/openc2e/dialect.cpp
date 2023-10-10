#include "dialect.h"

#include "cmddata.h"
#include "common/Exception.h"

#include <cassert>
#include <memory>
#include <stdlib.h>
#include <string.h>

static int cmd_cmp(const void* pa, const void* pb) {
	const cmdinfo* a = (const cmdinfo*)pa;
	const cmdinfo* b = (const cmdinfo*)pb;

	return strcmp(a->lookup_key, b->lookup_key);
}

static int count_cmds(const struct cmdinfo* tbl) {
	int i = 0;
	while (tbl[i].lookup_key)
		i++;
	return i;
}

Dialect::Dialect(const cmdinfo* cmds_, const std::string& n)
	: cmdcnt(count_cmds(cmds_)), cmds(cmds_), name(n) {
}

const cmdinfo* Dialect::find_command(const char* name) const {
	cmdinfo key;
	key.lookup_key = name;
	return (const cmdinfo*)bsearch((void*)&key, (void*)cmds, cmdcnt, sizeof key, cmd_cmp);
}

const struct cmdinfo* Dialect::getcmd(int idx) const {
	assert(idx >= 0 && idx < cmdcnt);
	return cmdbase() + idx;
}

int Dialect::cmd_index(const cmdinfo* ci) const {
	assert(ci >= cmds && ci < cmds + cmdcnt);
	return (ci - cmds);
}

/* vim: set noet: */
