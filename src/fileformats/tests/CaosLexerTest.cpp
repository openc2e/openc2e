#include <gtest/gtest.h>

#include "fileformats/caoslexer.h"
#include "fileformats/caostoken.h"

#include <fmt/format.h>
#include <string>
#include <vector>

std::string debug_repr(const std::string &s) {
    std::string out;
    for (auto c : s) {
        if (c == '\n') {
            out += "\\n";
        } else if (c == '\r') {
            out += "\\r";
        } else if (c == '\t') {
            out += "\\t";
        } else if (c == '\\') {
            out += "\\\\";
        } else if (c < 0x20 || c >= 0x7f) {
            out += fmt::format("\\x{:x}", static_cast<uint8_t>(c));
        } else {
            out += c;
        }
    }
    return out;
}

std::string format_token_list(const std::vector<caostoken>& token) {
    std::string out;
    for (const auto& t : token) {
        out += t.typeAsString() + " " +  debug_repr(t.format()) +
                  " line:" + std::to_string(t.lineno) + "\n";
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
