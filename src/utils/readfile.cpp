#include "readfile.h"

#include <fstream>

std::string readfile(const std::string &filename) {
	std::ifstream in(filename, std::ios_base::binary);
	return readfile(in);
}

std::string readfile(std::istream &in) {
    return std::string(std::istreambuf_iterator<char>(in), {});
}

std::vector<uint8_t> readfilebinary(const std::string &filename) {
	std::ifstream in(filename, std::ios_base::binary);
	return readfilebinary(in);
}

std::vector<uint8_t> readfilebinary(std::istream &in) {
    return std::vector<uint8_t>(std::istreambuf_iterator<char>(in), {});
}
