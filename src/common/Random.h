#pragma once

#include <stddef.h>
#include <stdint.h>

bool rand_bool();
uint8_t rand_uint8(uint8_t min, uint8_t max);
int32_t rand_int32(int32_t min, int32_t max);
uint32_t rand_uint32();
uint32_t rand_uint32(uint32_t min, uint32_t max);
size_t rand_size_t(size_t min, size_t max);
float rand_float(float min, float max);

template <typename Range>
decltype(auto) rand_choice(Range&& r) {
	return r[rand_size_t(0, r.size() - 1)];
}