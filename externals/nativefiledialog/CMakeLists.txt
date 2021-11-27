project(nativefiledialog)

add_library(nativefiledialog
    src/nfd_common.c
    $<$<PLATFORM_ID:Windows>:src/nfd_win.cpp>
    $<$<PLATFORM_ID:Darwin>:src/nfd_cocoa.m>
    $<$<NOT:$<OR:$<PLATFORM_ID:Windows>,$<PLATFORM_ID:Darwin>>>:src/nfd_zenity.c>
)
target_include_directories(nativefiledialog PUBLIC src/include)