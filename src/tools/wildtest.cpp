#include "PathResolver.h"

#include <cstdlib>
#include <fmt/core.h>
#include <string>
#include <vector>

int main(int argc, char** argv) {
	if (argc != 3) {
		fmt::print(stderr, "Usage: {} directory pattern\n", argc ? argv[0] : "wildtest");
		return EXIT_FAILURE;
	}

	std::string dir(argv[1]);
	std::string wild(argv[2]);
	std::vector<std::string> results = findByWildcard(dir, wild);

	for (auto& result : results) {
		fmt::print("Result: {}\n", result);
	}
	return EXIT_SUCCESS;
}
