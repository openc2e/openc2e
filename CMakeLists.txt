PROJECT (OPENC2E CXX C)

SET(SRC "${OPENC2E_SOURCE_DIR}")
SET(BIN "${OPENC2E_BINARY_DIR}")
SET(GEN "${BIN}/generated")
EXECUTE_PROCESS(COMMAND mkdir "${GEN}" -p)

SET(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS 1)
SET(CMAKE_MODULE_PATH "${SRC}/cmake")

SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "The type of build to perform. Valid values are: RelWithDebInfo (default), Debug, Release, MinSizeRel")
IF (CMAKE_BUILD_TYPE STREQUAL "")
	SET(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "The type of build to perform. Valid values are: RelWithDebInfo (default), Debug, Release, MinSizeRel" FORCE)
ENDIF (CMAKE_BUILD_TYPE STREQUAL "")

SET(OPENC2E_USE_QT "TRUE" CACHE BOOL "Build the qt GUI")
SET(OPENC2E_USE_OPENAL "AUTO" CACHE STRING "Use OpenAL (YES/NO/AUTO)")
IF (OPENC2E_USE_OPENAL MATCHES "^(YES|NO|AUTO)$")
ELSE (OPENC2E_USE_OPENAL MATCHES "^(YES|NO|AUTO)$")
	MESSAGE(SEND_ERROR "The value of OPENC2E_USE_OPENAL must be YES, NO, or AUTO.")
ENDIF (OPENC2E_USE_OPENAL MATCHES "^(YES|NO|AUTO)$")

SET(OPENC2E_USE_SERIALIZATION "FALSE" CACHE BOOL "Build the experimental serialization code")
MARK_AS_ADVANCED(FORCE OPENC2E_USE_SERIALIZATION)
IF (OPENC2E_USE_SERIALIZATION)
	SET(SER_SRCS caosVM_ser_real.cpp)
ELSE (OPENC2E_USE_SERIALIZATION)
	SET(SER_SRCS caosVM_ser_stub.cpp)
ENDIF (OPENC2E_USE_SERIALIZATION)


SET(OPENC2E_CORE
	Agent.cpp
	AgentHelpers.cpp
	AgentRef.cpp
	attFile.cpp
	Backend.cpp
	Blackboard.cpp
	blkImage.cpp
	bmpImage.cpp
	bytecode.cpp
	c16Image.cpp
	c2eBrain.cpp
	CallButton.cpp
	Camera.cpp
	caosScript.cpp
	caosVar.cpp
	caosVM_agent.cpp
	caosVM_camera.cpp
	caosVM_compound.cpp
	caosVM_core.cpp
	caosVM_creatures.cpp
	caosVM_debug.cpp
	caosVM_files.cpp
	caosVM_flow.cpp
	caosVM_genetics.cpp
	caosVM_history.cpp
	caosVM_input.cpp
	caosVM_map.cpp
	caosVM_motion.cpp
	caosVM_net.cpp
	caosVM.cpp
	caosVM_ports.cpp
	caosVM_resources.cpp
	caosVM_scripts.cpp
	caosVM_sounds.cpp
	caosVM_time.cpp
	caosVM_variables.cpp
	caosVM_vectors.cpp
	caosVM_vehicles.cpp
	caosVM_world.cpp
	${GEN}/cataloglexer.cpp
	Catalogue.cpp
	${GEN}/catalogue.tab.cpp
	${GEN}/cmddata.cpp
	cobFile.cpp
	CompoundAgent.cpp
	CompoundPart.cpp
	Creature.cpp
	CreatureAgent.cpp
	creaturesImage.cpp
	dialect.cpp
	Engine.cpp
	exceptions.cpp
	fileSwapper.cpp
	genomeFile.cpp
	historyManager.cpp
	imageManager.cpp
	#	${GEN}/lex.mng.cpp
	${GEN}/caoslexer.cpp
	Lift.cpp
	Map.cpp
	MetaRoom.cpp
	mmapifstream.cpp
	#	mngfile.cpp
	#	${GEN}/mngparser.tab.cpp
	oldBrain.cpp
	PathResolver.cpp
	physics.cpp
	PointerAgent.cpp
	pray.cpp
	prayManager.cpp
	renderable.cpp
	Room.cpp
	Scriptorium.cpp
	SDLBackend.cpp
	SFCFile.cpp
	SimpleAgent.cpp
	SkeletalCreature.cpp
	sprImage.cpp
	streamutils.cpp
	Vehicle.cpp
	World.cpp
	main.cpp
	util.cpp
)

