PROJECT (OPENC2E CXX C)

SET(SRC "${OPENC2E_SOURCE_DIR}")
SET(BIN "${OPENC2E_BINARY_DIR}")
SET(GEN "${BIN}/generated")
EXECUTE_PROCESS(COMMAND mkdir -p "${GEN}")

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS 1)
SET(CMAKE_MODULE_PATH "${SRC}/cmake")

SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "The type of build to perform. Valid values are: RelWithDebInfo (default), Debug, Release, MinSizeRel")
IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "The type of build to perform. Valid values are: RelWithDebInfo (default), Debug, Release, MinSizeRel" FORCE)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "")

SET(OPENC2E_USE_QT "TRUE" CACHE BOOL "Build the qt GUI")

SET(OPENC2E_USE_SERIALIZATION "FALSE" CACHE BOOL "Build the experimental serialization code")
MARK_AS_ADVANCED(FORCE OPENC2E_USE_SERIALIZATION)
IF (OPENC2E_USE_SERIALIZATION)
	SET(SER_SRCS src/caos/caosVM_ser_real.cpp)
ELSE (OPENC2E_USE_SERIALIZATION)
	SET(SER_SRCS src/caos/caosVM_ser_stub.cpp)
ENDIF (OPENC2E_USE_SERIALIZATION)

SET(OPENC2E_PROFILE_ALLOCATION "FALSE" CACHE BOOL "Collect allocation profile stats for DBG: SIZO")
MARK_AS_ADVANCED(FORCE OPENC2E_PROFILE_ALLOCATION)
IF (OPENC2E_PROFILE_ALLOCATION)
	ADD_DEFINITIONS("-DPROFILE_ALLOCATION_COUNT -DPROFILE_ALLOCATION_THREAD_SAFE")
ENDIF (OPENC2E_PROFILE_ALLOCATION)

SET(OPENC2E_CORE
	src/Agent.cpp
	src/AgentHelpers.cpp
	src/AgentRef.cpp
	src/alloc_count.cpp
	src/creatures/attFile.cpp
	src/Backend.cpp
	src/creatures/Biochemistry.cpp
	src/Blackboard.cpp
	src/images/blkImage.cpp
	src/images/bmpImage.cpp
	src/Bubble.cpp
	src/bytecode.cpp
	src/images/c16Image.cpp
	src/creatures/c2eBrain.cpp
	src/CallButton.cpp
	src/Camera.cpp
	src/caosScript.cpp
	src/caosVar.cpp
	src/caos/caosVM_agent.cpp
	src/caos/caosVM_camera.cpp
	src/caos/caosVM_compound.cpp
	src/caos/caosVM_core.cpp
	src/caos/caosVM_creatures.cpp
	src/caos/caosVM_debug.cpp
	src/caos/caosVM_files.cpp
	src/caos/caosVM_flow.cpp
	src/caos/caosVM_genetics.cpp
	src/caos/caosVM_history.cpp
	src/caos/caosVM_input.cpp
	src/caos/caosVM_map.cpp
	src/caos/caosVM_motion.cpp
	src/caos/caosVM_net.cpp
	src/caosVM.cpp
	src/caos/caosVM_ports.cpp
	src/caos/caosVM_resources.cpp
	src/caos/caosVM_scripts.cpp
	src/caos/caosVM_sounds.cpp
	src/caos/caosVM_time.cpp
	src/caos/caosVM_variables.cpp
	src/caos/caosVM_vectors.cpp
	src/caos/caosVM_vehicles.cpp
	src/caos/caosVM_world.cpp
	${GEN}/cataloglexer.cpp
	src/catalogparser.cpp
	src/Catalogue.cpp
	${GEN}/cmddata.cpp
	src/cobFile.cpp
	src/CompoundAgent.cpp
	src/creatures/CompoundCreature.cpp
	src/CompoundPart.cpp
	src/creatures/Creature.cpp
	src/creatures/CreatureAgent.cpp
	src/creatures/CreatureAI.cpp
	src/creaturesImage.cpp
	src/dialect.cpp
	src/Engine.cpp
	src/exceptions.cpp
	src/fileSwapper.cpp
	src/creatures/genomeFile.cpp
	src/historyManager.cpp
	src/imageManager.cpp
	${GEN}/mnglexer.cpp
	${GEN}/caoslexer.cpp
	src/Lift.cpp
	src/Map.cpp
	src/MetaRoom.cpp
	src/mmapifstream.cpp
	src/music/mngfile.cpp
	${GEN}/mngparser.tab.cpp
	src/MusicManager.cpp
	src/creatures/oldBrain.cpp
	src/PathResolver.cpp
	src/peFile.cpp
	src/physics.cpp
	src/PointerAgent.cpp
	src/Port.cpp
	src/pray.cpp
	src/prayManager.cpp
	src/renderable.cpp
	src/Room.cpp
	src/Scriptorium.cpp
	src/SFCFile.cpp
	src/SimpleAgent.cpp
	src/creatures/SkeletalCreature.cpp
	src/images/sprImage.cpp
	src/streamutils.cpp
	src/Vehicle.cpp
	src/VoiceData.cpp
	src/World.cpp
	src/main.cpp
	src/util.cpp
)

