#include "prayfile/Caos2PrayParser.h"
#include "prayfile/PrayFileReader.h"
#include "prayfile/PrayFileWriter.h"
#include "prayfile/PraySourceParser.h"
#include "spanstream.h"
#include "vectorstream.h"
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
const T* matchEvent(const std::vector<PraySourceParser::Event>& events, F f) {
    for (auto& e : events) {
        auto matched = mpark::get_if<T>(&e);
        if (matched && f(*matched)) {
            return matched;
        }
    }
    return nullptr;
}

template <typename T>
const T* matchEvent(const std::vector<PraySourceParser::Event>& events) {
    return matchEvent<T>(events, [](const T&) { return true; });
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
    auto string_tag = matchEvent<PraySourceParser::StringTag>(events, [](const PraySourceParser::StringTag &e) {
        return e.key == "Agent Description";
    });
    if (!string_tag) FAIL() << "No such event in:\n" + eventsToString(events);
    ASSERT_EQ(string_tag->value, "A \"really cool\" agent\nThis is a backslash\\");
}

TEST(praywriter, doesnt_compress_if_would_be_bigger) {
    vectorstream v;
    PrayFileWriter writer(v);
    std::map<std::string, std::string> string_tags{{"Description", "Mon agent est tr\xc3\xa8s cool"}};
    std::map<std::string, int> int_tags;
    writer.writeBlockTags("AGNT", "Agent tr\xc3\xa8s cool", int_tags, string_tags, PrayFileWriter::PRAY_COMPRESS_ON);

    ASSERT_EQ(v.vector().size(), 198);
}

TEST(praysourceparser, bom) {
    std::string good = "\"en-GB\"";
    {
        auto events = PraySourceParser::parse(good);
        EXPECT_TRUE(!matchEvent<PraySourceParser::Error>(events));
    }
    {
        auto events = PraySourceParser::parse("\xef\xbb\xbf" + good);
        EXPECT_TRUE(!matchEvent<PraySourceParser::Error>(events));
    }
    {
        auto events = PraySourceParser::parse("\xfe\xff" + good);
        EXPECT_TRUE(matchEvent<PraySourceParser::Error>(events));
    }
    {
        auto events = PraySourceParser::parse("\xff\xfe" + good);
        EXPECT_TRUE(matchEvent<PraySourceParser::Error>(events));
    }
}

TEST(praysourceparser, windows1252_to_utf8) {
    auto events = PraySourceParser::parse(std::string(R"(
        "en-GB"
        group AGNT "My Agent"
        "Agent Description-fr" )") + "\"Mon agent est tr\xe8s cool\""
    );
    auto string_tag = matchEvent<PraySourceParser::StringTag>(events);
    if (!string_tag) FAIL() << "No such event in:\n" + eventsToString(events);
    ASSERT_EQ(string_tag->value, "Mon agent est tr\xc3\xa8s cool");
}

TEST(praysourceparser, utf8_to_utf8) {
    auto events = PraySourceParser::parse(std::string(R"(
        "en-GB"
        group AGNT "My Agent"
        "Agent Description-fr" )") + "\"Mon agent est tr\xc3\xa8s cool\""
    );
    auto string_tag = matchEvent<PraySourceParser::StringTag>(events);
    if (!string_tag) FAIL() << "No such event in:\n" + eventsToString(events);
    ASSERT_EQ(string_tag->value, "Mon agent est tr\xc3\xa8s cool");
}

TEST(caos2prayparser, cp1252_to_utf8) {
    auto events = Caos2PrayParser::parse(std::string(R"(
        *# DS-Name "My Agent"
        *# Agent Description-fr = )") + "\"Un tr\xe8s cool agent\"",
    nullptr);
    auto string_tag = matchEvent<PraySourceParser::StringTag>(events, [](const PraySourceParser::StringTag &e) {
        return e.key == "Agent Description-fr";
    });
    if (!string_tag) FAIL() << "No such event in:\n" + eventsToString(events);
    ASSERT_EQ(string_tag->value, "Un tr\xc3\xa8s cool agent");
}

TEST(caos2prayparser, utf8_to_utf8) {
    auto events = Caos2PrayParser::parse(std::string(R"(
        *# DS-Name "My Agent"
        *# Agent Description-fr = )") + "\"Un tr\xc3\xa8s cool agent\"",
    nullptr);
    auto string_tag = matchEvent<PraySourceParser::StringTag>(events, [](const PraySourceParser::StringTag &e) {
        return e.key == "Agent Description-fr";
    });
    if (!string_tag) FAIL() << "No such event in:\n" + eventsToString(events);
    ASSERT_EQ(string_tag->value, "Un tr\xc3\xa8s cool agent");
}

TEST(praywriter, utf8_to_cp1252) {
    vectorstream v;
    PrayFileWriter writer(v);
    std::map<std::string, std::string> string_tags{{"Repr\xc3\xa8sentation", "Mon agent est tr\xc3\xa8s cool"}};
    std::map<std::string, int> int_tags;
    writer.writeBlockTags("AGNT", "Agent tr\xc3\xa8s cool", int_tags, string_tags);

    EXPECT_EQ(v.vector().size(), 201);

    EXPECT_EQ(v.vector()[16], 0xe8);
    EXPECT_EQ(v.vector()[164], 0xe8);
    EXPECT_EQ(v.vector()[194], 0xe8);
}


TEST(prayreader, windows1252_to_utf8) {
    std::vector<unsigned char> praybytes{
        'P', 'R', 'A', 'Y', 'A', 'G', 'N', 'T', 'A', 'g', 'e', 'n', 't', ' ',
        't', 'r', 0xe8, 's', ' ', 'c', 'o', 'o', 'l', 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
        0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x35, 0x0, 0x0, 0x0,
        0x35, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1, 0x0,
        0x0, 0x0, 0xe, 0x0, 0x0, 0x0, 'R', 'e', 'p', 'r', 0xe8, 's', 'e', 'n',
        't', 'a', 't', 'i', 'o', 'n', 0x17, 0x0, 0x0, 0x0, 'M', 'o', 'n', ' ',
        'a', 'g', 'e', 'n', 't', ' ', 'e', 's', 't', ' ', 't', 'r', 0xe8, 's',
        ' ', 'c', 'o', 'o', 'l'
    };

    spanstream s(praybytes.data(), praybytes.size());

    PrayFileReader reader(s);
    ASSERT_EQ(reader.getNumBlocks(), 1);

    auto tags = reader.getBlockTags(0);
    auto stringValues = tags.second;

    EXPECT_EQ(reader.getBlockName(0), "Agent tr\xc3\xa8s cool");
    ASSERT_EQ(stringValues.size(), 1);
    EXPECT_EQ(stringValues["Repr\xc3\xa8sentation"], "Mon agent est tr\xc3\xa8s cool");
}
