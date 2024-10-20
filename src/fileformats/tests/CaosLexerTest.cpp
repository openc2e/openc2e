#include <gtest/gtest.h>

#include "fileformats/caoslexer.h"
#include "fileformats/caostoken.h"

#include <vector>
#include <string>

TEST(lexcaos, empty_input) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "");
    ASSERT_EQ(tokens.size(), 1);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_EOI);
    EXPECT_EQ(tokens[0].value, "\0");
    EXPECT_EQ(tokens[0].lineno, 1);
}

TEST(lexcaos, single_line_comment) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "* This is a comment\n");
    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_COMMENT);
    EXPECT_EQ(tokens[0].value, "* This is a comment");
    EXPECT_EQ(tokens[0].lineno, 1);
    EXPECT_EQ(tokens[1].type, caostoken::TOK_NEWLINE);
    EXPECT_EQ(tokens[1].value, "\n");
    EXPECT_EQ(tokens[1].lineno, 1);
    EXPECT_EQ(tokens[2].type, caostoken::TOK_EOI);
}

TEST(lexcaos, multi_line_comment_line_numbers) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "(* This is a\nmulti-line\ncomment *)");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_COMMENT);
    EXPECT_EQ(tokens[0].value, "(* This is a\nmulti-line\ncomment *)");
    EXPECT_EQ(tokens[1].type, caostoken::TOK_EOI);
}

TEST(lexcaos, multiple_lines) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "foo\nbar\nbaz\n");
    ASSERT_EQ(tokens.size(), 7);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[0].value, "foo");
    EXPECT_EQ(tokens[0].lineno, 1);
    EXPECT_EQ(tokens[1].type, caostoken::TOK_NEWLINE);
    EXPECT_EQ(tokens[1].value, "\n");
    EXPECT_EQ(tokens[1].lineno, 1);
    EXPECT_EQ(tokens[2].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[2].value, "bar");
    EXPECT_EQ(tokens[2].lineno, 2);
    EXPECT_EQ(tokens[3].type, caostoken::TOK_NEWLINE);
    EXPECT_EQ(tokens[3].value, "\n");
    EXPECT_EQ(tokens[3].lineno, 2);
    EXPECT_EQ(tokens[4].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[4].value, "baz");
    EXPECT_EQ(tokens[4].lineno, 3);
    EXPECT_EQ(tokens[5].type, caostoken::TOK_NEWLINE);
    EXPECT_EQ(tokens[5].value, "\n");
    EXPECT_EQ(tokens[5].lineno, 3);
    EXPECT_EQ(tokens[6].type, caostoken::TOK_EOI);
    EXPECT_EQ(tokens[6].lineno, 4);
}

TEST(lexcaos, numbers_and_whitespace) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "42 3.14 -7 .5 -.25");
    ASSERT_EQ(tokens.size(), 10);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_INT);
    EXPECT_EQ(tokens[0].value, "42");
    EXPECT_EQ(tokens[1].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[1].value, " ");
    EXPECT_EQ(tokens[2].type, caostoken::TOK_FLOAT);
    EXPECT_EQ(tokens[2].value, "3.14");
    EXPECT_EQ(tokens[3].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[3].value, " ");
    EXPECT_EQ(tokens[4].type, caostoken::TOK_INT);
    EXPECT_EQ(tokens[4].value, "-7");
    EXPECT_EQ(tokens[5].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[5].value, " ");
    EXPECT_EQ(tokens[6].type, caostoken::TOK_FLOAT);
    EXPECT_EQ(tokens[6].value, ".5");
    EXPECT_EQ(tokens[7].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[7].value, " ");
    EXPECT_EQ(tokens[8].type, caostoken::TOK_FLOAT);
    EXPECT_EQ(tokens[8].value, "-.25");
    EXPECT_EQ(tokens[9].type, caostoken::TOK_EOI);
}

TEST(lexcaos, binary_string) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "%01011010");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_BINARY);
    EXPECT_EQ(tokens[0].value, "%01011010");
    EXPECT_EQ(tokens[0].lineno, 1);
    EXPECT_EQ(tokens[1].type, caostoken::TOK_EOI);
}

TEST(lexcaos, quoted_string) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "\"Hello, world!\"");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_STRING);
    EXPECT_EQ(tokens[0].value, "\"Hello, world!\"");
    EXPECT_EQ(tokens[0].lineno, 1);
    EXPECT_EQ(tokens[1].type, caostoken::TOK_EOI);
}

