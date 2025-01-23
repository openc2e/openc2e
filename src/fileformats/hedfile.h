#pragma once

#include <cstdint>
#include <string>

class Reader;

struct hedfile {
	uint32_t frame_width;
	uint32_t frame_height;
	uint32_t numframes;
};

hedfile read_hedfile(const std::string& path);
hedfile read_hedfile(Reader& in);