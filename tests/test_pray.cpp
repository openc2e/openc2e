#include "prayfile/Caos2PrayParser.h"
#include "prayfile/PraySourceParser.h"
#include <gtest/gtest.h>
#include <fmt/format.h>

static std::string eventsToString(const std::vector<PraySourceParser::Event>& events) {
    std::string all_events;
    for (auto e : events) {
        if (all_events.size() > 0) { all_events += "\n"; }
        all_events += PraySourceParser::eventToString(e);
    }
    return all_events;
}

template <typename T, typename F>
T* matchEvent(const std::vector<PraySourceParser::Event>& events, F f) {
    for (auto e : events) {
        auto matched = mpark::get_if<T>(&e);
        if (matched && f(*matched)) {
            return matched;
        }
    }
    return nullptr;
}

template <typename T>
T* matchEvent(const std::vector<PraySourceParser::Event>& events) {
    return matchEvent<T>(events, [](T&) { return true; });
}

TEST(praysourceparser, character_escapes) {
    auto events = PraySourceParser::parse(R"(
        "en-GB"
        group AGNT "My Agent"
        "Agent Description" "A \"really cool\" agent\nThis is a backslash\\"
    )");
    auto string_tag = matchEvent<PraySourceParser::StringTag>(events);
    if (!string_tag) FAIL() << "No such event in:\n" + eventsToString(events);
    ASSERT_EQ(string_tag->value, "A \"really cool\" agent\nThis is a backslash\\");
}


TEST(caos2prayparser, character_escapes) {
    auto events = Caos2PrayParser::parse(R"(
        *# DS-Name "My Agent"
        *# Agent Description = "A \"really cool\" agent\nThis is a backslash\\"
    )", nullptr);
    auto string_tag = matchEvent<PraySourceParser::StringTag>(events, [](PraySourceParser::StringTag &e) {
        return e.key == "Agent Description";
    });
    if (!string_tag) FAIL() << "No such event in:\n" + eventsToString(events);
    ASSERT_EQ(string_tag->value, "A \"really cool\" agent\nThis is a backslash\\");
}