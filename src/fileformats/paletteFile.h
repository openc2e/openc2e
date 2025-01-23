#include "common/Image.h"
#include "common/shared_array.h"

#include <string>

class Reader;

shared_array<Color> ReadPaletteFile(const std::string& path);
shared_array<Color> ReadPaletteFile(Reader& in);