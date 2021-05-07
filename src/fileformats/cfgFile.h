#pragma once

#include <ghc/filesystem.hpp>
#include <iosfwd>
#include <map>
#include <string>

std::map<std::string, std::string> readcfgfile(ghc::filesystem::path);
std::map<std::string, std::string> readcfgfile(std::istream& in);