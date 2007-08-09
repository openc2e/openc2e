# ***************************************************************************
# *   Copyright (C) 2007 The OpenAnno team                                  *
# *   team@openanno.org                                                     *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
# ***************************************************************************

# - Locate SDL_gfx library
# This module defines
#  SDLGFX_LIBRARY, the library to link against
#  SDLGFX_FOUND, if false, do not try to link to SDL_gfx
#  SDLGFX_INCLUDE_DIR, where to find e.g. SDL/SDL_rotozoom.h
#   
# $SDLDIR is an environment variable that would
# correspond to the ./configure --prefix=$SDLDIR
# used in building SDL.
# Created by Philipp Kerling. This was influenced by the FindSDL_ttf.cmake 
# module.
FIND_PATH(SDLGFX_INCLUDE_DIR SDL_rotozoom.h
  $ENV{SDLGFXDIR}/include
  $ENV{SDLDIR}/include
  ~/Library/Frameworks/SDL_gfx.framework/Headers
  /Library/Frameworks/SDL_gfx.framework/Headers
  /usr/local/include/SDL
  /usr/include/SDL
  /usr/local/include/SDL12
  /usr/local/include/SDL11 # FreeBSD ports
  /usr/include/SDL12
  /usr/include/SDL11
  /usr/local/include
  /usr/include
  /sw/include/SDL # Fink
  /sw/include
  /opt/local/include/SDL # DarwinPorts
  /opt/local/include
  /opt/csw/include/SDL # Blastwave
  /opt/csw/include 
  /opt/include/SDL
  /opt/include
  )
# I'm not sure if I should do a special casing for Apple. It is 
# unlikely that other Unix systems will find the framework path.
# But if they do ([Next|Open|GNU]Step?), 
# do they want the -framework option also?
IF(${SDLGFX_INCLUDE_DIR} MATCHES ".framework")
  # Extract the path the framework resides in so we can use it for the -F flag
  STRING(REGEX REPLACE "(.*)/.*\\.framework/.*" "\\1" SDLGFX_FRAMEWORK_PATH_TEMP ${SDLGFX_INCLUDE_DIR})
  IF("${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/Library/Frameworks"
      OR "${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/System/Library/Frameworks"
      )
    # String is in default search path, don't need to use -F
    SET(SDLGFX_LIBRARY "-framework SDL_gfx" CACHE STRING "SDL_gfx framework for OSX")
  ELSE("${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/Library/Frameworks"
      OR "${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/System/Library/Frameworks"
      )
    # String is not /Library/Frameworks, need to use -F
    SET(SDLGFX_LIBRARY "-F${SDLGFX_FRAMEWORK_PATH_TEMP} -framework SDL_gfx" CACHE STRING "SDL_gfx framework for OSX")
  ENDIF("${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/Library/Frameworks"
    OR "${SDLGFX_FRAMEWORK_PATH_TEMP}" STREQUAL "/System/Library/Frameworks"
    )
  # Clear the temp variable so nobody can see it
  SET(SDLGFX_FRAMEWORK_PATH_TEMP "" CACHE INTERNAL "")

ELSE(${SDLGFX_INCLUDE_DIR} MATCHES ".framework")
  FIND_LIBRARY(SDLGFX_LIBRARY 
    NAMES SDL_gfx
    PATHS
    $ENV{SDLGFXDIR}/lib
    $ENV{SDLDIR}/lib
    /usr/local/lib
    /usr/lib
    /sw/lib
    /opt/local/lib
    /opt/csw/lib
    /opt/lib
    )
ENDIF(${SDLGFX_INCLUDE_DIR} MATCHES ".framework")

SET(SDLGFX_FOUND "NO")
IF(SDLGFX_LIBRARY)
  SET(SDLGFX_FOUND "YES")
ENDIF(SDLGFX_LIBRARY)

