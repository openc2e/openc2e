#include "common/io/SpanReader.h"

#include "common/io/IOException.h"

#include <gtest/gtest.h>
#include <stdexcept>
#include <stdint.h>
#include <string.h>
#include <vector>

static auto vec(const char* s) {
	// note this removes the trailing NUL, which is usually what we want
	return std::vector<uint8_t>{
		reinterpret_cast<const uint8_t*>(s),
		reinterpret_cast<const uint8_t*>(s) + strlen(s)};
}

TEST(SpanReader, empty_tell) {
	SpanReader r;
	EXPECT_EQ(r.tell(), 0);
}

TEST(SpanReader, empty_has_data_left) {
	SpanReader r;
	EXPECT_FALSE(r.has_data_left());
}

TEST(SpanReader, empty_read_throws) {
	SpanReader r;
	uint8_t part[1];
	EXPECT_THROW(r.read(part), IOException);
}

TEST(SpanReader, empty_read_to_end) {
	SpanReader r;
	auto contents = r.read_to_end();
	EXPECT_EQ(contents, vec(""));
}

TEST(SpanReader, empty_seek_absolute_zero_does_not_throw) {
	SpanReader r;
	r.seek_absolute(0);
}

TEST(SpanReader, empty_seek_relative_zero_does_not_throw) {
	SpanReader r;
	r.seek_relative(0);
}

TEST(SpanReader, empty_seek_throws) {
	SpanReader r;
	EXPECT_THROW(r.seek_absolute(1), IOException);
	EXPECT_THROW(r.seek_relative(1), IOException);
	EXPECT_THROW(r.seek_relative(-1), IOException);
}

TEST(SpanReader, seek_absolute_to_end_does_not_throw) {
	auto buf = vec("x");
	SpanReader r(buf);
	r.seek_absolute(1);
}

TEST(SpanReader, seek_relative_to_end_does_not_throw) {
	auto buf = vec("x");
	SpanReader r(buf);
	r.seek_relative(1);
}

TEST(SpanReader, seek_relative_to_start_does_not_throw) {
	auto buf = vec("x");
	SpanReader r(buf);
	std::vector<uint8_t> contents(1);
	r.read(contents);
	r.seek_relative(-1);
}

TEST(SpanReader, read_parts) {
	auto buf = vec("hello world");
	SpanReader r(buf);
	std::vector<uint8_t> part1(5);
	r.read(part1);
	EXPECT_EQ(part1, vec("hello"));
	std::vector<uint8_t> part2(6);
	r.read(part2);
	EXPECT_EQ(part2, vec(" world"));
	EXPECT_FALSE(r.has_data_left());
}

TEST(SpanReader, read_to_end) {
	auto buf = vec("hello world");
	SpanReader r(buf);
	EXPECT_EQ(r.read_to_end(), vec("hello world"));
	EXPECT_FALSE(r.has_data_left());
}

TEST(SpanReader, read_to_end_after_read_part) {
	auto buf = vec("hello world");
	SpanReader r(buf);
	std::vector<uint8_t> part(5);
	r.read(part);
	EXPECT_EQ(part, vec("hello"));
	EXPECT_EQ(r.read_to_end(), vec(" world"));
	EXPECT_FALSE(r.has_data_left());
}