cmake_minimum_required(VERSION 3.3)
project(openc2e)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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
	find_package(SDL2 2.0.18 REQUIRED)
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
add_library(openc2e-common OBJECT
	src/common/audio/AudioBackend.cpp
	src/common/audio/NullAudioBackend.cpp
	src/common/backend/Backend.cpp
	src/common/backend/NullBackend.cpp
	src/common/render/RenderItemHandle.cpp
	src/common/render/RenderSystem.cpp
	src/common/case_insensitive_filesystem.cpp
	src/common/creaturesImage.cpp
	src/common/encoding.cpp
	src/common/mappedfile.cpp
	src/common/readfile.cpp
	src/common/Random.cpp
	src/common/userlocale.cpp
	src/common/WindowsRegistry.cpp
)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(openc2e-common PRIVATE -Wconversion -Wsign-conversion -Wsign-compare)
endif()
target_link_libraries(openc2e-common
	PUBLIC fmt
	PRIVATE ghc_filesystem "$<$<PLATFORM_ID:Darwin>:-framework Foundation>"
)

add_executable(test_common
	src/common/tests/DemangleTest.cpp
	src/common/tests/EncodingTest.cpp
	src/common/tests/EndsWithTest.cpp
	src/common/tests/NumericCastTest.cpp
	src/common/tests/SlotMapTest.cpp
	src/common/tests/WildcardTest.cpp
)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(test_common PRIVATE -Wconversion -Wsign-conversion -Wsign-compare)
endif()
target_link_libraries(test_common openc2e-common gtest_main)

add_library(openc2e-fileformats STATIC
	src/fileformats/attFile.cpp
	src/fileformats/blkImage.cpp
	src/fileformats/bmpImage.cpp
	src/fileformats/caoslexer.cpp
	src/fileformats/caostoken.cpp
	src/fileformats/catalogueFile.cpp
	src/fileformats/cfgFile.cpp
	src/fileformats/charsetdta.cpp
	src/fileformats/c1cobfile.cpp
	src/fileformats/c1defaultpalette.cpp
	src/fileformats/c16Image.cpp
	src/fileformats/c2cobfile.cpp
	src/fileformats/genomeFile.cpp
	src/fileformats/hedfile.cpp
	src/fileformats/MFCReader.cpp
	src/fileformats/MFCWriter.cpp
	src/fileformats/mngfile.cpp
	src/fileformats/mngparser.cpp
	src/fileformats/NewSFCFile.cpp
	src/fileformats/paletteFile.cpp
	src/fileformats/peFile.cpp
	src/fileformats/pngImage.cpp
	src/fileformats/PrayFileReader.cpp
	src/fileformats/PrayFileWriter.cpp
	src/fileformats/PraySourceParser.cpp
	src/fileformats/s16Image.cpp
	src/fileformats/sprImage.cpp
	src/fileformats/strFile.cpp
	src/fileformats/verticalbarFile.cpp
	src/fileformats/ImageUtils.cpp
)
target_include_directories(openc2e-fileformats PRIVATE src/fileformats)
target_link_libraries(openc2e-fileformats
	PUBLIC ghc_filesystem openc2e-common
	PRIVATE ZLIB::ZLIB
)

add_executable(test_fileformats
	src/fileformats/tests/CaosLexerTest.cpp
	src/fileformats/tests/CatalogueFileTest.cpp
	src/fileformats/tests/CfgFileTest.cpp
	src/fileformats/tests/MngTest.cpp
	src/fileformats/tests/PrayTest.cpp
)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(test_fileformats PRIVATE -Wconversion -Wsign-conversion -Wsign-compare)
endif()
target_link_libraries(test_fileformats openc2e-fileformats gtest_main)

add_library(openc2e-mngmusic STATIC
	src/libmngmusic/MNGMusic.cpp
)
target_link_libraries(openc2e-mngmusic PRIVATE openc2e-fileformats)

add_library(openc2e_sdlbackend OBJECT
	src/sdlbackend/SDLBackend.cpp
	src/sdlbackend/SDLMixerBackend.cpp
)
target_link_libraries(openc2e_sdlbackend PUBLIC openc2e-common imgui SDL2::SDL2 SDL2::Mixer ${CMAKE_DL_LIBS})