SET(OPENC2E_SDL
	src/backends/SDLBackend.cpp
)

SET(OPENC2E_QT
	src/backends/qtgui/qtopenc2e.cpp
	src/backends/qtgui/openc2eview.cpp
	src/backends/qtgui/AgentInjector.cpp
	src/backends/qtgui/BrainViewer.cpp
	src/backends/qtgui/imagepreview.cpp
	src/backends/qtgui/c1cobfile.cpp
	src/backends/qtgui/QtBackend.cpp
	src/backends/qtgui/GraphWidget.cpp
	src/backends/qtgui/CreatureGrapher.cpp
	src/backends/qtgui/ChemicalSelector.cpp
	src/backends/qtgui/Hatchery.cpp
	src/tools/braininavat/brainview.cpp)

SET(OPENC2E_QT_MOC_HDRS
	src/backends/qtgui/qtopenc2e.h
	src/backends/qtgui/openc2eview.h
	src/backends/qtgui/AgentInjector.h
	src/backends/qtgui/BrainViewer.h
	src/backends/qtgui/imagepreview.h
	src/backends/qtgui/GraphWidget.h
	src/backends/qtgui/CreatureGrapher.h
	src/backends/qtgui/ChemicalSelector.h
	src/backends/qtgui/Hatchery.h
	src/tools/braininavat/brainview.h)

SET(OPENC2E_QT_UIS
	src/backends/qtgui/agents.ui)
# 


cmake_minimum_required(VERSION 2.4.0)
INCLUDE(TestBigEndian)
INCLUDE(CheckIncludeFile)
TEST_BIG_ENDIAN(BIG_ENDIAN)
IF(BIG_ENDIAN)
	ADD_DEFINITIONS("-DOC2E_BIG_ENDIAN=1")
	CHECK_INCLUDE_FILE(byteswap.h HAVE_BYTESWAP_H)
	IF(HAVE_BYTESWAP_H)
		ADD_DEFINITIONS("-DHAVE_BYTESWAP_H=1")
	ELSE(HAVE_BYTESWAP_H)
		ADD_DEFINITIONS("-DHAVE_BYTESWAP_H=0")
	ENDIF(HAVE_BYTESWAP_H)
ELSE(BIG_ENDIAN)
	ADD_DEFINITIONS("-DOC2E_BIG_ENDIAN=0")
ENDIF(BIG_ENDIAN)
CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)
IF(HAVE_STDINT_H)
	ADD_DEFINITIONS("-DHAVE_STDINT_H=1")
ELSE(HAVE_STDINT_H)
ENDIF(HAVE_STDINT_H)

find_package(Python REQUIRED)

FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(SDL_mixer)
FIND_PACKAGE(SDL_net REQUIRED)
FIND_PACKAGE(SDL_gfx REQUIRED)
FIND_PACKAGE(SDL_ttf REQUIRED)

FIND_PACKAGE(Boost 1.34.0 COMPONENTS program_options serialization REQUIRED)
IF(OPENC2E_USE_QT)
	find_package(Qt4 REQUIRED)
ENDIF(OPENC2E_USE_QT)

add_subdirectory(externals/ghc_filesystem EXCLUDE_FROM_ALL)

IF(SDLMIXER_FOUND)
    SET(SDLMIXER_SRC "src/backends/SDLMixerBackend.cpp")
    SET(SDLMIXER_DEF "-DSDLMIXER_SUPPORT")
ELSE(SDLMIXER_FOUND)
    SET(SDLMIXER_INCLUDE_DIR "")
    SET(SDLMIXER_LIBRARY "")
    SET(SDLMIXER_SRC "")
    SET(SDLMIXER_DEF "")
ENDIF(SDLMIXER_FOUND)

IF(OPENC2E_USE_QT)
	include(${QT_USE_FILE})
	SET(SAVED_CMAKE_CURRENT_BINARY_DIR CMAKE_CURRENT_BINARY_DIR)
	SET(CMAKE_CURRENT_BINARY_DIR ${GEN})
	QT4_WRAP_UI(OPENC2E_QT_UIS_H ${OPENC2E_QT_UIS})
	QT4_WRAP_CPP(OPENC2E_QT_MOC_SRCS ${OPENC2E_QT_MOC_HDRS})
	SET(CMAKE_CURRENT_BINARY_DIR SAVED_CMAKE_CURRENT_BINARY_DIR)
	SET(QT_DEF "-DQT_SUPPORT")
ENDIF(OPENC2E_USE_QT)

# Pass flags to the compiler
ADD_DEFINITIONS("-W -Wall -Wno-conversion -Wno-unused -D_REENTRANT -DYYERROR_VERBOSE ${QT_DEF} ${SDLMIXER_DEF}")
set(CMAKE_CXX_STANDARD 11)

IF(OPENC2E_USE_QT)
	SET(FRONTEND_SRCS ${OPENC2E_SDL} ${OPENC2E_QT} ${OPENC2E_QT_UIS_H} ${OPENC2E_QT_MOC_SRCS})
	SET(FRONTEND_LIBS ${QT_LIBRARIES})
