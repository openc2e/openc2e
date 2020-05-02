if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.17.0")
	list(APPEND CMAKE_CTEST_ARGUMENTS "--output-on-failure")
elseif(NOT WINDOWS)
	message("")
	message(WARNING "CTest will hide test output even on failure. To fix this, either:
 1) Upgrade to CMake 3.17; or
 2) Set CTEST_OUTPUT_ON_FAILURE=1 in your shell")
endif()
