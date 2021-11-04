#pragma once

#include "caostoken.h"

#include <string>
#include <vector>

void lexcaos(std::vector<caostoken>& v, const char* p);
void lexcaos(std::vector<caostoken>& v, const std::string& p);