#include "ModularArithmetic.h"

#include <gtest/gtest.h>
#include <math.h>

TEST(ModularArithmetic, mod_remainder) {
	EXPECT_EQ(mod_remainder(0, 25), 0); // zero
	EXPECT_EQ(mod_remainder(1, 25), 1); // positive value
	EXPECT_EQ(mod_remainder(25, 25), 0); // positive wrap-around
	EXPECT_EQ(mod_remainder(27, 25), 2); // positive wrap-around
	EXPECT_EQ(mod_remainder(103, 25), 3); // large positive wrap-around
	EXPECT_EQ(mod_remainder(100, 25), 0); // large positive wrap-around

	EXPECT_EQ(mod_remainder(-1, 25), 24); // negative wrap-around
	EXPECT_EQ(mod_remainder(-25, 25), 0); // negative wrap-around
	EXPECT_EQ(mod_remainder(-27, 25), 23); // negative wrap-around
	EXPECT_EQ(mod_remainder(-100, 25), 0); // large negative wrap-around
	EXPECT_EQ(mod_remainder(-105, 25), 20); // large negative wrap-around

	EXPECT_FLOAT_EQ(mod_remainder(0.f, 25.f), 0.f); // floating-point
	EXPECT_FLOAT_EQ(mod_remainder(3.7f, 25.f), 3.7f); // floating-point
	EXPECT_FLOAT_EQ(mod_remainder(-19.1f, 25.f), 5.9f); // floating-point

	EXPECT_FLOAT_EQ(mod_remainder(229372823.2f, INFINITY), 229372823.2); // infinity
}

TEST(ModularArithmetic, mod_distance) {
	EXPECT_EQ(mod_distance(0, 0, 25), 0); // zero
	EXPECT_EQ(mod_distance(1, 3, 25), 2); // small distance
	EXPECT_EQ(mod_distance(1, 20, 25), 6); // numbers don't wrap, but distance does
	EXPECT_EQ(mod_distance(-1, 4, 25), 5); // numbers wrap on left
	EXPECT_EQ(mod_distance(-1, 26, 25), 2); // numbers wrap on left and right
	EXPECT_EQ(mod_distance(-1, 24, 25), 0); // numbers wrap and distance wraps
	EXPECT_FLOAT_EQ(mod_distance(-1.3f, 5.6f, 25.f), 6.9f); // numbers wrap and distance wraps
}

TEST(ModularArithmetic, mod_point_in_right_open_interval) {
	EXPECT_TRUE(mod_point_in_right_open_interval(0, 0, 3, 25)); // no wrapping
	EXPECT_FALSE(mod_point_in_right_open_interval(3, 0, 3, 25)); // right-open

	EXPECT_TRUE(mod_point_in_right_open_interval(27, 1, 3, 25)); // point wraps
	EXPECT_TRUE(mod_point_in_right_open_interval(1, 24, 4, 25)); // interval wraps
	EXPECT_TRUE(mod_point_in_right_open_interval(-3, 16, 14, 25)); // point and interval wraps
	EXPECT_FALSE(mod_point_in_right_open_interval(32, -5, 6, 25)); // point and interval wraps

	EXPECT_TRUE(mod_point_in_right_open_interval(0.f, 0.f, 3.f, 25.f)); // floating-point
	EXPECT_TRUE(mod_point_in_right_open_interval(-3.f, 16.f, 14.f, 25.f)); // floating-point
}

TEST(ModularArithmetic, mod_intervals_overlap) {
	EXPECT_TRUE(mod_intervals_overlap(0, 5, 4, 3, 25)); // left-right overlap
	EXPECT_TRUE(mod_intervals_overlap(4, 3, 0, 5, 25)); // right-left overlap
	EXPECT_TRUE(mod_intervals_overlap(1, 6, 2, 3, 25)); // subset
	EXPECT_FALSE(mod_intervals_overlap(2, 4, 7, 1, 25)); // no overlap

	EXPECT_TRUE(mod_intervals_overlap(20, 8, 1, 7, 25)); // wrapping overlap
}