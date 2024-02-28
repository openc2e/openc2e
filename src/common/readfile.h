#pragma once

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

std::string readfile(const std::string& filename);
std::string readfile(std::istream& in);
std::vector<uint8_t> readfilebinary(const std::string& filename);
std::vector<uint8_t> readfilebinary(std::istream& in);
