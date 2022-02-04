cmake_minimum_required(VERSION 3.3)
project(openc2e)

set(SRC "${CMAKE_CURRENT_SOURCE_DIR}")
set(BIN "${CMAKE_CURRENT_BINARY_DIR}")
set(GEN "${BIN}/generated")

list(APPEND CMAKE_MODULE_PATH "${SRC}/cmake")
if(EMSCRIPTEN)
	list(APPEND CMAKE_MODULE_PATH "${SRC}/cmake/emscripten")
endif()

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
		"The type of build to perform. Valid values are: RelWithDebInfo (default), Debug, Release, MinSizeRel" FORCE)
endif()

if(WINDOWS AND NOT BUILD_SHARED_LIBS)
	cmake_policy(SET CMP0091 NEW)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

include(CheckCXXSourceCompiles)
if(NOT (WIN32 OR EMSCRIPTEN))
	set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
	check_cxx_source_compiles("int main(int, char**) { return 0; }" have_fsanitize_address)
	if(have_fsanitize_address)
		string(APPEND CMAKE_CXX_FLAGS " -fsanitize=address")
	endif()
endif()

option(OPENC2E_USE_SERIALIZATION "Build the experimental serialization code" OFF)
option(OPENC2E_PROFILE_ALLOCATION "Collect allocation profile stats for DBG: SIZO" OFF)
if (OPENC2E_PROFILE_ALLOCATION)
	add_definitions("-DPROFILE_ALLOCATION_COUNT -DPROFILE_ALLOCATION_THREAD_SAFE")
endif()

# Find / include dependencies
find_package(Python REQUIRED)

find_program(RE2C_EXECUTABLE re2c)
if(NOT RE2C_EXECUTABLE)
	add_subdirectory(externals/re2c EXCLUDE_FROM_ALL)
	set(RE2C_EXECUTABLE re2c)
endif()

if(WIN32)
	add_subdirectory(externals/zlib EXCLUDE_FROM_ALL)
else()
	find_package(ZLIB REQUIRED)
endif()

if(WIN32 OR APPLE)
	add_subdirectory(externals/SDL2 EXCLUDE_FROM_ALL)
	add_subdirectory(externals/SDL2_mixer EXCLUDE_FROM_ALL)
	if(BUILD_SHARED_LIBS)
		add_library(SDL2::SDL2 ALIAS SDL2)
	else()
		add_library(SDL2::SDL2 ALIAS SDL2-static)
	endif()
	add_library(SDL2::Mixer ALIAS SDL2_mixer)
else()
	find_package(SDL2 REQUIRED)
	find_package(SDL2_mixer REQUIRED)
endif()

if (OPENC2E_USE_SERIALIZATION)
	find_package(Boost 1.34.0 COMPONENTS serialization REQUIRED)
endif()

add_subdirectory(externals/cxxopts EXCLUDE_FROM_ALL)
add_subdirectory(externals/fmt EXCLUDE_FROM_ALL)
add_subdirectory(externals/ghc_filesystem EXCLUDE_FROM_ALL)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # fix 'mismatch detected for RuntimeLibrary'
add_subdirectory(externals/googletest EXCLUDE_FROM_ALL)
add_subdirectory(externals/imgui EXCLUDE_FROM_ALL)
add_subdirectory(externals/nativefiledialog EXCLUDE_FROM_ALL)

# Pass flags to the compiler
add_compile_definitions("_REENTRANT" "YYERROR_VERBOSE")
if(WIN32)
	add_compile_definitions("NOMINMAX")
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "(Clang|GNU)")
	add_compile_options(-Wextra -Wall -Wno-conversion)
endif()
set(CMAKE_CXX_STANDARD 14)

