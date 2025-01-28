#include "Biochemistry.h"

#include "common/math/Saturating.h"

const uint32_t EVERY[32] = {
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	// these are all powers of two minus one, e.g. Mersenne numbers.
	// likely because a modulus operation on a power of two can be represented
	// as a much cheaper bitwise "AND" operation with the power of two minus one.
	1,
	3,
	7,
	15,
	31,
	63,
	127,
	255,
	511,
	1023,
	2047,
	4095,
	8191,
	16383,
	32767,
	65535,
	131071,
	262143,
	524287,
	1048575,
	2097151,
	4194303,
	8388607,
	16777215};

const float MULTIPLIERS[32] = {
	0.0000000000000000,
	0.1978302001953125,
	0.4447784423828125,
	0.6669158935546875,
	0.8166503906250000,
	0.9036865234375000,
	0.9506225585937500,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9749908447265625,
	0.9999847412109375};

uint8_t decay_chemical(uint8_t concentration, uint8_t rate, uint32_t current_biotick) {
	// TODO: this might be faster to keep in the header so the compiler can inline it
	return decay_chemical_n_ticks(concentration, rate, current_biotick, 1);
}

uint8_t decay_chemical_n_ticks(uint8_t concentration, uint8_t rate, uint32_t start_biotick, uint32_t n) {
	// this function basically exists just for testing. otherwise, testing the super large decay rates
	// can take a while.

	// TODO: move shifted rate into the ChemicalState itself, and only serialize as 0-255 ?
	rate >>= 3;
	const auto every = EVERY[rate];
	const auto mul = MULTIPLIERS[rate];
	uint32_t biotick = start_biotick;

	while (biotick < start_biotick + n) {
		if ((biotick & every) == every) {
			concentration = static_cast<uint8_t>(concentration * mul);
			biotick++;
		} else {
			if (n == 1) {
				// special-case for n=1, a failure to match means we're done for this tick.
				// otherwise the compiler won't optimize this loop out.
				break;
			}
			// advance to next biotick that matches the bitmask
			// while ((biotick & every) != every) biotick++;
			// wait! that's really slow for large masks. we can do this more quickly
			// by exploiting a property of our specific bitmasks, which are all Mersenne
			// numbers, e.g. they only have trailing 1s in binary. the next number up that
			// matches the mask is simply: current_number | mask.
			biotick = biotick | every;
		}
	}
	return concentration;
}

void run_chemical_reaction(ChemicalStates& chemicals, const ChemicalReaction& rxn, uint32_t current_biotick) {
	uint8_t availability = 255;
	if (rxn.r1_chem != 0 && rxn.r1_prop != 0) {
		availability = chemicals[rxn.r1_chem].concentration / rxn.r1_prop;
	}
	if (rxn.r2_chem != 0 && rxn.r2_prop != 0) {
		uint8_t availability2 = chemicals[rxn.r2_chem].concentration / rxn.r2_prop;
		availability = availability2 < availability ? availability2 : availability;
	}

	if (availability == 0) {
		return;
	}

	uint8_t diff = availability - decay_chemical(availability, rxn.rate, current_biotick);
	if (diff == 0) {
		return;
	}

	chemicals[rxn.r1_chem].concentration = sub_sat(chemicals[rxn.r1_chem].concentration, mul_sat(diff, rxn.r1_prop));
	chemicals[rxn.r2_chem].concentration = sub_sat(chemicals[rxn.r2_chem].concentration, mul_sat(diff, rxn.r2_prop));
	chemicals[rxn.p1_chem].concentration = add_sat(chemicals[rxn.p1_chem].concentration, mul_sat(diff, rxn.p1_prop));
	chemicals[rxn.p2_chem].concentration = add_sat(chemicals[rxn.p2_chem].concentration, mul_sat(diff, rxn.p2_prop));
}