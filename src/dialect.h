#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <map>
#include <vector>

struct cmdinfo;

class Dialect {
	private:
		const int cmdcnt;
		const struct cmdinfo *cmds;
	public:
		const std::string name;
		const struct cmdinfo *cmdbase() const { return cmds; }
		const struct cmdinfo *getcmd(int idx) const;
		int cmdcount() const { return cmdcnt; }

		const cmdinfo *find_command(const char *name) const;
		const cmdinfo *find_command(const std::string &name) const {
			return find_command(name.c_str());
		}

		Dialect(const cmdinfo *cmds, const std::string &n);

		int cmd_index(const cmdinfo *) const;
};

std::vector<std::string> getDialectNames(); // defined in generated cmddata.cpp
Dialect* getDialectByName(const std::string&); // defined in generated cmddata.cpp
