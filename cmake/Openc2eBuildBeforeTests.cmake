file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/ctest_build_openc2e_first.cmake" "
	execute_process(
		COMMAND \"${CMAKE_COMMAND}\" --build \"${CMAKE_BINARY_DIR}\" --parallel
		RESULT_VARIABLE exit_code
	)
	if(NOT exit_code EQUAL 0)
		message(FATAL_ERROR \"Build failed\")
	endif()
")
set_property(DIRECTORY
  APPEND PROPERTY TEST_INCLUDE_FILES "${CMAKE_CURRENT_BINARY_DIR}/ctest_build_openc2e_first.cmake"
)