#pragma once

#include "common/Image.h"

#include <iosfwd>
#include <string>

void WritePngFile(const Image& image, const std::string& path);
void WritePngFile(const Image& image, std::ostream& out);
