#include "caosScript.h"
#include "utils/readfile.h"

#include <fmt/core.h>

int main(int argc, char** argv) {
	if (argc != 2) {
		fmt::print(stderr, "USAGE: {} FILE\n", argv[0]);
		return 1;
	}

	auto filename = argv[1];

	caosScript script("c3", filename);
	script.parse(readfile(filename));

	if (script.installer) {
		fmt::print("Install Script:\n");
		fmt::print("{}\n", script.installer->dump());
	}
	for (auto& s : script.scripts) {
		fmt::print("Script {} {} {} {}\n", s->fmly, s->gnus, s->spcs, s->scrp);
		fmt::print("{}\n", s->dump());
	}
	if (script.removal) {
		fmt::print("Remove Script:\n");
		fmt::print("{}\n", script.removal->dump());
	}

	return 0;
}