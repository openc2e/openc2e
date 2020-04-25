#include "util.h"

#include <fstream>

std::string readfile(const std::string &filename) {
	std::ifstream in(filename);
	return readfile(in);
}

std::string readfile(std::istream &in) {
    return std::string(std::istreambuf_iterator<char>(in), {});
}
