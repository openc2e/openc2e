#include "common/ends_with.h"

#include <gtest/gtest.h>

TEST(EndsWith, ends_with_ignore_case) {
	EXPECT_TRUE(ends_with_ignore_case("my agent.catalogue", ".catalogue"));
	EXPECT_TRUE(ends_with_ignore_case("my agent.CATalOgue", ".catalogue"));
	EXPECT_FALSE(ends_with_ignore_case("my agent.cataloguep", ".catalogue"));
	EXPECT_FALSE(ends_with_ignore_case("my agent.s16", ".catalogue"));
	EXPECT_FALSE(ends_with_ignore_case("", ".s16"));
	EXPECT_TRUE(ends_with_ignore_case("my agent.s16", ".s16"));
}