SET(OPENC2E_SDL
	# all in the main openc2e listing
	# XXX: non-sdl-based backends?
)

SET(OPENC2E_QT
	qtgui/qtopenc2e.cpp
	qtgui/openc2eview.cpp
	qtgui/AgentInjector.cpp
	qtgui/BrainViewer.cpp
	qtgui/imagepreview.cpp
	qtgui/c1cobfile.cpp
	qtgui/QtBackend.cpp
	qtgui/GraphWidget.cpp
	qtgui/CreatureGrapher.cpp
	tools/braininavat/brainview.cpp)

SET(OPENC2E_QT_MOC_HDRS
	qtgui/qtopenc2e.h
	qtgui/openc2eview.h
	qtgui/AgentInjector.h
	qtgui/BrainViewer.h
	qtgui/imagepreview.h
	qtgui/GraphWidget.h
	qtgui/CreatureGrapher.h
	tools/braininavat/brainview.h)

SET(OPENC2E_QT_UIS
	qtgui/agents.ui)
# 


cmake_minimum_required(VERSION 2.4.0)

FIND_PACKAGE(SDL REQUIRED)
FIND_PACKAGE(SDL_mixer REQUIRED)
FIND_PACKAGE(SDL_net REQUIRED)
FIND_PACKAGE(SDL_gfx REQUIRED)
IF(OPENC2E_USE_OPENAL MATCHES "^(YES|AUTO)$")
	FIND_PACKAGE(OpenAL)
ELSE(OPENC2E_USE_OPENAL MATCHES "^(YES|AUTO)$")
	SET(OPENAL_FOUND "NO")
ENDIF(OPENC2E_USE_OPENAL MATCHES "^(YES|AUTO)$")
FIND_PACKAGE(ALUT)
FIND_PACKAGE(Boost REQUIRED)
IF(OPENC2E_USE_QT)
	find_package(Qt4 REQUIRED)
ENDIF(OPENC2E_USE_QT)

SET(USE_OPENAL "NO")

IF(OPENAL_FOUND STREQUAL "YES")
    IF(ALUT_FOUND STREQUAL "YES")
        SET(USE_OPENAL "YES")
    ENDIF(ALUT_FOUND STREQUAL "YES")
ENDIF(OPENAL_FOUND STREQUAL "YES")
     
IF(USE_OPENAL STREQUAL "NO")
	IF (OPENC2E_USE_OPENAL MATCHES "^YES")
		MESSAGE(FATAL_ERROR "OpenAL not found.")
	ENDIF (OPENC2E_USE_OPENAL MATCHES "^YES")
	
    SET(ALUT_INCLUDE_DIR "")
    SET(OPENAL_INCLUDE_DIR "")
    SET(ALUT_LIBRARY "")
    SET(OPENAL_LIBRARY "")
    SET(OPENAL_SRC "")
    SET(OPENAL_DEF "")
ELSE(OPENAL_SRC STREQUAL "")
    SET(OPENAL_SRC "OpenALBackend.cpp")
    SET(OPENAL_DEF "-DOPENAL_SUPPORT")
ENDIF(OPENAL_SRC STREQUAL "")

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
ADD_DEFINITIONS("-W -Wall -Wno-conversion -Wno-unused -pthread -D_REENTRANT -DYYERROR_VERBOSE ${OPENAL_DEF} ${QT_DEF}")

IF(OPENC2E_USE_QT)
	SET(FRONTEND_SRCS ${OPENC2E_QT} ${OPENC2E_QT_UIS_H} ${OPENC2E_QT_MOC_SRCS})
	SET(FRONTEND_LIBS ${QT_LIBRARIES})
ELSE(OPENC2E_USE_QT)
	SET(FRONTEND_SRCS ${OPENC2E_SDL})
	SET(FRONTEND_LIBS)
ENDIF(OPENC2E_USE_QT)
ADD_EXECUTABLE(openc2e ${OPENC2E_CORE} ${FRONTEND_SRCS} ${OPENAL_SRC} ${SER_SRCS})

TARGET_LINK_LIBRARIES(openc2e z m pthread 
	${SDL_LIBRARY}
	${SDLMIXER_LIBRARY} 
	${SDLNET_LIBRARY}
	${SDLGFX_LIBRARY}
	${OPENAL_LIBRARY}
	${ALUT_LIBRARY}
	${FRONTEND_LIBS}
	boost_program_options-mt
	boost_serialization-mt
	boost_filesystem-mt
	boost_thread-mt
	boost_regex-mt
	)

