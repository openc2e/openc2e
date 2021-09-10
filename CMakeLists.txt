cmake_minimum_required(VERSION 3.0)
project(openc2e)

set(SRC "${CMAKE_CURRENT_SOURCE_DIR}")
set(BIN "${CMAKE_CURRENT_BINARY_DIR}")
set(GEN "${BIN}/generated")

set(CMAKE_MODULE_PATH "${SRC}/cmake")

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
		"The type of build to perform. Valid values are: RelWithDebInfo (default), Debug, Release, MinSizeRel" FORCE)
endif()

include(CheckCXXSourceCompiles)
if(NOT WIN32)
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
	add_subdirectory(externals/libpng EXCLUDE_FROM_ALL)

	add_subdirectory(externals/SDL2 EXCLUDE_FROM_ALL)
	add_subdirectory(externals/SDL2_mixer EXCLUDE_FROM_ALL)
	add_library(SDL2::SDL2 ALIAS SDL2)
	add_library(SDL2::Mixer ALIAS SDL2_mixer)
else()
	find_package(PNG REQUIRED)

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
add_subdirectory(externals/imgui_sdl EXCLUDE_FROM_ALL)
add_subdirectory(externals/mpark-variant EXCLUDE_FROM_ALL)


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
add_library(openc2e-utils STATIC
	src/utils/encoding.cpp
	src/utils/mmapifstream.cpp
	src/utils/readfile.cpp
	src/utils/string_in.cpp
	src/utils/userlocale.cpp
	src/utils/case_insensitive_filesystem.cpp
)
target_link_libraries(openc2e-utils PRIVATE ghc_filesystem)

add_library(openc2e-utils-socket STATIC
	src/utils/socket.cpp # separate because it links to Ws2_32.lib
)

add_library(openc2e-fileformats STATIC
	src/fileformats/attFile.cpp
	src/fileformats/blkImage.cpp
	src/fileformats/bmpImage.cpp
	${GEN}/fileformats/Caos2PrayParser.cpp
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
	src/fileformats/mngfile.cpp
	${GEN}/fileformats/mngparser.cpp
	src/fileformats/paletteFile.cpp
	src/fileformats/peFile.cpp
	src/fileformats/pngImage.cpp
	src/fileformats/PrayFileReader.cpp
	src/fileformats/PrayFileWriter.cpp
	${GEN}/fileformats/PraySourceParser.cpp
	src/fileformats/s16Image.cpp
	src/fileformats/sprImage.cpp
	src/fileformats/strFile.cpp
	src/fileformats/verticalbarFile.cpp
	src/fileformats/ImageUtils.cpp
)
target_link_libraries(openc2e-fileformats
	PUBLIC fmt ghc_filesystem mpark_variant openc2e-utils
	PRIVATE ZLIB::ZLIB PNG::PNG
)

add_library(openc2e-core STATIC
	src/Agent.cpp
	src/AgentHelpers.cpp
	src/AgentRef.cpp
	src/alloc_count.cpp
	src/creatures/Biochemistry.cpp
	src/Blackboard.cpp
	src/Bubble.cpp
	src/bytecode.cpp
	src/creatures/c2eBrain.cpp
	src/CallButton.cpp
	src/Camera.cpp
	src/caosException.cpp
	src/caosparser.cpp
	src/caosScript.cpp
	src/caosValue.cpp
	src/caos/caosVM_agent.cpp
	src/caos/caosVM_camera.cpp
	src/caos/caosVM_cdplayer.cpp
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
	src/Catalogue.cpp
	${GEN}/cmddata.cpp
	src/CompoundAgent.cpp
	src/creaturesImage.cpp
	src/AnimatablePart.cpp
	src/ButtonPart.cpp
	src/CameraPart.cpp
	src/ChemicalNamesManager.cpp
	src/CobManager.cpp
	src/CompoundPart.cpp
	src/DullPart.cpp
	src/FixedTextPart.cpp
	src/GraphPart.cpp
	src/SpritePart.cpp
	src/TextPart.cpp
	src/TextEntryPart.cpp
	src/creatures/CompoundCreature.cpp
	src/creatures/Creature.cpp
	src/creatures/CreatureAgent.cpp
	src/creatures/CreatureAI.cpp
	src/dialect.cpp
	src/Engine.cpp
	src/historyManager.cpp
	src/imageManager.cpp
	src/Lift.cpp
	src/Map.cpp
	src/MetaRoom.cpp
	src/MNGMusic.cpp
	src/MusicManager.cpp
	src/NetBackend.cpp
	src/creatures/oldBrain.cpp
	src/parseException.cpp
	src/partzorder.cpp
	src/PathResolver.cpp
	src/physics.cpp
	src/PointerAgent.cpp
	src/Port.cpp
	src/prayManager.cpp
	src/renderable.cpp
	src/renderablezorder.cpp
	src/Room.cpp
	src/Scriptorium.cpp
	src/SFCFile.cpp
	src/SimpleAgent.cpp
	src/creatures/SkeletalCreature.cpp
	src/Sound.cpp
	src/SoundManager.cpp
	src/Vehicle.cpp
	src/VoiceData.cpp
	src/World.cpp
)
target_link_libraries(openc2e-core PUBLIC
	cxxopts
	fmt
	ghc_filesystem
	mpark_variant
	openc2e-utils
	openc2e-utils-socket
	openc2e-fileformats
)
if(APPLE)
	target_link_libraries(openc2e-core PUBLIC "-framework CoreFoundation")