add_library(openc2e_core OBJECT
	src/openc2e/Agent.cpp
	src/openc2e/AgentHelpers.cpp
	src/openc2e/AgentRef.cpp
	src/openc2e/alloc_count.cpp
	src/openc2e/creatures/Biochemistry.cpp
	src/openc2e/Blackboard.cpp
	src/openc2e/Bubble.cpp
	src/openc2e/bytecode.cpp
	src/openc2e/creatures/c2eBrain.cpp
	src/openc2e/CallButton.cpp
	src/openc2e/Camera.cpp
	src/openc2e/caosException.cpp
	src/openc2e/caosparser.cpp
	src/openc2e/caosScript.cpp
	src/openc2e/caosValue.cpp
	src/openc2e/caos/caosVM_agent.cpp
	src/openc2e/caos/caosVM_camera.cpp
	src/openc2e/caos/caosVM_cdplayer.cpp
	src/openc2e/caos/caosVM_compound.cpp
	src/openc2e/caos/caosVM_core.cpp
	src/openc2e/caos/caosVM_creatures.cpp
	src/openc2e/caos/caosVM_debug.cpp
	src/openc2e/caos/caosVM_files.cpp
	src/openc2e/caos/caosVM_flow.cpp
	src/openc2e/caos/caosVM_genetics.cpp
	src/openc2e/caos/caosVM_history.cpp
	src/openc2e/caos/caosVM_input.cpp
	src/openc2e/caos/caosVM_map.cpp
	src/openc2e/caos/caosVM_motion.cpp
	src/openc2e/caos/caosVM_net.cpp
	src/openc2e/caosVM.cpp
	src/openc2e/caos/caosVM_ports.cpp
	src/openc2e/caos/caosVM_resources.cpp
	src/openc2e/caos/caosVM_scripts.cpp
	src/openc2e/caos/caosVM_sounds.cpp
	src/openc2e/caos/caosVM_time.cpp
	src/openc2e/caos/caosVM_variables.cpp
	src/openc2e/caos/caosVM_vectors.cpp
	src/openc2e/caos/caosVM_vehicles.cpp
	src/openc2e/caos/caosVM_world.cpp
	src/openc2e/Catalogue.cpp
	${GEN}/cmddata.cpp
	src/openc2e/CompoundAgent.cpp
	src/openc2e/AnimatablePart.cpp
	src/openc2e/ButtonPart.cpp
	src/openc2e/CameraPart.cpp
	src/openc2e/ChemicalNamesManager.cpp
	src/openc2e/CobManager.cpp
	src/openc2e/CompoundPart.cpp
	src/openc2e/DullPart.cpp
	src/openc2e/FixedTextPart.cpp
	src/openc2e/GraphPart.cpp
	src/openc2e/SpritePart.cpp
	src/openc2e/TextPart.cpp
	src/openc2e/TextEntryPart.cpp
	src/openc2e/creatures/CompoundCreature.cpp
	src/openc2e/creatures/Creature.cpp
	src/openc2e/creatures/CreatureAgent.cpp
	src/openc2e/creatures/CreatureAI.cpp
	src/openc2e/dialect.cpp
	src/openc2e/Engine.cpp
	src/openc2e/historyManager.cpp
	src/openc2e/imageManager.cpp
	src/openc2e/Lift.cpp
	src/openc2e/Map.cpp
	src/openc2e/MetaRoom.cpp
	src/openc2e/MusicManager.cpp
	src/openc2e/net/socket.cpp
	src/openc2e/NetBackend.cpp
	src/openc2e/creatures/oldBrain.cpp
	src/openc2e/parseException.cpp
	src/openc2e/partzorder.cpp
	src/openc2e/PathResolver.cpp
	src/openc2e/physics.cpp
	src/openc2e/PointerAgent.cpp
	src/openc2e/Port.cpp
	src/openc2e/prayManager.cpp
	src/openc2e/renderable.cpp
	src/openc2e/renderablezorder.cpp
	src/openc2e/Room.cpp
	src/openc2e/Scriptorium.cpp
	src/openc2e/SFCFile.cpp
	src/openc2e/SimpleAgent.cpp
	src/openc2e/creatures/SkeletalCreature.cpp
	src/openc2e/Sound.cpp
	src/openc2e/SoundManager.cpp
	src/openc2e/Vehicle.cpp
	src/openc2e/VoiceData.cpp
	src/openc2e/World.cpp
	src/openc2e/openc2eimgui/AgentInjector.cpp
	src/openc2e/openc2eimgui/BrainViewer.cpp
	src/openc2e/openc2eimgui/C1ToolBar.cpp
	src/openc2e/openc2eimgui/C2StatusBar.cpp
	src/openc2e/openc2eimgui/C2ToolBar.cpp
	src/openc2e/openc2eimgui/CreatureGrapher.cpp
	src/openc2e/openc2eimgui/Hatchery.cpp
	src/openc2e/openc2eimgui/ImGuiUtils.cpp
	src/openc2e/openc2eimgui/MainMenu.cpp
	src/openc2e/openc2eimgui/Openc2eImGui.cpp
)
target_include_directories(openc2e_core PRIVATE src/openc2e)
target_link_libraries(openc2e_core PUBLIC
	cxxopts
	ghc_filesystem
	imgui
	openc2e-common
	openc2e-fileformats
	openc2e-mngmusic
)
if (OPENC2E_USE_SERIALIZATION)
	target_sources(openc2e_core PRIVATE src/openc2e/caos/caosVM_ser_real.cpp)
	target_link_libraries(openc2e_core PUBLIC Boost::serialization)
else()
	target_sources(openc2e_core PRIVATE src/openc2e/caos/caosVM_ser_stub.cpp)
endif()

