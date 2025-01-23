#include "common/io/VectorWriter.h"

#include <gtest/gtest.h>
#include <stdint.h>
#include <string.h>
#include <vector>

static auto vec(const char* s) {
	// note this removes the trailing NUL, which is usually what we want
	return std::vector<uint8_t>{
		reinterpret_cast<const uint8_t*>(s),
		reinterpret_cast<const uint8_t*>(s) + strlen(s)};
}

TEST(VectorWriter, empty) {
	VectorWriter v;
	EXPECT_EQ(v.vector(), vec(""));
}

TEST(VectorWriter, write) {
	VectorWriter v;
	v.write("hello");
	v.write(" ");
	v.write("world");
	EXPECT_EQ(v.vector(), vec("hello world"));
}