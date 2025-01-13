#include "fileformats/caoslexer.h"

#include "fileformats/caostoken.h"

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

static std::string format_token_list(const std::vector<caostoken>& token) {
	std::string out;
	for (const auto& t : token) {
		out += fmt::format("{} {:?} line:{}\n", t.typeAsString(), t.data, t.lineno);
	}
	return out;
}

static void assert_lexcaos(const std::string& input, const std::vector<caostoken>& expected) {
	std::vector<caostoken> tokens;
	lexcaos(tokens, input);

	ASSERT_EQ(format_token_list(tokens), format_token_list(expected));
}

TEST(lexcaos, unterminated_single_quote) {
	assert_lexcaos("'",
		{
			{caostoken::TOK_ERROR, "'", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, unterminated_single_quote_letter) {
	assert_lexcaos("'a",
		{
			{caostoken::TOK_ERROR, "'a", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, unterminated_double_quote) {
	assert_lexcaos("\"",
		{
			{caostoken::TOK_ERROR, "\"", 1},
			{caostoken::TOK_EOI, 1},
		});
}
