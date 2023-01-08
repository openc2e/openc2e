#pragma once

#include <stdint.h>

// Named according to P0543R2
// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p0543r2.html

uint8_t add_sat(uint8_t left, uint8_t right) {
	uint8_t val = left + right;
	return val < left ? 255 : val;
}

uint8_t sub_sat(uint8_t left, uint8_t right) {
	uint8_t val = left - right;
	return val > left ? 0 : val;
}

uint8_t mul_sat(uint8_t left, uint8_t right) {
	uint16_t value = static_cast<uint16_t>(left) * static_cast<uint16_t>(right);
	return value > 255 ? 255 : static_cast<uint8_t>(value);
}

uint8_t div_sat(uint8_t left, uint8_t right) {
	return left / right;
}