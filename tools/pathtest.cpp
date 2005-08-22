#include "PathResolver.h"
#include <iostream>

int main(int argc, char **argv) {
    for(int i = 1; i < argc; i++) {
        std::string orig = argv[i];
        std::string res = orig;
        std::cout << orig << " -> ";
        if (resolveFile(res))
            std::cout << res;
        else
            std::cout << "(nil)";
        std::cout << std::endl;
    }
    return 0;
}
