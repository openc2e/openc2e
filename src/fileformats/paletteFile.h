#include "common/Image.h"
#include "common/shared_array.h"

#include <iosfwd>
#include <string>

shared_array<Color> ReadPaletteFile(const std::string& path);
shared_array<Color> ReadPaletteFile(std::istream& in);