#include "common/io/Scanf.h"

#include "common/io/SpanReader.h"

#include <gtest/gtest.h>


TEST(Scanf, scan_int) {
	const char* tests[] = {
		"",
		"     ",
		"\n",
		"   \n  ",
		"x",
		"  x",
		"+",
		"-",
		"  +",
		"  +1.4",
		"  +1.4jsjksks",
		"  +1.4jsjksks ",
		"  -2,356.1",
		"345 232 1 ",
	};
	for (auto c : tests) {
		SpanReader reader(c);
		std::stringstream stream(c);
		int stream_value = 0;
		stream >> stream_value;
		EXPECT_EQ(scan_int(reader), stream_value);

		auto stream_remaining = std::vector<uint8_t>(std::istreambuf_iterator<char>(stream), {});
		EXPECT_EQ(reader.read_to_end(), stream_remaining);
	}
}

TEST(Scanf, scan_float) {
	const char* tests[] = {
		"",
		"     ",
		"\n",
		"   \n  ",
		"x",
		"  x",
		"+",
		"-",
		"  +",
		"  +1.4",
		"  +1.4jsjksks",
		"  +1.4jsjksks ",
		"  -2,356.1",
	};
	for (auto c : tests) {
		SpanReader reader(c);
		std::stringstream stream(c);
		float stream_value = 0;
		stream >> stream_value;
		EXPECT_EQ(scan_float(reader), stream_value);

		auto stream_remaining = std::vector<uint8_t>(std::istreambuf_iterator<char>(stream), {});
		EXPECT_EQ(reader.read_to_end(), stream_remaining);
	}
}