TEST(lexcaos, byte_string) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "[48656C6C6F]");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_BYTESTR);
    EXPECT_EQ(tokens[0].value, "[48656C6C6F]");
    EXPECT_EQ(tokens[0].lineno, 1);
    EXPECT_EQ(tokens[1].type, caostoken::TOK_EOI);
}

TEST(lexcaos, comparison_operators_and_whitespace) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "< > <= >= <> =");
    ASSERT_EQ(tokens.size(), 12);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[0].value, "<");
    EXPECT_EQ(tokens[1].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[1].value, " ");
    EXPECT_EQ(tokens[2].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[2].value, ">");
    EXPECT_EQ(tokens[3].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[3].value, " ");
    EXPECT_EQ(tokens[4].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[4].value, "<=");
    EXPECT_EQ(tokens[5].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[5].value, " ");
    EXPECT_EQ(tokens[6].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[6].value, ">=");
    EXPECT_EQ(tokens[7].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[7].value, " ");
    EXPECT_EQ(tokens[8].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[8].value, "<>");
    EXPECT_EQ(tokens[9].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[9].value, " ");
    EXPECT_EQ(tokens[10].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[10].value, "=");
    EXPECT_EQ(tokens[11].type, caostoken::TOK_EOI);
}

TEST(lexcaos, barewords_and_whitespace) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "hello_world var1 CONST_VALUE");
    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[0].value, "hello_world");
    EXPECT_EQ(tokens[1].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[1].value, " ");
    EXPECT_EQ(tokens[2].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[2].value, "var1");
    EXPECT_EQ(tokens[3].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[3].value, " ");
    EXPECT_EQ(tokens[4].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[4].value, "CONST_VALUE");
    EXPECT_EQ(tokens[5].type, caostoken::TOK_EOI);
}

TEST(lexcaos, mixed_whitespace) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "  \t \r");
    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[0].value, "  \t \r");
    EXPECT_EQ(tokens[0].lineno, 1);
    EXPECT_EQ(tokens[1].type, caostoken::TOK_EOI);
}

TEST(lexcaos, character_literals) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "'a' '\\n' 'x'");
    ASSERT_EQ(tokens.size(), 6);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_CHAR);
    EXPECT_EQ(tokens[0].value, "'a'");
    EXPECT_EQ(tokens[1].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[1].value, " ");
    EXPECT_EQ(tokens[2].type, caostoken::TOK_CHAR);
    EXPECT_EQ(tokens[2].value, "'\\n'");
    EXPECT_EQ(tokens[3].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[3].value, " ");
    EXPECT_EQ(tokens[4].type, caostoken::TOK_CHAR);
    EXPECT_EQ(tokens[4].value, "'x'");
    EXPECT_EQ(tokens[5].type, caostoken::TOK_EOI);
}

TEST(lexcaos, commas) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "1, 2, 3");
    ASSERT_EQ(tokens.size(), 8);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_INT);
    EXPECT_EQ(tokens[0].value, "1");
    EXPECT_EQ(tokens[1].type, caostoken::TOK_COMMA);
    EXPECT_EQ(tokens[1].value, ",");
    EXPECT_EQ(tokens[2].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[2].value, " ");
    EXPECT_EQ(tokens[3].type, caostoken::TOK_INT);
    EXPECT_EQ(tokens[3].value, "2");
    EXPECT_EQ(tokens[4].type, caostoken::TOK_COMMA);
    EXPECT_EQ(tokens[4].value, ",");
    EXPECT_EQ(tokens[4].lineno, 1);
    EXPECT_EQ(tokens[5].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[5].value, " ");
    EXPECT_EQ(tokens[6].type, caostoken::TOK_INT);
    EXPECT_EQ(tokens[6].value, "3");
    EXPECT_EQ(tokens[7].type, caostoken::TOK_EOI);
}

TEST(lexcaos, error_token) {
    std::vector<caostoken> tokens;
    lexcaos(tokens, "valid_token @invalid_token");
    ASSERT_EQ(tokens.size(), 5);
    EXPECT_EQ(tokens[0].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[0].value, "valid_token");
    EXPECT_EQ(tokens[1].type, caostoken::TOK_WHITESPACE);
    EXPECT_EQ(tokens[1].value, " ");
    EXPECT_EQ(tokens[2].type, caostoken::TOK_ERROR);
    EXPECT_EQ(tokens[2].value, "@");
    EXPECT_EQ(tokens[3].type, caostoken::TOK_WORD);
    EXPECT_EQ(tokens[3].value, "invalid_token");
    EXPECT_EQ(tokens[4].type, caostoken::TOK_EOI);
}
