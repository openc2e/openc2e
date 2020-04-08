#include "dialect.h"
#include "exceptions.h"
#include <stdlib.h>
#include <string.h>

static int cmd_cmp(const void *pa, const void *pb) {
	const cmdinfo *a = (const cmdinfo *)pa;
	const cmdinfo *b = (const cmdinfo *)pb;

	return strcmp(a->lookup_key, b->lookup_key);
}

static const cmdinfo *find_cmd(const struct cmdinfo *tbl, int cnt, const char *name) {
	cmdinfo key;
	key.lookup_key = name;
	return (const cmdinfo *)bsearch((void *)&key, (void *)tbl, cnt, sizeof key, cmd_cmp);
}

static int count_cmds(const struct cmdinfo *tbl) {
	int i = 0;
	while (tbl[i].lookup_key)
		i++;
	return i;
}

Dialect::Dialect(const cmdinfo *cmds_, const std::string &n) : cmdcnt(count_cmds(cmds_)), cmds(cmds_), name(n) {
}

const cmdinfo *Dialect::find_command(const char *name) const {
	return find_cmd(cmds, cmdcnt, name);
}

int Dialect::cmd_index(const cmdinfo *ci) const {
	assert(ci >= cmds && ci < cmds + cmdcnt);
	return (ci - cmds);
}

std::map<std::string, std::shared_ptr<Dialect> > dialects;

void registerDelegates() {
	registerAutoDelegates();
}

void freeDelegates() {
	dialects.clear();
}

/* vim: set noet: */
