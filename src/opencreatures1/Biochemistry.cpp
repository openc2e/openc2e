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
	// these are all powers of two minus one
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
	// TODO: move shifted rate into the ChemicalState itself, and only serialize as 0-255 ?
	rate >>= 3;

	auto every = EVERY[rate];
	if ((current_biotick & every) == every) {
		return static_cast<uint8_t>(concentration * MULTIPLIERS[rate]);
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