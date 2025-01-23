#pragma once

#include <ghc/filesystem.hpp>
#include <map>
#include <string>

class Reader;

std::map<std::string, std::string> readcfgfile(ghc::filesystem::path);
std::map<std::string, std::string> readcfgfile(Reader& in);