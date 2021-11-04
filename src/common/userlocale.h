#pragma once

#include <string>
#include <vector>

// C++ has no built-in cross-platform way to get the user's preferred language(s)

std::vector<std::string> get_preferred_languages();