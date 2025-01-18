#include "PathResolver.h"

#include <fmt/core.h>

int main(int argc, char** argv) {
	for (int i = 1; i < argc; i++) {
		std::string orig = argv[i];
		std::string res = orig;
		fmt::print("{} -> {}\n", orig, resolveFile(res) ? res : "(nil)");
	}
	return 0;
}
