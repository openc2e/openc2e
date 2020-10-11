include(FindPackageHandleStandardArgs)

find_library(UTF8PROC_LIBRARY "utf8proc")
find_path(UTF8PROC_INCLUDE_DIR "utf8proc.h")

mark_as_advanced(UTF8PROC_LIBRARY UTF8PROC_INCLUDE_DIR)
find_package_handle_standard_args(utf8proc DEFAULT_MSG
  UTF8PROC_LIBRARY
  UTF8PROC_INCLUDE_DIR
)

if(UTF8PROC_FOUND)
  add_library(utf8proc::utf8proc UNKNOWN IMPORTED)
  set_target_properties(utf8proc::utf8proc PROPERTIES IMPORTED_LOCATION "${UTF8PROC_LIBRARY}")
  target_include_directories(utf8proc::utf8proc INTERFACE "${UTF8PROC_INCLUDE_DIR}")
  message(STATUS "${UTF8PROC_INCLUDE_DIR}")
endif()