endif()
if (OPENC2E_USE_SERIALIZATION)
	target_sources(openc2e-core PRIVATE src/caos/caosVM_ser_real.cpp)
	target_link_libraries(openc2e-core PUBLIC Boost::serialization)
else()
	target_sources(openc2e-core PRIVATE src/caos/caosVM_ser_stub.cpp)
endif()

add_library(openc2e-audiobackend STATIC
	src/audiobackend/SDLMixerBackend.cpp
)
target_compile_definitions(openc2e-audiobackend PUBLIC "SDLMIXER_SUPPORT")
target_link_libraries(openc2e-audiobackend PUBLIC SDL2::Mixer)

add_executable(openc2e
	src/main.cpp
	src/backends/SDLBackend.cpp
	src/backends/openc2eimgui/AgentInjector.cpp
	src/backends/openc2eimgui/BrainViewer.cpp
	src/backends/openc2eimgui/C1ToolBar.cpp
	src/backends/openc2eimgui/C2StatusBar.cpp
	src/backends/openc2eimgui/C2ToolBar.cpp
	src/backends/openc2eimgui/CreatureGrapher.cpp
	src/backends/openc2eimgui/Hatchery.cpp
	src/backends/openc2eimgui/ImGuiUtils.cpp
	src/backends/openc2eimgui/MainMenu.cpp
	src/backends/openc2eimgui/Openc2eImGui.cpp
)
target_link_libraries(openc2e
	openc2e-core
	openc2e-audiobackend
	imgui
	imgui_sdl
	SDL2::SDL2
	${CMAKE_DL_LIBS}
)

add_executable(change_breed_slot src/tools/change_breed_slot.cpp)
target_link_libraries(change_breed_slot openc2e-fileformats)

add_executable(cobdumper src/tools/cobdumper.cpp)
target_link_libraries(cobdumper openc2e-core)

add_executable(creaturesarchivedumper src/tools/creaturesarchivedumper.cpp)
target_link_libraries(creaturesarchivedumper fmt ghc_filesystem z)

add_executable(mngdumper src/tools/mngdumper.cpp)
target_link_libraries(mngdumper fmt ghc_filesystem openc2e-fileformats)

add_executable(mngbuild src/tools/mngbuild.cpp)
target_link_libraries(mngbuild fmt ghc_filesystem openc2e-fileformats)

add_executable(mngplayer
	src/MNGMusic.cpp
	src/tools/mngplayer.cpp
)
target_link_libraries(mngplayer openc2e-audiobackend openc2e-fileformats fmt ghc_filesystem)

add_executable(pefiledumper src/tools/pefiledumper.cpp)
target_link_libraries(pefiledumper openc2e-fileformats)

add_executable(praydumper src/tools/praydumper.cpp)
target_link_libraries(praydumper openc2e-fileformats)

add_executable(praybuilder src/tools/praybuilder.cpp)
target_link_libraries(praybuilder openc2e-fileformats)

