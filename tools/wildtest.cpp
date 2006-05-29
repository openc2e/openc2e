#include "PathResolver.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << (argc ? argv[0] : "wildtest") << " directory pattern" << std::endl;
        return EXIT_FAILURE;
    }

    std::string dir(argv[1]);
    std::string wild(argv[2]);
    std::vector<std::string> results = findByWildcard(dir, wild);

    for (
            std::vector<std::string>::iterator it = results.begin();
            it != results.end();
            it++
        )
    {
        std::cout << "Result: " << *it << std::endl;
    }
    return EXIT_SUCCESS;
}
