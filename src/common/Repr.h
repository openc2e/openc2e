#pragma once

#include <string>

namespace ghc {
namespace filesystem {
class path;
}
} // namespace ghc

std::string repr(char);
std::string repr(const std::string&);
std::string repr(const ghc::filesystem::path&);