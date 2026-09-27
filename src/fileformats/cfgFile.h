#pragma once

#include <filesystem>
#include <map>
#include <string>

class Reader;

std::map<std::string, std::string> readcfgfile(std::filesystem::path);
std::map<std::string, std::string> readcfgfile(Reader& in);