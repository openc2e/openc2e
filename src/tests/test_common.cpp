#include "common/demangle.h"
#include "common/iendswith.h"
#include "common/wildcard_match.h"

#include <gtest/gtest.h>
#include <string>
#include <typeinfo>

class myexception {};

TEST(common, demangle) {
	myexception e;

	std::string name = typeid(e).name();
	std::string demangled = demangle(name.c_str());

	EXPECT_TRUE(demangled == "myexception" || demangled == name)
		<< "Actual value: \"" << demangled << "\", expected \"myexception\" or \""
		<< name << "\".";
}

TEST(common, iendswith) {
	EXPECT_TRUE(iendswith("my agent.catalogue", ".catalogue"));
	EXPECT_TRUE(iendswith("my agent.CATalOgue", ".catalogue"));
	EXPECT_FALSE(iendswith("my agent.cataloguep", ".catalogue"));
	EXPECT_FALSE(iendswith("my agent.s16", ".catalogue"));
	EXPECT_TRUE(iendswith("my agent.s16", ".s16"));
}

TEST(common, wildcard_match) {
	EXPECT_FALSE(wildcard_match("hello", "nope"));
	EXPECT_TRUE(wildcard_match("hello", "hello"));
	EXPECT_TRUE(wildcard_match("hel?o", "hello"));
	EXPECT_TRUE(wildcard_match("h*o", "hello"));
	EXPECT_TRUE(wildcard_match("h*l*o", "hello"));
}