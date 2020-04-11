project(SDL2_net)

add_library(SDL2_net SDLnet.c SDLnetselect.c SDLnetTCP.c SDLnetUDP.c)
target_link_libraries(SDL2_net PRIVATE SDL2)
target_include_directories(SDL2_net PUBLIC .)
if(WIN32)
  target_link_libraries(SDL2_net PRIVATE wsock32 iphlpapi)
endif()