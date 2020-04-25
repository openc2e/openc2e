set(prefix "${TEST_PREFIX}")
set(suffix "${TEST_SUFFIX}")
set(extra_args ${TEST_EXTRA_ARGS})
set(script)
set(suite)

function(add_command NAME)
  set(_args "")
  foreach(_arg ${ARGN})
    set(_args "${_args} [==[${_arg}]==]")
  endforeach()
  set(script "${script}${NAME}(${_args})\n" PARENT_SCOPE)
endfunction()

# Run test executable to get list of available tests
if(NOT EXISTS "${TEST_EXECUTABLE}")
  message(FATAL_ERROR
    "Specified test executable '${TEST_EXECUTABLE}' does not exist"
  )
endif()
execute_process(
  COMMAND "${TEST_EXECUTABLE}" --gtest_list_tests
  OUTPUT_VARIABLE output
  RESULT_VARIABLE result
)
if(NOT ${result} EQUAL 0)
  message(FATAL_ERROR
    "Error running test executable '${TEST_EXECUTABLE}'"
  )
endif()

string(REPLACE "\n" ";" output "${output}")

# Parse output
foreach(line ${output})
  # Skip header
  if(NOT line MATCHES "gtest_main\\.cc")
    # Do we have a module name or a test name?
    if(NOT line MATCHES "^  ")
      # Module; remove trailing '.' to get just the name...
      string(REGEX REPLACE "\\.$" "" suite "${line}")
    else()
      # Test name; strip spaces to get just the name...
      string(REGEX REPLACE " +" "" test "${line}")
      # ...and add to script
      add_command(add_test
        "${prefix}${suite}.${test}${suffix}"
        "${TEST_EXECUTABLE}"
        "--gtest_filter=${suite}.${test}"
        ${extra_args}
      )
      add_command(set_tests_properties
        "${suite}.${test}" PROPERTIES WORKING_DIRECTORY
        "${TEST_WORKING_DIR}"
      )
    endif()
  endif()
endforeach()

# Write CTest script
file(WRITE "${CTEST_FILE}" "${script}")