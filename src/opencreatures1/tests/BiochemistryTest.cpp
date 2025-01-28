#include "opencreatures1/Biochemistry.h"

#include <gtest/gtest.h>

constexpr struct DecayTest {
	uint8_t rate;
	uint32_t half_life_in_bioticks;
	uint32_t zero_life_in_bioticks;
	uint8_t value_after_half_life;
} decay_tests[] = {
	{0, 1, 1, 0},
	{8, 1, 4, 50},
	{16, 1, 7, 113},
	{24, 2, 13, 113},
	{32, 4, 22, 112},
	{40, 7, 38, 122},
	{48, 13, 62, 127},
	{56, 25, 99, 126},
	{64, 50, 198, 126},
	{72, 100, 396, 126},
	{80, 200, 792, 126},
	{88, 400, 1584, 126},
	{96, 800, 3168, 126},
	{104, 1600, 6336, 126},
	{112, 3200, 12672, 126},
	{120, 6400, 25344, 126},
	{128, 12800, 50688, 126},
	{136, 25600, 101376, 126},
	{144, 51200, 202752, 126},
	{152, 102400, 405504, 126},
	{160, 204800, 811008, 126},
	{168, 409600, 1622016, 126},
	{176, 819200, 3244032, 126},
	{184, 1638400, 6488064, 126},
	{192, 3276800, 12976128, 126},
	{200, 6553600, 25952256, 126},
	{208, 13107200, 51904512, 126},
	{216, 26214400, 103809024, 126},
	{224, 52428800, 207618048, 126},
	{232, 104857600, 415236096, 126},
	{240, 209715200, 830472192, 126},
	{248, 2147483648, 4278190080, 127},
};


TEST(Biochemistry, decay_half_lives_and_zero_lives) {
	for (auto t : decay_tests) {
		uint8_t concentration = 255;
		uint8_t previous_concentration = concentration;
		uint32_t biotick = 0;

		previous_concentration = concentration;
		concentration = decay_chemical_n_ticks(concentration, t.rate, biotick, t.half_life_in_bioticks);
		biotick += t.half_life_in_bioticks;
		EXPECT_LE((int)concentration, 127);
		EXPECT_EQ((int)concentration, (int)t.value_after_half_life);

		if (biotick < t.zero_life_in_bioticks) {
			concentration = decay_chemical_n_ticks(concentration, t.rate, biotick, t.zero_life_in_bioticks - 1 - biotick);
			previous_concentration = concentration;
			biotick += t.zero_life_in_bioticks - 1 - biotick;
			concentration = decay_chemical_n_ticks(concentration, t.rate, biotick, 1);
			biotick += 1;
		}
		EXPECT_GT((int)previous_concentration, 0);
		EXPECT_EQ((int)concentration, 0);
	}
}

TEST(Biochemistry, decay_curve_16) {
	// test a simple decay curve that decays every biotick
	const uint8_t rate = 16;
	const uint8_t concentrations[] = {
		113, 50, 22, 9, 4, 1, 0};

	uint8_t concentration = 255;
	for (uint32_t biotick = 0; biotick < sizeof(concentrations); biotick++) {
		concentration = decay_chemical(concentration, rate, biotick);
		EXPECT_EQ((int)concentration, (int)concentrations[biotick]);
	}
}

