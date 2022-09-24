

#include "common/wildcard_match.h"

#include <gtest/gtest.h>


TEST(Wildcard, wildcard_match) {
	EXPECT_FALSE(wildcard_match("hello", "nope"));
	EXPECT_TRUE(wildcard_match("hello", "hello"));
	EXPECT_TRUE(wildcard_match("hel?o", "hello"));
	EXPECT_TRUE(wildcard_match("h*o", "hello"));
	EXPECT_TRUE(wildcard_match("h*l*o", "hello"));
}