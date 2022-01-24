#include "common/encoding.h"

#include <gtest/gtest.h>

TEST(encoding, is_valid_utf8) {
	// surrogate half
	EXPECT_FALSE(is_valid_utf8("\xed\xa0\x80"));
	// overlong
	EXPECT_FALSE(is_valid_utf8("\xc0\xa0"));
}

TEST(encoding, ensure_utf8) {
	EXPECT_EQ(ensure_utf8("hello world"), "hello world");
	EXPECT_EQ(ensure_utf8("tr\xc3\xa8s cool"), "tr\xc3\xa8s cool");
	EXPECT_EQ(ensure_utf8("\xef\xbb\xbftr\xc3\xa8s cool"), "tr\xc3\xa8s cool");
	EXPECT_EQ(ensure_utf8("tr\xe8s cool"), "tr\xc3\xa8s cool");
}

TEST(encoding, cp1252_to_utf8) {
	EXPECT_EQ(cp1252_to_utf8("tr\xe8s cool"), "tr\xc3\xa8s cool");
	EXPECT_EQ(cp1252_to_utf8("\x80"), "\xe2\x82\xac");
}

TEST(encoding, utf8_to_cp1252) {
	EXPECT_EQ(utf8_to_cp1252("tr\xc3\xa8s cool"), "tr\xe8s cool");
}
TEST(encoding, utf8_to_cp1252_combining_diacriticals) {
	EXPECT_EQ(utf8_to_cp1252("tre\xcc\x80s cool"), "tr\xe8s cool");
}

TEST(encoding, is_valid_ascii) {
	EXPECT_TRUE(is_valid_ascii((uint8_t*)"hello world", 12));
	EXPECT_FALSE(is_valid_ascii((uint8_t*)"tr\xe8s cool", 10));
	EXPECT_FALSE(is_valid_ascii((uint8_t*)"tr\xc3\xa8s cool", 11));
}