INCLUDE_DIRECTORIES(BEFORE
	${GEN}
	${SRC}
	${SDL_INCLUDE_DIR}
	${SDLMIXER_INCLUDE_DIR}
	${SDLNET_INCLUDE_DIR}
	${SDLGFX_INCLUDE_DIR}
	${OPENAL_INCLUDE_DIR}
	${ALUT_INCLUDE_DIR}
	)

FILE(GLOB CAOSVM_FILES ${SRC}/caosVM_*.cpp)
ADD_CUSTOM_COMMAND(
	OUTPUT ${GEN}/commandinfo.yml
	COMMAND perl parsedocs.pl ${CAOSVM_FILES} > ${GEN}/commandinfo.yml
	DEPENDS parsedocs.pl ${CAOSVM_FILES}
	WORKING_DIRECTORY ${SRC})

ADD_CUSTOM_COMMAND(
	OUTPUT ${BIN}/caosdocs.html
	COMMAND perl writehtml.pl ${GEN}/commandinfo.yml > ${BIN}/caosdocs.html
	DEPENDS writehtml.pl ${GEN}/commandinfo.yml
	WORKING_DIRECTORY ${SRC})
ADD_CUSTOM_COMMAND(
	OUTPUT ${BIN}/docs.css
	COMMAND cp docs.css ${BIN}/docs.css -f
	DEPENDS docs.css
	WORKING_DIRECTORY ${SRC})
ADD_CUSTOM_COMMAND(
	OUTPUT ${BIN}/openc2e.6
	COMMAND pod2man --center="Openc2e Documentation" --section=6 ${SRC}/openc2e.pod ${BIN}/openc2e.6
	DEPENDS openc2e.pod
	)
# it seems these ADD_CUSTOM_COMMANDS for flex and bison could be made
# into a macro... e.g. FLEX(file1) FLEX(file2) etc...
ADD_CUSTOM_COMMAND(
	OUTPUT ${GEN}/cataloglexer.cpp
	COMMAND re2c -o ${GEN}/cataloglexer.cpp ${SRC}/cataloglexer.re2c
	DEPENDS ${SRC}/cataloglexer.re2c
	WORKING_DIRECTORY ${SRC})

ADD_CUSTOM_COMMAND(
	OUTPUT ${GEN}/catalogue.tab.cpp ${GEN}/catalogue.tab.hpp
	COMMAND bison -d --name-prefix=cata --file-prefix="${GEN}/catalogue" catalogue.ypp
	DEPENDS catalogue.ypp
	WORKING_DIRECTORY ${SRC})

ADD_CUSTOM_COMMAND(
	OUTPUT ${GEN}/cmddata.cpp
	COMMAND perl writecmds.pl ${GEN}/commandinfo.yml > ${GEN}/cmddata.cpp
	DEPENDS ${GEN}/commandinfo.yml writecmds.pl
	WORKING_DIRECTORY ${SRC})

ADD_CUSTOM_COMMAND(
	OUTPUT ${GEN}/caoslexer.cpp
	COMMAND re2c -o ${GEN}/caoslexer.cpp ${SRC}/caoslexer.re2c
	DEPENDS ${SRC}/caoslexer.re2c
	WORKING_DIRECTORY ${SRC})

#ADD_CUSTOM_COMMAND(
#	OUTPUT ${GEN}/lex.mng.cpp ${GEN}/lex.mng.h
#	COMMAND flex -+ --prefix=mng -d -o ${GEN}/lex.mng.cpp --header-file=${GEN}/lex.mng.h mng.l
#	DEPENDS mng.l
#	WORKING_DIRECTORY ${SRC})
#
#ADD_CUSTOM_COMMAND(
#	OUTPUT ${GEN}/mngparser.tab.cpp ${GEN}/mngparser.tab.hpp
#	COMMAND bison -d --name-prefix=mng --file-prefix="${GEN}/mngparser" mngparser.ypp
#	DEPENDS mngparser.ypp
#	WORKING_DIRECTORY ${SRC})

ADD_CUSTOM_TARGET(docs ALL DEPENDS ${BIN}/caosdocs.html ${BIN}/docs.css ${BIN}/openc2e.6)
ADD_CUSTOM_TARGET(generated DEPENDS
	${GEN}/catalogue.tab.cpp ${GEN}/catalogue.tab.hpp
	${GEN}/caoslexer.cpp
	${GEN}/cataloglexer.cpp
	${GEN}/cmddata.cpp
	#	${GEN}/lex.mng.cpp ${GEN}/lex.mng.h
	#	${GEN}/mngparser.tab.cpp ${GEN}/mngparser.tab.hpp
)

