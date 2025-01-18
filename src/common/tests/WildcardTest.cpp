

#include "common/wildcard_match.h"

#include <gtest/gtest.h>


TEST(Wildcard, wildcard_match) {
	EXPECT_FALSE(wildcard_match_ignore_case("hello", "nope"));
	EXPECT_TRUE(wildcard_match_ignore_case("hello", "hello"));
	EXPECT_TRUE(wildcard_match_ignore_case("hel?o", "hello"));
	EXPECT_TRUE(wildcard_match_ignore_case("h*o", "hello"));
	EXPECT_TRUE(wildcard_match_ignore_case("h*l*o", "hello"));
	EXPECT_TRUE(wildcard_match_ignore_case("*.agent", "test.agent"));
	EXPECT_FALSE(wildcard_match_ignore_case("*.agent", "test.agents"));
}

TEST(Wildcard, wildcard_match_case_insensitive) {
	EXPECT_TRUE(wildcard_match_ignore_case("hello", "HELLO"));
	EXPECT_TRUE(wildcard_match_ignore_case("h*o", "HELLO"));
}
