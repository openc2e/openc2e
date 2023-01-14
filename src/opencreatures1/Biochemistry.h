#pragma once

#include <array>
#include <stdint.h>

struct ChemicalState {
	uint8_t concentration = 0;
	uint8_t decay = 0;
};

using ChemicalStates = std::array<ChemicalState, 256>;

struct ChemicalReaction {
	uint8_t r1_prop = 0;
	uint8_t r1_chem = 0;
	uint8_t r2_prop = 0;
	uint8_t r2_chem = 0;
	uint8_t rate = 0;
	uint8_t p1_prop = 0;
	uint8_t p1_chem = 0;
	uint8_t p2_prop = 0;
	uint8_t p2_chem = 0;
};

uint8_t decay_chemical(uint8_t concentration, uint8_t rate, uint32_t current_biotick);
void run_chemical_reaction(ChemicalStates& chemicals, const ChemicalReaction& rxn, uint32_t current_biotick);