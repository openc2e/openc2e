#include "paletteFile.h"

#include <array>
#include <fstream>

shared_array<Color> ReadPaletteFile(const std::string& path) {
	std::ifstream in(path, std::ios::binary);
	return ReadPaletteFile(in);
}

shared_array<Color> ReadPaletteFile(std::istream& in) {
	std::array<uint8_t, 768> palette_data;
	in.read((char*)palette_data.data(), 768);

	shared_array<Color> palette(256);
	for (unsigned int i = 0; i < 256; i++) {
		palette[i].r = palette_data[i * 3] * 4;
		palette[i].g = palette_data[i * 3 + 1] * 4;
		palette[i].b = palette_data[i * 3 + 2] * 4;
		palette[i].a = 0xff;
	}
	return palette;
}