add_executable(spritedumper src/tools/spritedumper.cpp)
target_link_libraries(spritedumper openc2e-fileformats fmt ghc_filesystem)

add_executable(translate_voice
	src/tools/translate_voice.cpp
	src/VoiceData.cpp
	src/Catalogue.cpp
)
target_link_libraries(translate_voice openc2e-fileformats openc2e-utils openc2e-audiobackend fmt ghc_filesystem)

# tests
enable_testing()
include(GoogleTest)
include(Openc2eOutputOnTestFailure)

add_executable(test_openc2e
	tests/test_caos.cpp
	tests/test_encoding.cpp
	tests/test_pray.cpp
        tests/test_utils.cpp
)
target_link_libraries(test_openc2e openc2e-core gtest gtest_main)
if(COMMAND gtest_discover_testsp)
	gtest_discover_tests(test_openc2e)
else()
    gtest_add_tests(test_openc2e "" "")
endif()

# Generated files
file(GLOB CAOSVM_FILES CMAKE_CONFIGURE_DEPENDS ${SRC}/src/caos/caosVM_*.cpp)
add_custom_command(
	OUTPUT ${GEN}/commandinfo.json
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}"
	COMMAND ${Python_EXECUTABLE} parsedocs.py ${CAOSVM_FILES} > ${GEN}/commandinfo.json
	DEPENDS parsedocs.py ${CAOSVM_FILES}
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${BIN}/caosdocs.html
	COMMAND ${Python_EXECUTABLE} docs/writehtml.py ${GEN}/commandinfo.json > ${BIN}/caosdocs.html
	DEPENDS docs/writehtml.py ${GEN}/commandinfo.json
	WORKING_DIRECTORY ${SRC})
add_custom_command(
	OUTPUT ${BIN}/openc2e.6
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC}/docs/openc2e.6 ${BIN}/openc2e.6
	DEPENDS docs/openc2e.6
	)
# it seems these add_custom_command for re2c could be made
# into a macro... e.g. RE2C(file1) RE2C(file2) etc...
add_custom_command(
	OUTPUT ${GEN}/cataloglexer.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/cataloglexer.cpp ${SRC}/src/cataloglexer.re2c.cpp
	DEPENDS ${SRC}/src/cataloglexer.re2c.cpp
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/cmddata.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}"
	COMMAND ${Python_EXECUTABLE} writecmds.py ${GEN}/commandinfo.json > ${GEN}/cmddata.cpp
	DEPENDS ${GEN}/commandinfo.json writecmds.py
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
	OUTPUT ${GEN}/fileformats/Caos2PrayParser.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}/fileformats"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/fileformats/Caos2PrayParser.cpp ${SRC}/src/fileformats/Caos2PrayParser.re2c.cpp
	DEPENDS ${SRC}/src/fileformats/Caos2PrayParser.re2c.cpp
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/fileformats/PraySourceParser.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}/fileformats"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/fileformats/PraySourceParser.cpp ${SRC}/src/fileformats/PraySourceParser.re2c.cpp
	DEPENDS ${SRC}/src/fileformats/PraySourceParser.re2c.cpp
	WORKING_DIRECTORY ${SRC})

add_custom_target(docs ALL DEPENDS ${BIN}/caosdocs.html ${BIN}/openc2e.6)
add_custom_target(generated DEPENDS
	${GEN}/caoslexer.cpp
	${GEN}/mngparser.cpp
	${GEN}/cmddata.cpp
)

