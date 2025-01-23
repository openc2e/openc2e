#include "common/io/Getline.h"

#include "common/io/SpanReader.h"

#include <gtest/gtest.h>


TEST(Getline, getline) {
	const char* tests[] = {
		"  hi\ncool\n",
		"  hi\r\ncool\r\n",
		"     ",
		"\n",
		"\r\n",
		"",
		"   \n  ",
		"   \r\n  ",
		"x",
	};
	for (auto c : tests) {
		SpanReader reader(c);
		std::stringstream stream(c);

		std::string stream_line;
		std::getline(stream, stream_line);
		if (!stream_line.empty() && stream_line.back() == '\r') {
			// our version of getline absorbs both \r\n and \n, regardless
			// of what platform its running on, unlike std::getline
			stream_line.pop_back();
		}
		EXPECT_EQ(getline(reader), stream_line);

		auto stream_remaining = std::vector<uint8_t>(std::istreambuf_iterator<char>(stream), {});
		EXPECT_EQ(reader.read_to_end(), stream_remaining);
	}
}
