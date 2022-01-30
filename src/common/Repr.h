#pragma once

#include <string>

namespace ghc {
namespace filesystem {
class path;
}
} // namespace ghc

std::string repr(const std::string& s);
std::string repr(const ghc::filesystem::path& p);