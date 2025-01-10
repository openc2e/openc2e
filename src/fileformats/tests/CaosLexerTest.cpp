#include "fileformats/caoslexer.h"

#include "fileformats/caostoken.h"

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>

std::string format_token_list(const std::vector<caostoken>& token) {
	std::string out;
	for (const auto& t : token) {
		out += fmt::format("{} {:?} line:{}\n", t.typeAsString(), t.data, t.lineno);
	}
	return out;
}

TEST(lexcaos, unterminated_single_quote) {
	std::vector<caostoken> tokens;
	lexcaos(tokens, "'");

	std::vector<caostoken> expected{
		{caostoken::TOK_ERROR, "'", 1},
		{caostoken::TOK_EOI, 1},
	};

	ASSERT_EQ(format_token_list(tokens), format_token_list(expected));
}

TEST(lexcaos, unterminated_single_quote_letter) {
	std::vector<caostoken> tokens;
	lexcaos(tokens, "'a");

	std::vector<caostoken> expected{
		{caostoken::TOK_ERROR, "'a", 1},
		{caostoken::TOK_EOI, 1},
	};

	ASSERT_EQ(format_token_list(tokens), format_token_list(expected));
}

TEST(lexcaos, unterminated_double_quote) {
	std::vector<caostoken> tokens;
	lexcaos(tokens, "\"");

	std::vector<caostoken> expected{
		{caostoken::TOK_ERROR, "\"", 1},
		{caostoken::TOK_EOI, 1},
	};

	ASSERT_EQ(format_token_list(tokens), format_token_list(expected));
}

TEST(lexcaos, unterminated_byte_string) {
	std::vector<caostoken> tokens;
	lexcaos(tokens, "[");

	std::vector<caostoken> expected{
		{caostoken::TOK_ERROR, "[", 1},
		{caostoken::TOK_EOI, "\0", 1},
	};

	ASSERT_EQ(format_token_list(tokens), format_token_list(expected));
}

TEST(lexcaos, byte_string_single_character) {
	std::vector<caostoken> tokens;
	lexcaos(tokens, "[0]");

	std::vector<caostoken> expected{
		{caostoken::TOK_BYTESTR, "[0]", 1},
		{caostoken::TOK_EOI, "\0", 1},
	};

	ASSERT_EQ(format_token_list(tokens), format_token_list(expected));
}

TEST(lexcaos, byte_string_multiple_characters) {
	std::vector<caostoken> tokens;
	lexcaos(tokens, "[0123]");

	std::vector<caostoken> expected{
		{caostoken::TOK_BYTESTR, "[0123]", 1},
		{caostoken::TOK_EOI, "\0", 1},
	};

	ASSERT_EQ(format_token_list(tokens), format_token_list(expected));
}

TEST(lexcaos, byte_string_with_newline) {
	std::vector<caostoken> tokens;
	lexcaos(tokens, "[01\n02]");

	std::vector<caostoken> expected{
		{caostoken::TOK_ERROR, "[01\n", 1},
		{caostoken::TOK_INT, "02", 2},
		{caostoken::TOK_ERROR, "]", 2},
		{caostoken::TOK_EOI, "\0", 2},
	};

	ASSERT_EQ(format_token_list(tokens), format_token_list(expected));
}

TEST(lexcaos, byte_string_with_carriage_return) {
	std::vector<caostoken> tokens;
	lexcaos(tokens, "[01\r02]");

	std::vector<caostoken> expected{
		{caostoken::TOK_ERROR, "[01\r", 1},
		{caostoken::TOK_INT, "02", 2},
		{caostoken::TOK_ERROR, "]", 2},
		{caostoken::TOK_EOI, "\0", 2},
	};

	ASSERT_EQ(format_token_list(tokens), format_token_list(expected));
}
