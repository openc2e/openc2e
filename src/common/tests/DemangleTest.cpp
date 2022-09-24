#include "common/demangle.h"

#include <gtest/gtest.h>
#include <string>
#include <typeinfo>

struct mytype {};

TEST(Demangle, demangle) {
	mytype e;

	std::string name = typeid(e).name();
	std::string demangled = demangle(name.c_str());

	EXPECT_TRUE(demangled == "mytype" || demangled == name)
		<< "Actual value: \"" << demangled << "\", expected \"mytype\" or \""
		<< name << "\".";
}