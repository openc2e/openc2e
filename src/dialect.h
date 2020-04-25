#ifndef DIALECT_H
#define DIALECT_H 1

#include "cmddata.h"
#include <cassert>
#include <memory>
#include <string>
#include <map>

class Dialect {
	private:
		const int cmdcnt;
		const struct cmdinfo *cmds;
	public:
		const std::string name;
		const struct cmdinfo *cmdbase() const { return cmds; }
		const struct cmdinfo *getcmd(int idx) const {
			assert(idx >= 0 && idx < cmdcnt);
			return cmdbase() + idx;
		}
		int cmdcount() const { return cmdcnt; }

		const cmdinfo *find_command(const char *name) const;

		Dialect(const cmdinfo *cmds, const std::string &n);

		int cmd_index(const cmdinfo *) const;
};

std::vector<std::string> getDialectNames(); // defined in generated cmddata.cpp
Dialect* getDialectByName(const std::string&); // defined in generated cmddata.cpp

#endif