# make sure assert() works
string(REPLACE "/DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
string(REPLACE "-DNDEBUG" "" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")

find_package(Threads REQUIRED) # stupid GCC needs explicit flags to use std::thread
link_libraries(Threads::Threads)

include_directories(BEFORE
	${GEN}
	${SRC}/src
)

# Targets
add_library(openc2e-common STATIC
	src/common/encoding.cpp
	src/common/mappedfile.cpp
	src/common/readfile.cpp
	src/common/Repr.cpp
	src/common/string_in.cpp
	src/common/userlocale.cpp
	src/common/case_insensitive_filesystem.cpp
)
target_link_libraries(openc2e-common PRIVATE fmt ghc_filesystem)

add_library(openc2e-fileformats STATIC
	src/fileformats/attFile.cpp
	src/fileformats/blkImage.cpp
	src/fileformats/bmpImage.cpp
	${GEN}/fileformats/caoslexer.cpp
	src/fileformats/caostoken.cpp
	${GEN}/fileformats/catalogueFile.cpp
	${GEN}/fileformats/cfgFile.cpp
	src/fileformats/charsetdta.cpp
	src/fileformats/c1cobfile.cpp
	src/fileformats/c1defaultpalette.cpp
	src/fileformats/c16Image.cpp
	src/fileformats/c2cobfile.cpp
	src/fileformats/genomeFile.cpp
	src/fileformats/hedfile.cpp
	src/fileformats/MfcReader.cpp
	src/fileformats/mngfile.cpp
	${GEN}/fileformats/mngparser.cpp
	src/fileformats/paletteFile.cpp
	src/fileformats/peFile.cpp
	src/fileformats/PrayFileReader.cpp
	src/fileformats/PrayFileWriter.cpp
	${GEN}/fileformats/PraySourceParser.cpp
	src/fileformats/s16Image.cpp
	src/fileformats/sprImage.cpp
	src/fileformats/strFile.cpp
	src/fileformats/verticalbarFile.cpp
	src/fileformats/ImageUtils.cpp
)
target_include_directories(openc2e-fileformats PRIVATE src/fileformats)
target_link_libraries(openc2e-fileformats
	PUBLIC fmt ghc_filesystem openc2e-common
	PRIVATE ZLIB::ZLIB
)

add_library(openc2e-mngmusic STATIC
	src/libmngmusic/MNGMusic.cpp
)
target_link_libraries(openc2e-mngmusic PRIVATE openc2e-fileformats)

add_library(openc2e-core STATIC
	src/openc2e-core/Agent.cpp
	src/openc2e-core/AgentHelpers.cpp
	src/openc2e-core/AgentRef.cpp
	src/openc2e-core/alloc_count.cpp
	src/openc2e-core/creatures/Biochemistry.cpp
	src/openc2e-core/Blackboard.cpp
	src/openc2e-core/Bubble.cpp
	src/openc2e-core/bytecode.cpp
	src/openc2e-core/creatures/c2eBrain.cpp
	src/openc2e-core/CallButton.cpp
	src/openc2e-core/Camera.cpp
	src/openc2e-core/caosException.cpp
	src/openc2e-core/caosparser.cpp
	src/openc2e-core/caosScript.cpp
	src/openc2e-core/caosValue.cpp
	src/openc2e-core/caos/caosVM_agent.cpp
	src/openc2e-core/caos/caosVM_camera.cpp
	src/openc2e-core/caos/caosVM_cdplayer.cpp
	src/openc2e-core/caos/caosVM_compound.cpp
	src/openc2e-core/caos/caosVM_core.cpp
	src/openc2e-core/caos/caosVM_creatures.cpp
	src/openc2e-core/caos/caosVM_debug.cpp
	src/openc2e-core/caos/caosVM_files.cpp
	src/openc2e-core/caos/caosVM_flow.cpp
	src/openc2e-core/caos/caosVM_genetics.cpp
	src/openc2e-core/caos/caosVM_history.cpp
	src/openc2e-core/caos/caosVM_input.cpp
	src/openc2e-core/caos/caosVM_map.cpp
	src/openc2e-core/caos/caosVM_motion.cpp
	src/openc2e-core/caos/caosVM_net.cpp
	src/openc2e-core/caosVM.cpp
	src/openc2e-core/caos/caosVM_ports.cpp
	src/openc2e-core/caos/caosVM_resources.cpp
	src/openc2e-core/caos/caosVM_scripts.cpp
	src/openc2e-core/caos/caosVM_sounds.cpp
	src/openc2e-core/caos/caosVM_time.cpp
	src/openc2e-core/caos/caosVM_variables.cpp
	src/openc2e-core/caos/caosVM_vectors.cpp
	src/openc2e-core/caos/caosVM_vehicles.cpp
	src/openc2e-core/caos/caosVM_world.cpp
	src/openc2e-core/Catalogue.cpp
	${GEN}/cmddata.cpp
	src/openc2e-core/CompoundAgent.cpp
	src/openc2e-core/creaturesImage.cpp
	src/openc2e-core/AnimatablePart.cpp
	src/openc2e-core/ButtonPart.cpp
	src/openc2e-core/CameraPart.cpp
	src/openc2e-core/ChemicalNamesManager.cpp
	src/openc2e-core/CobManager.cpp
	src/openc2e-core/CompoundPart.cpp
	src/openc2e-core/DullPart.cpp
	src/openc2e-core/FixedTextPart.cpp
	src/openc2e-core/GraphPart.cpp
	src/openc2e-core/SpritePart.cpp
	src/openc2e-core/TextPart.cpp
	src/openc2e-core/TextEntryPart.cpp
	src/openc2e-core/creatures/CompoundCreature.cpp
	src/openc2e-core/creatures/Creature.cpp
	src/openc2e-core/creatures/CreatureAgent.cpp
	src/openc2e-core/creatures/CreatureAI.cpp
	src/openc2e-core/dialect.cpp
	src/openc2e-core/Engine.cpp
	src/openc2e-core/historyManager.cpp
	src/openc2e-core/imageManager.cpp
	src/openc2e-core/Lift.cpp
	src/openc2e-core/Map.cpp
	src/openc2e-core/MetaRoom.cpp
	src/openc2e-core/MusicManager.cpp
	src/openc2e-core/net/socket.cpp
	src/openc2e-core/NetBackend.cpp
	src/openc2e-core/creatures/oldBrain.cpp
	src/openc2e-core/parseException.cpp
	src/openc2e-core/partzorder.cpp
	src/openc2e-core/PathResolver.cpp
	src/openc2e-core/physics.cpp
	src/openc2e-core/PointerAgent.cpp
	src/openc2e-core/Port.cpp
	src/openc2e-core/prayManager.cpp
	src/openc2e-core/renderable.cpp
	src/openc2e-core/renderablezorder.cpp
	src/openc2e-core/Room.cpp
	src/openc2e-core/Scriptorium.cpp
	src/openc2e-core/SFCFile.cpp
	src/openc2e-core/SimpleAgent.cpp
	src/openc2e-core/creatures/SkeletalCreature.cpp
	src/openc2e-core/Sound.cpp
	src/openc2e-core/SoundManager.cpp
	src/openc2e-core/Vehicle.cpp
	src/openc2e-core/VoiceData.cpp
	src/openc2e-core/World.cpp
)
target_include_directories(openc2e-core PRIVATE src/openc2e-core)
target_link_libraries(openc2e-core PUBLIC
	cxxopts
	fmt
	ghc_filesystem
	openc2e-common
	openc2e-fileformats
	openc2e-mngmusic
)
if(APPLE)
	target_link_libraries(openc2e-core PUBLIC "-framework CoreFoundation")
endif()
if (OPENC2E_USE_SERIALIZATION)
	target_sources(openc2e-core PRIVATE src/openc2e-core/caos/caosVM_ser_real.cpp)
	target_link_libraries(openc2e-core PUBLIC Boost::serialization)
else()
	target_sources(openc2e-core PRIVATE src/openc2e-core/caos/caosVM_ser_stub.cpp)
endif()

add_library(openc2e-audiobackend STATIC
	src/openc2e-audiobackend/SDLMixerBackend.cpp
)
target_compile_definitions(openc2e-audiobackend PUBLIC "SDLMIXER_SUPPORT")
target_link_libraries(openc2e-audiobackend PUBLIC openc2e-common SDL2::Mixer)

add_library(imgui_sdl src/imgui_sdl/imgui_sdl.cpp)
target_link_libraries(imgui_sdl PRIVATE imgui)

add_executable(openc2e
	src/openc2e-main/main.cpp
	src/openc2e-main/SDLBackend.cpp
	src/openc2e-main/openc2eimgui/AgentInjector.cpp
	src/openc2e-main/openc2eimgui/BrainViewer.cpp
	src/openc2e-main/openc2eimgui/C1ToolBar.cpp
	src/openc2e-main/openc2eimgui/C2StatusBar.cpp
	src/openc2e-main/openc2eimgui/C2ToolBar.cpp
	src/openc2e-main/openc2eimgui/CreatureGrapher.cpp
	src/openc2e-main/openc2eimgui/Hatchery.cpp
	src/openc2e-main/openc2eimgui/ImGuiUtils.cpp
	src/openc2e-main/openc2eimgui/MainMenu.cpp
	src/openc2e-main/openc2eimgui/Openc2eImGui.cpp
)
target_include_directories(openc2e PRIVATE src/openc2e-core)
target_link_libraries(openc2e
	openc2e-core
	openc2e-audiobackend
	imgui
	imgui_sdl
	SDL2::SDL2
	${CMAKE_DL_LIBS}
)

add_executable(opencreatures1
	src/opencreatures1/main.cpp
	src/opencreatures1/ImageManager.cpp
	src/opencreatures1/PathManager.cpp
	src/opencreatures1/SDLBackend.cpp
	src/opencreatures1/C1MusicManager.cpp
	src/openc2e-core/creaturesImage.cpp
)
target_link_libraries(opencreatures1
	openc2e-common
	openc2e-audiobackend
	openc2e-fileformats
	SDL2::SDL2
	fmt
	ghc_filesystem
)

add_executable(attparser src/tools/attparser.cpp)
target_link_libraries(attparser openc2e-fileformats fmt)

add_executable(caos_to_bytecode src/tools/caos_to_bytecode.cpp)
target_link_libraries(caos_to_bytecode openc2e-core)

add_executable(change_breed_slot src/tools/change_breed_slot.cpp)
target_link_libraries(change_breed_slot openc2e-fileformats)

add_executable(cobdumper src/tools/cobdumper.cpp)
target_link_libraries(cobdumper openc2e-core)

add_executable(creaturesarchivedumper src/tools/creaturesarchivedumper.cpp)
target_link_libraries(creaturesarchivedumper openc2e-common fmt ghc_filesystem z)

add_executable(mngdumper src/tools/mngdumper.cpp)
target_link_libraries(mngdumper fmt ghc_filesystem openc2e-fileformats)

add_executable(mngbuild src/tools/mngbuild.cpp)
target_link_libraries(mngbuild fmt ghc_filesystem openc2e-fileformats)

add_executable(mngplayer
	src/tools/mngplayer.cpp
)
target_link_libraries(mngplayer openc2e-audiobackend openc2e-mngmusic openc2e-fileformats fmt ghc_filesystem)

add_executable(mngplayer2
    src/mngplayer2/mngplayer2.cpp
    src/mngplayer2/mngplayer2.rc
)
target_link_libraries(mngplayer2
	SDL2::SDL2
	fmt
	ghc_filesystem
	imgui
	imgui_sdl
	nativefiledialog
	openc2e-audiobackend
	openc2e-fileformats
	openc2e-mngmusic
	${CMAKE_DL_LIBS}
)
if(WIN32)
	target_link_libraries(mngplayer2 SDL2main)
	set_target_properties(mngplayer2 PROPERTIES WIN32_EXECUTABLE TRUE OUTPUT_NAME MNGPlayer2)
endif()

add_executable(pefiledumper src/tools/pefiledumper.cpp)
target_link_libraries(pefiledumper openc2e-fileformats)

add_executable(praydumper src/tools/praydumper.cpp)
target_link_libraries(praydumper openc2e-fileformats)

add_executable(praybuilder src/tools/praybuilder.cpp)
target_link_libraries(praybuilder openc2e-fileformats)

add_executable(sfcdumper src/tools/sfcdumper.cpp)
target_link_libraries(sfcdumper openc2e-fileformats fmt ghc_filesystem)

add_executable(spritedumper src/tools/spritedumper.cpp)
target_link_libraries(spritedumper openc2e-fileformats fmt ghc_filesystem)

add_executable(translate_voice
	src/tools/translate_voice.cpp
)
target_link_libraries(translate_voice openc2e-fileformats openc2e-common openc2e-core openc2e-audiobackend fmt ghc_filesystem)

# tests
enable_testing()
include(GoogleTest)
include(Openc2eOutputOnTestFailure)

add_executable(test_openc2e
	src/tests/test_caos.cpp
	src/tests/test_encoding.cpp
	src/tests/test_pray.cpp
	src/tests/test_common.cpp
	src/tests/test_sparseset.cpp
	src/tests/test_entitycontainer.cpp
)
target_link_libraries(test_openc2e openc2e-core gtest gtest_main)
if(COMMAND gtest_discover_testsp)
	gtest_discover_tests(test_openc2e)
else()
    gtest_add_tests(test_openc2e "" "")
endif()

# Generated files
file(GLOB CAOSVM_FILES CMAKE_CONFIGURE_DEPENDS ${SRC}/src/openc2e-core/caos/caosVM_*.cpp)
add_custom_command(
	OUTPUT ${GEN}/commandinfo.json
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}"
	COMMAND ${Python_EXECUTABLE} scripts/parsedocs.py ${CAOSVM_FILES} > ${GEN}/commandinfo.json
	DEPENDS scripts/parsedocs.py ${CAOSVM_FILES}
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${BIN}/caosdocs.html
	COMMAND ${Python_EXECUTABLE} scripts/writehtml.py ${GEN}/commandinfo.json > ${BIN}/caosdocs.html
	DEPENDS scripts/writehtml.py scripts/docs.css ${GEN}/commandinfo.json
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/cmddata.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}"
	COMMAND ${Python_EXECUTABLE} scripts/writecmds.py ${GEN}/commandinfo.json > ${GEN}/cmddata.cpp
	DEPENDS scripts/writecmds.py ${GEN}/commandinfo.json
	WORKING_DIRECTORY ${SRC})

