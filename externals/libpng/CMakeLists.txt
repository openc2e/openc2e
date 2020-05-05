project(png VERSION 1.6.37 LANGUAGES C)

add_library(png STATIC
  png.h
  pngconf.h
  png.c
  pngerror.c
  pngget.c
  pngmem.c
  pngpread.c
  pngread.c
  pngrio.c
  pngrtran.c
  pngrutil.c
  pngset.c
  pngtrans.c
  pngwio.c
  pngwrite.c
  pngwtran.c
  pngwutil.c
)
target_include_directories(png PUBLIC .)
target_link_libraries(png PUBLIC ZLIB::ZLIB)

add_library(PNG::PNG ALIAS png)
