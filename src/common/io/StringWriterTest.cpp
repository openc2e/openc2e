#include "common/io/StringWriter.h"

#include <gtest/gtest.h>

TEST(StringWriter, empty) {
	StringWriter v;
	EXPECT_EQ(v.string(), "");
}

TEST(StringWriter, write) {
	StringWriter v;
	v.write("hello");
	v.write(" ");
	v.write("world");
	EXPECT_EQ(v.string(), "hello world");
}