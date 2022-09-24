#include "Random.h"

#include <random>

static std::mt19937& default_rng() {
	// TODO: is this correct? meh, C++ random number generation is whack
	thread_local static std::mt19937 rng([] {
		std::random_device dev;
		std::seed_seq seed{dev(), dev(), dev(), dev()};
		return std::mt19937(seed);
	}());
	return rng;
}

bool rand_bool() {
	return std::uniform_int_distribution<int>(0, 1)(default_rng()) == 1;
}

uint8_t rand_uint8(uint8_t min, uint8_t max) {
	// uniform_int_distribution is not defined for uint8_t
	// compiles on some platforms but has undefined behavior, fails to compile on windows
	return static_cast<uint8_t>(std::uniform_int_distribution<int>(min, max)(default_rng()));
}

int32_t rand_int32(int32_t min, int32_t max) {
	return std::uniform_int_distribution<int32_t>(min, max)(default_rng());
}

uint32_t rand_uint32(uint32_t min, uint32_t max) {
	return std::uniform_int_distribution<uint32_t>(min, max)(default_rng());
}

size_t rand_size_t(size_t min, size_t max) {
	return std::uniform_int_distribution<size_t>(min, max)(default_rng());
}

float rand_float(float min, float max) {
	return std::uniform_real_distribution<float>(min, max)(default_rng());
}