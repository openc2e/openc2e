include(FindPackageHandleStandardArgs)

find_library(SDL2_LIBRARY "SDL2")
find_path(SDL2_INCLUDE_DIR "SDL2/SDL.h")

mark_as_advanced(SDL2_LIBRARY SDL2_INCLUDE_DIR)
find_package_handle_standard_args(SDL2 DEFAULT_MSG
  SDL2_LIBRARY
  SDL2_INCLUDE_DIR
)

if(SDL2_FOUND)
  add_library(SDL2::SDL2 UNKNOWN IMPORTED)
  set_target_properties(SDL2::SDL2 PROPERTIES IMPORTED_LOCATION "${SDL2_LIBRARY}")
  target_include_directories(SDL2::SDL2 INTERFACE "${SDL2_INCLUDE_DIR}/SDL2")
endif()