#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Reader;

std::string readfile(const std::string& filename);
std::string readfile(Reader& in);
std::vector<uint8_t> readfilebinary(const std::string& filename);
