#pragma once

#include <stdint.h>

struct ChemicalData {
	uint8_t concentration = 0;
	uint8_t decay = 0;
};

uint8_t decay_chemical(uint8_t concentration, uint8_t rate, uint32_t current_biotick);