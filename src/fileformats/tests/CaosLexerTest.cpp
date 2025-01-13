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

TEST(lexcaos, byte_string_single_character) {
	assert_lexcaos("[0]",
		{
			{caostoken::TOK_BYTESTR, "[0]", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, byte_string_multiple_characters) {
	assert_lexcaos("[0123]",
		{
			{caostoken::TOK_BYTESTR, "[0123]", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, byte_string_with_newline) {
	assert_lexcaos("[01\n02]",
		{
			{caostoken::TOK_ERROR, "[01", 1},
			{caostoken::TOK_NEWLINE, "\n", 1},
			{caostoken::TOK_INT, "02", 2},
			{caostoken::TOK_ERROR, "]", 2},
			{caostoken::TOK_EOI, 2},
		});
}

TEST(lexcaos, byte_string_with_carriage_return) {
	assert_lexcaos("[01\r02]",
		{
			{caostoken::TOK_ERROR, "[01", 1},
			{caostoken::TOK_WHITESPACE, "\r", 1},
			{caostoken::TOK_INT, "02", 1},
			{caostoken::TOK_ERROR, "]", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, unterminated_byte_string) {
	assert_lexcaos("[",
		{
			{caostoken::TOK_ERROR, "[", 1},
			{caostoken::TOK_EOI, "", 1},
		});
}

TEST(lexcaos, unterminated_double_quote) {
	assert_lexcaos("\"",
		{
			{caostoken::TOK_ERROR, "\"", 1},
			{caostoken::TOK_EOI, 1},
		});
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


TEST(lexcaos, empty_input) {
	assert_lexcaos("",
		{
			{caostoken::TOK_EOI, "\0", 1},
		});
}

TEST(lexcaos, single_line_comment) {
	assert_lexcaos("* This is a comment\n",
		{
			{caostoken::TOK_COMMENT, "* This is a comment", 1},
			{caostoken::TOK_NEWLINE, "\n", 1},
			{caostoken::TOK_EOI, 2},
		});
}

TEST(lexcaos, multiple_lines) {
	assert_lexcaos("foo\nbar\nbaz\n",
		{
			{caostoken::TOK_WORD, "foo", 1},
			{caostoken::TOK_NEWLINE, "\n", 1},
			{caostoken::TOK_WORD, "bar", 2},
			{caostoken::TOK_NEWLINE, "\n", 2},
			{caostoken::TOK_WORD, "baz", 3},
			{caostoken::TOK_NEWLINE, "\n", 3},
			{caostoken::TOK_EOI, 4},
		});
}

TEST(lexcaos, numbers_and_whitespace) {
	assert_lexcaos("42 3.14 -7 .5 -.25",
		{
			{caostoken::TOK_INT, "42", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_FLOAT, "3.14", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_INT, "-7", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_FLOAT, ".5", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_FLOAT, "-.25", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, binary_string) {
	assert_lexcaos("%01011010",
		{
			{caostoken::TOK_BINARY, "%01011010", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, quoted_string) {
	assert_lexcaos("\"Hello, world!\"",
		{
			{caostoken::TOK_STRING, "\"Hello, world!\"", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, byte_string) {
	assert_lexcaos("[48656C6C6F]",
		{
			{caostoken::TOK_BYTESTR, "[48656C6C6F]", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, comparison_operators_and_whitespace) {
	assert_lexcaos("< > <= >= <> =",
		{
			{caostoken::TOK_WORD, "<", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_WORD, ">", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_WORD, "<=", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_WORD, ">=", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_WORD, "<>", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_WORD, "=", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, barewords_and_whitespace) {
	assert_lexcaos("hello_world var1 CONST_VALUE",
		{
			{caostoken::TOK_WORD, "hello_world", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_WORD, "var1", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_WORD, "CONST_VALUE", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, mixed_whitespace) {
	assert_lexcaos("  \t \r",
		{
			{caostoken::TOK_WHITESPACE, "  \t \r", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, character_literals) {
	assert_lexcaos("'a' '\\n' 'x'",
		{
			{caostoken::TOK_CHAR, "'a'", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_CHAR, "'\\n'", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_CHAR, "'x'", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, commas) {
	assert_lexcaos("1, 2, 3",
		{
			{caostoken::TOK_INT, "1", 1},
			{caostoken::TOK_COMMA, ",", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_INT, "2", 1},
			{caostoken::TOK_COMMA, ",", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_INT, "3", 1},
			{caostoken::TOK_EOI, 1},
		});
}

TEST(lexcaos, error_token) {
	assert_lexcaos("valid_token @invalid_token",
		{
			{caostoken::TOK_WORD, "valid_token", 1},
			{caostoken::TOK_WHITESPACE, " ", 1},
			{caostoken::TOK_ERROR, "@", 1},
			{caostoken::TOK_WORD, "invalid_token", 1},
			{caostoken::TOK_EOI, 1},
		});
}