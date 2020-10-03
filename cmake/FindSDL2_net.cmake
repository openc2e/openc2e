include(FindPackageHandleStandardArgs)

find_library(SDL2_NET_LIBRARY "SDL2_net")
find_path(SDL2_NET_INCLUDE_DIR "SDL2/SDL_net.h")

mark_as_advanced(SDL2_NET_LIBRARY SDL2_NET_INCLUDE_DIR)
find_package_handle_standard_args(SDL2_net DEFAULT_MSG
  SDL2_NET_LIBRARY
  SDL2_NET_INCLUDE_DIR
)

if(SDL2_NET_FOUND)
  add_library(SDL2::Net UNKNOWN IMPORTED)
  set_target_properties(SDL2::Net PROPERTIES IMPORTED_LOCATION "${SDL2_NET_LIBRARY}")
  target_include_directories(SDL2::Net INTERFACE "${SDL2_NET_INCLUDE_DIR}/SDL2")
  if(NOT TARGET SDL2::SDL2)
    # otherwise CMake reports an error on the end targets, which is confusing
    message(FATAL_ERROR "Could NOT find target SDL2::SDL2")
  endif()
  target_link_libraries(SDL2::Net INTERFACE SDL2::SDL2)
endif()