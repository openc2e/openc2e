#include "fileformats/cfgFile.h"

#include "common/spanstream.h"

#include <gtest/gtest.h>

TEST(fileformats, CfgFile) {
	std::string buffer = R"(
		"Backgrounds Directory" Backgrounds
		"Body Data Directory" "Body Data"
		"Display Type" 0

		"Main Directory" .

		Language en-GB
		"Default Background" DS_splash.blk
		"Server 0 Host" 127.0.0.1
	)";
	spanstream in(buffer);

	auto cfg = readcfgfile(in);
	EXPECT_EQ(cfg["Backgrounds Directory"], "Backgrounds");
	EXPECT_EQ(cfg["Body Data Directory"], "Body Data");
	EXPECT_EQ(cfg["Display Type"], "0");
	EXPECT_EQ(cfg["Main Directory"], ".");
	EXPECT_EQ(cfg["Language"], "en-GB");
	EXPECT_EQ(cfg["Default Background"], "DS_splash.blk");
	EXPECT_EQ(cfg["Server 0 Host"], "127.0.0.1");
}