install(FILES
	AUTHORS
	COPYING
	README.md
	docs/openc2e.6
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
if(TARGET SDL2)
	install(TARGETS SDL2 DESTINATION ".")
endif()
install(SCRIPT "cmake/Openc2eBundle.cmake")
if(APPLE)
	install(CODE "message(STATUS \"Writing: fix_unidentified_developer.sh\")
	              file(WRITE \"\${CMAKE_INSTALL_PREFIX}/fix_unidentified_developer.sh\" \"sudo xattr -r -d com.apple.quarantine .\")")
endif()
if(WIN32)
	include(Openc2eDetectTargetArchitecture)
	if(TARGETARCH_X64)
		install(CODE "message(STATUS \"Downloading: vc_redist.x64.exe\")
		              file(DOWNLOAD \"https://aka.ms/vs/16/release/vc_redist.x64.exe\" \"\${CMAKE_INSTALL_PREFIX}/vc_redist.x64.exe\")")
	endif()
	if(TARGETARCH_ARM64)
		install(CODE "message(STATUS \"Downloading: vc_redist.arm64.exe\")
		              file(DOWNLOAD \"https://aka.ms/vs/16/release/vc_redist.arm64.exe\" \"\${CMAKE_INSTALL_PREFIX}/vc_redist.arm64.exe\")")
	endif()
endif()

if(WIN32)
	set(CPACK_GENERATOR ZIP)
else()
	set(CPACK_GENERATOR TXZ)
endif()
include(Openc2eReadGitRev)
set(CPACK_PACKAGE_VERSION "${git_short_rev}")
include(CPack)

# Creating folder for distribution
include(Openc2eReadGitRev)
set(dist_name "openc2e-${git_short_rev}-${CMAKE_SYSTEM_NAME}")
set(dist_dir "${CMAKE_CURRENT_BINARY_DIR}/${dist_name}")
set(dist_args)
list(APPEND dist_args
	COMMAND ${CMAKE_COMMAND} -E make_directory "${dist_dir}/"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/AUTHORS"
		"${CMAKE_CURRENT_SOURCE_DIR}/COPYING"
		"${CMAKE_CURRENT_SOURCE_DIR}/README.md"
		"${CMAKE_CURRENT_SOURCE_DIR}/docs/openc2e.6"
		"${BIN}/caosdocs.html"
		"${GEN}/commandinfo.json"
		"$<TARGET_FILE:openc2e>"
		"$<TARGET_FILE:cobdumper>"
		"$<TARGET_FILE:mngdumper>"
		"$<TARGET_FILE:mngbuild>"
		"$<TARGET_FILE:praybuilder>"
		"$<TARGET_FILE:praydumper>"
		"$<TARGET_FILE:spritedumper>"
		"${dist_dir}/"
	DEPENDS "${BIN}/caosdocs.html"
	COMMAND ${CMAKE_COMMAND}
		-D "DIST_DIR=${dist_dir}"
		-D "EXTRA_LIB_DIRS=$<TARGET_FILE_DIR:openc2e>" # MSVC puts built binaries and DLLs under $BUILD_TYPE
		-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Openc2eBundle.cmake
)
if(APPLE)
	list(APPEND dist_args
		COMMAND echo "sudo xattr -r -d com.apple.quarantine ." > "${dist_dir}/fix_unidentified_developer.sh"
		COMMAND chmod +x "${dist_dir}/fix_unidentified_developer.sh"
	)
endif()
if(WIN32 OR TRUE)
	include(Openc2eDetectTargetArchitecture)
	if(TARGETARCH_X64)
		file(WRITE "${BIN}/Openc2eDownloadVCRedistX64.cmake" "file(DOWNLOAD \"https://aka.ms/vs/16/release/vc_redist.x64.exe\" \"${dist_dir}/vc_redist.x64.exe\")")
		list(APPEND dist_args COMMAND cmake -P "${BIN}/Openc2eDownloadVCRedistX64.cmake")
	endif()
	if(TARGETARCH_ARM64)
		file(WRITE "${BIN}/Openc2eDownloadVCRedistArm64.cmake" "file(DOWNLOAD \"https://aka.ms/vs/16/release/vc_redist.arm64.exe\" \"${dist_dir}/vc_redist.arm64.exe\")")
		list(APPEND dist_args COMMAND cmake -P "${BIN}/Openc2eDownloadVCRedistARM64.cmake")
	endif()
endif()
if(DEFINED ENV{GITHUB_ENV})
	list(APPEND dist_args
		COMMAND echo "DIST_NAME=${dist_name}" >> "$ENV{GITHUB_ENV}"
	)
endif()
add_custom_target(dist ${dist_args})

add_custom_target(dist-xz COMMAND ${CMAKE_COMMAND} -E tar cfvJ "${dist_name}.tar.xz" "${dist_dir}")
add_dependencies(dist-xz dist)

add_custom_target(dist-zip COMMAND ${CMAKE_COMMAND} -E tar cfv "${dist_name}.zip" --format=zip "${dist_dir}")
add_dependencies(dist-zip dist)
