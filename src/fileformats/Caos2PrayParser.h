#pragma once

#include "PraySourceParser.h"

#include <string>
#include <vector>

namespace Caos2PrayParser {
std::vector<PraySourceParser::Event> parse(const std::string& script, std::string* output_filename);
}