TEST(Biochemistry, decay_curve_72) {
	// test a more complex decay curve that only decays on certain bioticks
	const uint8_t rate = 72;
	const uint8_t concentrations[] = {
		255, 255, 255, 248, 248, 248, 248, 241, 241, 241, 241, 234, 234, 234, 234, 228, 228, 228, 228,
		222, 222, 222, 222, 216, 216, 216, 216, 210, 210, 210, 210, 204, 204, 204, 204, 198, 198, 198,
		198, 193, 193, 193, 193, 188, 188, 188, 188, 183, 183, 183, 183, 178, 178, 178, 178, 173, 173,
		173, 173, 168, 168, 168, 168, 163, 163, 163, 163, 158, 158, 158, 158, 154, 154, 154, 154, 150,
		150, 150, 150, 146, 146, 146, 146, 142, 142, 142, 142, 138, 138, 138, 138, 134, 134, 134, 134,
		130, 130, 130, 130, 126, 126, 126, 126, 122, 122, 122, 122, 118, 118, 118, 118, 115, 115, 115,
		115, 112, 112, 112, 112, 109, 109, 109, 109, 106, 106, 106, 106, 103, 103, 103, 103, 100, 100,
		100, 100, 97, 97, 97, 97, 94, 94, 94, 94, 91, 91, 91, 91, 88, 88, 88, 88, 85, 85, 85, 85, 82,
		82, 82, 82, 79, 79, 79, 79, 77, 77, 77, 77, 75, 75, 75, 75, 73, 73, 73, 73, 71, 71, 71, 71, 69,
		69, 69, 69, 67, 67, 67, 67, 65, 65, 65, 65, 63, 63, 63, 63, 61, 61, 61, 61, 59, 59, 59, 59, 57,
		57, 57, 57, 55, 55, 55, 55, 53, 53, 53, 53, 51, 51, 51, 51, 49, 49, 49, 49, 47, 47, 47, 47, 45,
		45, 45, 45, 43, 43, 43, 43, 41, 41, 41, 41, 39, 39, 39, 39, 38, 38, 38, 38, 37, 37, 37, 37, 36,
		36, 36, 36, 35, 35, 35, 35, 34, 34, 34, 34, 33, 33, 33, 33, 32, 32, 32, 32, 31, 31, 31, 31, 30,
		30, 30, 30, 29, 29, 29, 29, 28, 28, 28, 28, 27, 27, 27, 27, 26, 26, 26, 26, 25, 25, 25, 25, 24,
		24, 24, 24, 23, 23, 23, 23, 22, 22, 22, 22, 21, 21, 21, 21, 20, 20, 20, 20, 19, 19, 19, 19, 18,
		18, 18, 18, 17, 17, 17, 17, 16, 16, 16, 16, 15, 15, 15, 15, 14, 14, 14, 14, 13, 13, 13, 13, 12,
		12, 12, 12, 11, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5,
		5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0};

	uint8_t concentration = 255;
	for (uint32_t biotick = 0; biotick < sizeof(concentrations); biotick++) {
		concentration = decay_chemical(concentration, rate, biotick);
		EXPECT_EQ((int)concentration, (int)concentrations[biotick]);
	}
}

TEST(Biochemistry, reaction) {
	// 1 Glucose58 + 2 Hexokinase61 =24=> 4 CarbonDioxide62 + 8 Activase91
	ChemicalReaction rxn{1, 58, 2, 61, 24, 4, 62, 8, 91};

	// availability - min(153, 74/2) = 37
	// decay rate 24 - every tick, multiply by 0.6669158935546875
	// diff - 13
	// r1 = 153 - 13 * 1 = 140
	// r2 = 74 - 13 * 2 = 48
	// p1 = 0 + 13 * 4 = 52
	// p2 = 0 + 13 * 8 = 104

	ChemicalStates chemicals;
	chemicals[58].concentration = 153;
	chemicals[58].decay = 248;
	chemicals[61].concentration = 74;
	chemicals[61].decay = 248;

	uint32_t biotick = 0;
	run_chemical_reaction(chemicals, rxn, biotick);

	EXPECT_EQ(chemicals[58].concentration, 140);
	EXPECT_EQ(chemicals[61].concentration, 48);
	EXPECT_EQ(chemicals[62].concentration, 52);
	EXPECT_EQ(chemicals[91].concentration, 104);

	// More example reactions we could test
	// {'r1_amount': 1, 'r1_chem': 58, 'r2_amount': 2, 'r2_chem': 61, 'rate': 24, 'p1_amount': 4, 'p1_chem': 62, 'p2_amount': 8, 'p2_chem': 91}
	// {'r1_amount': 1, 'r1_chem': 17, 'r2_amount': 1, 'r2_chem': 0, 'rate': 8, 'p1_amount': 1, 'p1_chem': 1, 'p2_amount': 1, 'p2_chem': 50}
	// {'r1_amount': 1, 'r1_chem': 102, 'r2_amount': 1, 'r2_chem': 0, 'rate': 32, 'p1_amount': 2, 'p1_chem': 33, 'p2_amount': 1, 'p2_chem': 23}
}
