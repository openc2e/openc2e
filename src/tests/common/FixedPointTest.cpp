#include "common/FixedPoint.h"

#include <gtest/gtest.h>

TEST(FixedPoint, fixed24_8_t) {
	constexpr float eps = 1.0 / (1 << 8);

	fixed24_8_t f;
	EXPECT_EQ(f, 0.0);
	EXPECT_EQ(f.trunc(), 0);

	f = 5;
	EXPECT_EQ(f, 5.0);
	EXPECT_EQ(f.trunc(), 5);

	f = 5.1;
	EXPECT_NEAR(static_cast<float>(f), 5.1, eps);
	EXPECT_EQ(f.trunc(), 5);

	f = -6.2;
	EXPECT_NEAR(static_cast<float>(f), -6.2, eps);
	EXPECT_EQ(f.trunc(), -6);

	f = 0;
	f += 1;
	EXPECT_TRUE(f == 1);
	f += 2.3;
	EXPECT_NEAR(static_cast<float>(f), 3.3, eps);
}