# it seems these add_custom_command for re2c could be made
# into a macro... e.g. RE2C(file1) RE2C(file2) etc...
add_custom_command(
	OUTPUT ${GEN}/cataloglexer.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/cataloglexer.cpp ${SRC}/src/cataloglexer.re2c.cpp
	DEPENDS ${SRC}/src/cataloglexer.re2c.cpp
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/fileformats/caoslexer.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}/fileformats"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/fileformats/caoslexer.cpp ${SRC}/src/fileformats/caoslexer.re2c.cpp
	DEPENDS ${SRC}/src/fileformats/caoslexer.re2c.cpp
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/fileformats/catalogueFile.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}/fileformats"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/fileformats/catalogueFile.cpp ${SRC}/src/fileformats/catalogueFile.re2c.cpp
	DEPENDS ${SRC}/src/fileformats/catalogueFile.re2c.cpp
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/fileformats/cfgFile.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}/fileformats"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/fileformats/cfgFile.cpp ${SRC}/src/fileformats/cfgFile.re2c.cpp
	DEPENDS ${SRC}/src/fileformats/cfgFile.re2c.cpp
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/fileformats/mngparser.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}/fileformats"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/fileformats/mngparser.cpp ${SRC}/src/fileformats/mngparser.re2c.cpp
	DEPENDS ${SRC}/src/fileformats/mngparser.re2c.cpp
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/fileformats/PraySourceParser.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}/fileformats"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/fileformats/PraySourceParser.cpp ${SRC}/src/fileformats/PraySourceParser.re2c.cpp
	DEPENDS ${SRC}/src/fileformats/PraySourceParser.re2c.cpp
	WORKING_DIRECTORY ${SRC})

add_custom_target(docs ALL DEPENDS ${BIN}/caosdocs.html)
add_custom_target(generated DEPENDS
	${GEN}/caoslexer.cpp
	${GEN}/mngparser.cpp
	${GEN}/cmddata.cpp
)

install(FILES
	AUTHORS
	COPYING
	README.md
	${BIN}/caosdocs.html
	${GEN}/commandinfo.json
	DESTINATION "."
)
install(TARGETS
	openc2e
	cobdumper
	mngdumper
	praybuilder
	praydumper
	spritedumper
	DESTINATION "."
)

install(TARGETS
	mngplayer2
	DESTINATION "."
	COMPONENT mngplayer2
)

if(WIN32)
	set(CPACK_GENERATOR ZIP)
else()
	set(CPACK_GENERATOR TGZ)
endif()
include(Openc2eReadGitRev)
set(CPACK_PACKAGE_VERSION "${git_short_rev}")
include(CPack)
