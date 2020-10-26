#pragma once

#include <iosfwd>
#include "Image.h"

void WritePngFile(const Image& image, const std::string& path);
void WritePngFile(const Image& image, std::ostream& out);