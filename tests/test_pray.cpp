#include "prayfile/PraySourceParser.h"
#include <gtest/gtest.h>
#include <fmt/format.h>

TEST(praysourceparser, string_escapes) {
    auto events = PraySourceParser::parse(R"(
        "en-GB"
        group AGNT "test"
        "Agent Description" "A \"really cool\" agent\nThis is a backslash\\"
    )");
    ASSERT_GT(events.size(), 0);
    for (auto e : events) {
        auto string_tag = mpark::get_if<PraySourceParser::StringTag>(&e);
        if (string_tag) {
            ASSERT_EQ(string_tag->value, "A \"really cool\" agent\nThis is a backslash\\");
        }
    }
}