ELSE(OPENC2E_USE_QT)
	SET(FRONTEND_SRCS ${OPENC2E_SDL})
	SET(FRONTEND_LIBS)
ENDIF(OPENC2E_USE_QT)
ADD_EXECUTABLE(openc2e ${OPENC2E_CORE} ${FRONTEND_SRCS} ${SDLMIXER_SRC} ${SER_SRCS})

TARGET_LINK_LIBRARIES(openc2e z m pthread
	${SDL_LIBRARY}
	${SDLNET_LIBRARY}
	${SDLTTF_LIBRARY}
	${SDLGFX_LIBRARY}
	${SDLMIXER_LIBRARY}
	${FRONTEND_LIBS}
	Boost::program_options
	Boost::serialization
	ghc_filesystem
	)

INCLUDE_DIRECTORIES(BEFORE
	${GEN}
	${SRC}/src
	${SDL_INCLUDE_DIR}
	${SDLNET_INCLUDE_DIR}
	${SDLTTF_INCLUDE_DIR}
	${SDLGFX_INCLUDE_DIR}
	${SDLMIXER_INCLUDE_DIR}
	)

FILE(GLOB CAOSVM_FILES ${SRC}/src/caos/caosVM_*.cpp)
ADD_CUSTOM_COMMAND(
	OUTPUT ${GEN}/commandinfo.json
	COMMAND ${Python_EXECUTABLE} parsedocs.py ${CAOSVM_FILES} > ${GEN}/commandinfo.json
	DEPENDS parsedocs.py ${CAOSVM_FILES}
	WORKING_DIRECTORY ${SRC})

ADD_CUSTOM_COMMAND(
	OUTPUT ${BIN}/caosdocs.html
	COMMAND ${Python_EXECUTABLE} docs/writehtml.py ${GEN}/commandinfo.json > ${BIN}/caosdocs.html
	DEPENDS docs/writehtml.py ${GEN}/commandinfo.json
	WORKING_DIRECTORY ${SRC})
ADD_CUSTOM_COMMAND(
	OUTPUT ${BIN}/docs.css
	COMMAND cp docs/docs.css ${BIN}/docs.css
	DEPENDS docs/docs.css
	WORKING_DIRECTORY ${SRC})
ADD_CUSTOM_COMMAND(
	OUTPUT ${BIN}/openc2e.6
	COMMAND pod2man --center="Openc2e Documentation" --section=6 ${SRC}/docs/openc2e.pod ${BIN}/openc2e.6
	DEPENDS docs/openc2e.pod
	)
# it seems these ADD_CUSTOM_COMMANDS for flex and bison could be made
# into a macro... e.g. FLEX(file1) FLEX(file2) etc...
ADD_CUSTOM_COMMAND(
	OUTPUT ${GEN}/cataloglexer.cpp
	COMMAND re2c -o ${GEN}/cataloglexer.cpp ${SRC}/src/cataloglexer.re2c
	DEPENDS ${SRC}/src/cataloglexer.re2c
	WORKING_DIRECTORY ${SRC})

ADD_CUSTOM_COMMAND(
	OUTPUT ${GEN}/cmddata.cpp
	COMMAND ${Python_EXECUTABLE} writecmds.py ${GEN}/commandinfo.json > ${GEN}/cmddata.cpp
	DEPENDS ${GEN}/commandinfo.json writecmds.py
	WORKING_DIRECTORY ${SRC})

ADD_CUSTOM_COMMAND(
	OUTPUT ${GEN}/caoslexer.cpp
	COMMAND re2c -o ${GEN}/caoslexer.cpp ${SRC}/src/caoslexer.re2c
	DEPENDS ${SRC}/src/caoslexer.re2c
	WORKING_DIRECTORY ${SRC})

ADD_CUSTOM_COMMAND(
	OUTPUT ${GEN}/mnglexer.cpp
	COMMAND re2c -o ${GEN}/mnglexer.cpp ${SRC}/src/music/mnglexer.re2c
	DEPENDS ${SRC}/src/music/mnglexer.re2c
	WORKING_DIRECTORY ${SRC})

ADD_CUSTOM_COMMAND(
	OUTPUT ${GEN}/mngparser.tab.cpp ${GEN}/mngparser.tab.hpp
	COMMAND bison -d --name-prefix=mng --file-prefix="${GEN}/mngparser" src/music/mngparser.ypp
	DEPENDS src/music/mngparser.ypp
	WORKING_DIRECTORY ${SRC})

ADD_CUSTOM_TARGET(test DEPENDS openc2e
	COMMAND perl ${SRC}/runtests.pl ${SRC}/tests)
ADD_CUSTOM_TARGET(docs ALL DEPENDS ${BIN}/caosdocs.html ${BIN}/docs.css ${BIN}/openc2e.6)
ADD_CUSTOM_TARGET(generated DEPENDS
	${GEN}/caoslexer.cpp
	${GEN}/mngparser.tab.cpp ${GEN}/mngparser.tab.hpp
	${GEN}/mnglexer.cpp
	${GEN}/cataloglexer.cpp
	${GEN}/cmddata.cpp
)

