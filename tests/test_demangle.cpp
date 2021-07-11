#include <gtest/gtest.h>

#include "utils/demangle.h"
#include <string>
#include <typeinfo>

class myexception {};

TEST(demangle, demangle) {
  myexception e;

  std::string name = typeid(e).name();
  std::string demangled = demangle(name.c_str());

  EXPECT_TRUE(demangled == "myexception" || demangled == name)
      << "Actual value: \"" << demangled << "\", expected \"myexception\" or \""
      << name << "\".";
}