add_executable(openc2e src/openc2e/main.cpp)
target_include_directories(openc2e PRIVATE src/openc2e)
target_link_libraries(openc2e openc2e_core openc2e_sdlbackend)

add_executable(test_openc2e
	src/openc2e/tests/CaosTest.cpp
)
target_link_libraries(test_openc2e openc2e_core gtest_main)

add_library(opencreatures1_core STATIC
	src/opencreatures1/Biochemistry.cpp
	src/opencreatures1/C1MusicManager.cpp
	src/opencreatures1/EngineContext.cpp
	src/opencreatures1/ImageManager.cpp
	src/opencreatures1/MacroCommands.cpp
	src/opencreatures1/MacroContext.cpp
	src/opencreatures1/MacroManager.cpp
	src/opencreatures1/MapManager.cpp
	src/opencreatures1/MessageManager.cpp
	src/opencreatures1/PathManager.cpp
	src/opencreatures1/PointerManager.cpp
	src/opencreatures1/Renderable.cpp
	src/opencreatures1/SFCSerialization.cpp
	src/opencreatures1/ViewportManager.cpp
	src/opencreatures1/C1ControlledSound.cpp
	src/opencreatures1/C1SoundManager.cpp
	src/opencreatures1/objects/Blackboard.cpp
	src/opencreatures1/objects/Object.cpp
	src/opencreatures1/objects/ObjectNames.cpp
)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(opencreatures1_core PRIVATE -Wconversion -Wsign-conversion -Wsign-compare)
endif()
target_include_directories(opencreatures1_core PUBLIC src/opencreatures1)
target_link_libraries(opencreatures1_core PUBLIC
	openc2e-common
	openc2e-fileformats
	SDL2::SDL2
	ghc_filesystem
)

add_executable(opencreatures1
	src/opencreatures1/main.cpp
)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(opencreatures1 PRIVATE -Wconversion -Wsign-conversion -Wsign-compare)
endif()
target_link_libraries(opencreatures1 opencreatures1_core openc2e_sdlbackend)

add_executable(test_opencreatures1
	src/opencreatures1/tests/BiochemistryTest.cpp
	src/opencreatures1/tests/MacroTest.cpp
	src/opencreatures1/tests/ScriptoriumTest.cpp
)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	target_compile_options(test_opencreatures1 PRIVATE -Wconversion -Wsign-conversion -Wsign-compare)
endif()
target_link_libraries(test_opencreatures1 opencreatures1_core gtest_main)

add_executable(attparser src/tools/attparser.cpp)
target_link_libraries(attparser openc2e-fileformats)

add_executable(caos_to_bytecode src/tools/caos_to_bytecode.cpp)
target_link_libraries(caos_to_bytecode openc2e_core)

add_executable(change_breed_slot src/tools/change_breed_slot.cpp)
target_link_libraries(change_breed_slot openc2e-fileformats)

add_executable(cobdumper src/tools/cobdumper.cpp)
target_link_libraries(cobdumper openc2e_core)

add_executable(creaturesarchivedumper src/tools/creaturesarchivedumper.cpp)
target_link_libraries(creaturesarchivedumper openc2e-common ghc_filesystem z)

add_executable(expdumper src/tools/expdumper.cpp)
target_link_libraries(expdumper openc2e-fileformats ghc_filesystem)

add_executable(mngdumper src/tools/mngdumper.cpp)
target_link_libraries(mngdumper ghc_filesystem openc2e-fileformats)

add_executable(mngbuild src/tools/mngbuild.cpp)
target_link_libraries(mngbuild ghc_filesystem openc2e-fileformats)

add_executable(mngplayer
	src/tools/mngplayer.cpp
)
target_link_libraries(mngplayer openc2e_sdlbackend openc2e-mngmusic openc2e-fileformats ghc_filesystem)

add_executable(mngplayer2
    src/mngplayer2/mngplayer2.cpp
    src/mngplayer2/mngplayer2.rc
)
target_link_libraries(mngplayer2
	ghc_filesystem
	nativefiledialog
	openc2e-mngmusic
	openc2e_sdlbackend
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
target_link_libraries(sfcdumper openc2e-fileformats ghc_filesystem)

add_executable(spritedumper src/tools/spritedumper.cpp)
target_link_libraries(spritedumper openc2e-fileformats ghc_filesystem)

add_executable(translate_voice
	src/tools/translate_voice.cpp
)
target_link_libraries(translate_voice openc2e-common openc2e_core openc2e_sdlbackend ghc_filesystem)

# tests
enable_testing()
include(GoogleTest)
include(Openc2eOutputOnTestFailure)

gtest_discover_tests(test_openc2e)
gtest_discover_tests(test_opencreatures1)
gtest_discover_tests(test_common)
gtest_discover_tests(test_fileformats)

# Generated files
file(GLOB CAOSVM_FILES CMAKE_CONFIGURE_DEPENDS ${SRC}/src/openc2e/caos/caosVM_*.cpp)
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

add_custom_target(docs ALL DEPENDS ${BIN}/caosdocs.html)
add_custom_target(generated DEPENDS
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
