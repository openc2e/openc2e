#include "fileformats/catalogueFile.h"

#include "common/io/SpanReader.h"

#include <gtest/gtest.h>

TEST(fileformats, CatalogueFile) {
	std::string buffer = R"(
        # this is a comment
        
        TAG "Message Centre Text"
        # 0
        "Only the first 7 of your" # maybe 8?
        "messages are shown as your Inbox is full." # done
        
        ARRAY "Learnt Specials" 3
        "yes" # positive
        "no" # bad punish
        "look"
        
        TAG OVERRIDE "About Box"
        "Docking Station"
        
        ** Iconised Chat Window
        
        ARRAY OVERRIDE "Agent Categories" 3
        # 0
        "self"
        "hand"
        "door"
    )";
	SpanReader in(buffer);

	auto cat = readCatalogueFile(in);

	EXPECT_EQ(cat.tags.size(), 4);

	EXPECT_EQ(cat.tags[0].type, CatalogueTag::TYPE_TAG);
	EXPECT_FALSE(cat.tags[0].override);
	EXPECT_EQ(cat.tags[0].name, "Message Centre Text");
	EXPECT_EQ(cat.tags[0].values.size(), 2);
	EXPECT_EQ(cat.tags[0].values[0], "Only the first 7 of your");
	EXPECT_EQ(cat.tags[0].values[1], "messages are shown as your Inbox is full.");

	EXPECT_EQ(cat.tags[1].type, CatalogueTag::TYPE_ARRAY);
	EXPECT_FALSE(cat.tags[1].override);
	EXPECT_EQ(cat.tags[1].name, "Learnt Specials");
	EXPECT_EQ(cat.tags[1].values.size(), 3);
	EXPECT_EQ(cat.tags[1].values[0], "yes");
	EXPECT_EQ(cat.tags[1].values[1], "no");
	EXPECT_EQ(cat.tags[1].values[2], "look");

	EXPECT_EQ(cat.tags[2].type, CatalogueTag::TYPE_TAG);
	EXPECT_TRUE(cat.tags[2].override);
	EXPECT_EQ(cat.tags[2].name, "About Box");
	EXPECT_EQ(cat.tags[2].values.size(), 1);
	EXPECT_EQ(cat.tags[2].values[0], "Docking Station");

	EXPECT_EQ(cat.tags[3].type, CatalogueTag::TYPE_ARRAY);
	EXPECT_TRUE(cat.tags[3].override);
	EXPECT_EQ(cat.tags[3].name, "Agent Categories");
	EXPECT_EQ(cat.tags[3].values.size(), 3);
	EXPECT_EQ(cat.tags[3].values[0], "self");
	EXPECT_EQ(cat.tags[3].values[1], "hand");
	EXPECT_EQ(cat.tags[3].values[2], "door");
}

TEST(fileformats, CatalogueFileEndingCommentWithoutNewline) {
	std::string buffer = R"(
		TAG "Message Centre Text"
        "My tag"
# comment )";
	SpanReader in(buffer);
	// expect to not throw
	readCatalogueFile(in);
}