#include "fileformats/caoslexer.h"

#include "fileformats/caostoken.h"

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

std::string format_token_list(const std::vector<caostoken>& token) {
	std::string out;
	for (const auto& t : token) {
		out += fmt::format("{} {:?} line:{}\n", t.typeAsString(), t.format(), t.lineno);
	}
	return out;
}

TEST(lexcaos, unterminated_double_quote) {
	std::vector<caostoken> tokens;
	lexcaos(tokens, "\"");

	std::vector<caostoken> expected{
		{caostoken::TOK_ERROR, 1},
		{caostoken::TOK_EOI, "\0", 1},
	};

	ASSERT_EQ(format_token_list(tokens), format_token_list(expected));
}