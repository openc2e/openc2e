cmake_minimum_required(VERSION 3.0)
project(openc2e)

set(SRC "${CMAKE_CURRENT_SOURCE_DIR}")
set(BIN "${CMAKE_CURRENT_BINARY_DIR}")
set(GEN "${BIN}/generated")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "") # so openc2e can find the DLLs
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" CACHE PATH "") # so dist can find the shared libraries

set(CMAKE_MODULE_PATH "${SRC}/cmake" "${SRC}/externals/aminosbh-sdl2-cmake-modules")

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
		"The type of build to perform. Valid values are: RelWithDebInfo (default), Debug, Release, MinSizeRel" FORCE)
endif()

option(OPENC2E_USE_QT "Build the qt GUI" ON)
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
	add_subdirectory(externals/winflexbison EXCLUDE_FROM_ALL)
	set(BISON_EXECUTABLE "win_bison")

	add_subdirectory(externals/zlib EXCLUDE_FROM_ALL)
else()
	find_package(BISON REQUIRED)

	find_package(ZLIB REQUIRED)
endif()

if(WIN32 OR APPLE)
	add_subdirectory(externals/libpng EXCLUDE_FROM_ALL)

	add_subdirectory(externals/SDL2 EXCLUDE_FROM_ALL)
	add_subdirectory(externals/SDL2_mixer EXCLUDE_FROM_ALL)
	add_subdirectory(externals/SDL2_net EXCLUDE_FROM_ALL)
	add_subdirectory(externals/SDL2_gfx EXCLUDE_FROM_ALL)
	add_library(SDL2::Core ALIAS SDL2)
	add_library(SDL2::Mixer ALIAS SDL2_mixer)
	add_library(SDL2::Net ALIAS SDL2_net)
	add_library(SDL2::GFX ALIAS SDL2_gfx)
else()
	find_package(PNG REQUIRED)

	find_package(SDL2 REQUIRED)
	find_package(SDL2_mixer REQUIRED)
	find_package(SDL2_net REQUIRED)
	find_package(SDL2_gfx REQUIRED)
endif()

if (OPENC2E_USE_SERIALIZATION)
	find_package(Boost 1.34.0 COMPONENTS serialization REQUIRED)
endif()
if (OPENC2E_USE_QT)
	if(WIN32)
		file(GLOB qt_directories "C:/Qt/Qt5.*/5.*/msvc*")
		list(APPEND CMAKE_PREFIX_PATH ${qt_directories})
	endif()
	find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif()

add_subdirectory(externals/cxxopts EXCLUDE_FROM_ALL)
add_subdirectory(externals/fmt EXCLUDE_FROM_ALL)
add_subdirectory(externals/ghc_filesystem EXCLUDE_FROM_ALL)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # fix 'mismatch detected for RuntimeLibrary'
add_subdirectory(externals/googletest EXCLUDE_FROM_ALL)
add_subdirectory(externals/mpark-variant EXCLUDE_FROM_ALL)
add_subdirectory(externals/utf8proc EXCLUDE_FROM_ALL)

# Pass flags to the compiler
add_compile_definitions("_REENTRANT" "YYERROR_VERBOSE")
if(WIN32)
	add_compile_definitions("VCPP_BROKENNESS")
endif()
if (CMAKE_CXX_COMPILER_ID MATCHES "(Clang|GNU)")
	add_compile_options(-Wextra -Wall -Wno-conversion -Wno-unused)
endif()
set(CMAKE_CXX_STANDARD 11)

include_directories(BEFORE
	${GEN}
	${SRC}/src
)

# Targets
add_library(openc2e-common STATIC
	src/encoding.cpp
)
target_link_libraries(openc2e-common PRIVATE utf8proc)

add_library(mngfile
	src/mngfile/mngfile.cpp
	src/mmapifstream.cpp
	${GEN}/mngfile/mnglexer.cpp
	${GEN}/mngfile/mngparser.tab.cpp
)
target_link_libraries(mngfile PRIVATE fmt)

add_library(prayfile
	src/prayfile/PrayFileReader.cpp
	src/prayfile/PrayFileWriter.cpp
	${GEN}/prayfile/Caos2PrayParser.cpp
	${GEN}/prayfile/PraySourceParser.cpp
)
target_link_libraries(prayfile
	PUBLIC fmt ghc_filesystem mpark_variant openc2e-common
	PRIVATE ZLIB::ZLIB
)

add_library(openc2e-core STATIC
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
	src/creatures/genomeFile.cpp
	src/historyManager.cpp
	src/imageManager.cpp
	${GEN}/caoslexer.cpp
	src/Lift.cpp
	src/Map.cpp
	src/MetaRoom.cpp
	src/MusicManager.cpp
	src/creatures/oldBrain.cpp
	src/partzorder.cpp
	src/PathResolver.cpp
	src/peFile.cpp
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
	src/images/sprImage.cpp
	src/Vehicle.cpp
	src/VoiceData.cpp
	src/World.cpp
	src/userlocale.cpp
	src/util.cpp
)
target_link_libraries(openc2e-core PUBLIC
	cxxopts
	fmt
	ghc_filesystem
	mpark_variant
	openc2e-common
	mngfile
	prayfile
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

add_executable(openc2e
	src/main.cpp
	src/backends/SDLBackend.cpp
	src/backends/SDLMixerBackend.cpp
)
target_compile_definitions(openc2e PRIVATE "SDLMIXER_SUPPORT")
target_link_libraries(openc2e
	openc2e-core
	SDL2::Core
	SDL2::GFX
	SDL2::Mixer
	SDL2::Net
)
if (OPENC2E_USE_QT)
	target_sources(openc2e PRIVATE
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
		src/tools/braininavat/brainview.cpp
		src/backends/qtgui/agents.ui
	)
	target_link_libraries(openc2e Qt5::Widgets)
	target_compile_definitions(openc2e PUBLIC "QT_SUPPORT")
	set_target_properties(openc2e PROPERTIES
		AUTOMOC ON
		AUTORCC ON
		AUTOUIC ON
	)

	if (WIN32)
		find_program(WINDEPLOYQT_EXECUTABLE windeployqt)
		get_target_property(qt_core_location Qt5::Core LOCATION)
		get_filename_component(qt_runtime_dir "${qt_core_location}" DIRECTORY)
		add_custom_command(
			TARGET openc2e POST_BUILD
			COMMAND ${CMAKE_COMMAND}
			-D "DIST_DIR=${CMAKE_CURRENT_BINARY_DIR}"
			-D "EXTRA_LIB_DIRS=${qt_runtime_dir}"
			-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Openc2eBundle.cmake
		)
		if(OPENC2E_USE_QT)
				add_custom_command(
				  TARGET openc2e POST_BUILD
					COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/platforms/"
					COMMAND ${CMAKE_COMMAND} -E copy_if_different
					"$<TARGET_FILE:Qt5::QWindowsIntegrationPlugin>"
					"${CMAKE_CURRENT_BINARY_DIR}/platforms/"
				)
		endif()
	endif()
endif()

add_executable(mngdumper src/tools/mngdumper.cpp)
target_link_libraries(mngdumper fmt ghc_filesystem mngfile)

add_executable(praydumper src/tools/praydumper.cpp)
target_link_libraries(praydumper prayfile)

add_executable(praybuilder src/tools/praybuilder.cpp ${GEN}/caoslexer.cpp)
target_link_libraries(praybuilder prayfile)

add_executable(spritedumper
	src/tools/spritedumper.cpp
	src/images/blkImage.cpp
	src/images/c16Image.cpp
	src/images/sprImage.cpp
	src/creaturesImage.cpp
)
target_link_libraries(spritedumper fmt ghc_filesystem PNG::PNG)

# tests
enable_testing()
include(GoogleTest)
include(Openc2eOutputOnTestFailure)

add_executable(test_openc2e
	tests/test_caos.cpp
	tests/test_encoding.cpp
	tests/test_pray.cpp
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
	OUTPUT ${BIN}/docs.css
	COMMAND ${CMAKE_COMMAND} -E copy_if_different docs/docs.css ${BIN}/docs.css
	DEPENDS docs/docs.css
	WORKING_DIRECTORY ${SRC})
add_custom_command(
	OUTPUT ${BIN}/openc2e.6
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SRC}/docs/openc2e.6 ${BIN}/openc2e.6
	DEPENDS docs/openc2e.6
	)
# it seems these add_custom_command for flex and bison could be made
# into a macro... e.g. FLEX(file1) FLEX(file2) etc...
add_custom_command(
	OUTPUT ${GEN}/cataloglexer.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/cataloglexer.cpp ${SRC}/src/cataloglexer.re2c
	DEPENDS ${SRC}/src/cataloglexer.re2c
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/cmddata.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}"
	COMMAND ${Python_EXECUTABLE} writecmds.py ${GEN}/commandinfo.json > ${GEN}/cmddata.cpp
	DEPENDS ${GEN}/commandinfo.json writecmds.py
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/caoslexer.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/caoslexer.cpp ${SRC}/src/caoslexer.re2c
	DEPENDS ${SRC}/src/caoslexer.re2c
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/mngfile/mnglexer.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}/mngfile"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/mngfile/mnglexer.cpp ${SRC}/src/mngfile/mnglexer.re2c
	DEPENDS ${SRC}/src/mngfile/mnglexer.re2c
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/prayfile/Caos2PrayParser.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}/prayfile"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/prayfile/Caos2PrayParser.cpp ${SRC}/src/prayfile/Caos2PrayParser.re2c
	DEPENDS ${SRC}/src/prayfile/Caos2PrayParser.re2c
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/prayfile/PraySourceParser.cpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}/prayfile"
	COMMAND ${RE2C_EXECUTABLE} -o ${GEN}/prayfile/PraySourceParser.cpp ${SRC}/src/prayfile/PraySourceParser.re2c
	DEPENDS ${SRC}/src/prayfile/PraySourceParser.re2c
	WORKING_DIRECTORY ${SRC})

add_custom_command(
	OUTPUT ${GEN}/mngfile/mngparser.tab.cpp ${GEN}/mngfile/mngparser.tab.hpp
	COMMAND ${CMAKE_COMMAND} -E make_directory "${GEN}"
	COMMAND ${BISON_EXECUTABLE} -d --name-prefix=mng --file-prefix="${GEN}/mngfile/mngparser" src/mngfile/mngparser.ypp
	DEPENDS src/mngfile/mngparser.ypp
	WORKING_DIRECTORY ${SRC})

add_custom_target(docs ALL DEPENDS ${BIN}/caosdocs.html ${BIN}/docs.css ${BIN}/openc2e.6)
add_custom_target(generated DEPENDS
	${GEN}/caoslexer.cpp
	${GEN}/mngparser.tab.cpp ${GEN}/mngparser.tab.hpp
	${GEN}/mnglexer.cpp
	${GEN}/cataloglexer.cpp
	${GEN}/cmddata.cpp
)

# Creating folder for distribution
include(Openc2eReadGitRev)
set(dist_name "openc2e-${git_short_rev}-${CMAKE_SYSTEM_NAME}")
set(dist_dir "${CMAKE_CURRENT_BINARY_DIR}/${dist_name}")
set(dist_args)
list(APPEND dist_args
	COMMAND ${CMAKE_COMMAND} -E echo "::set-env name=DIST_NAME::${dist_name}"
	COMMAND ${CMAKE_COMMAND} -E make_directory "${dist_dir}/"
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
		"${CMAKE_CURRENT_SOURCE_DIR}/AUTHORS"
		"${CMAKE_CURRENT_SOURCE_DIR}/COPYING"
		"${CMAKE_CURRENT_SOURCE_DIR}/README.md"
		"${CMAKE_CURRENT_SOURCE_DIR}/docs/openc2e.6"
		"${BIN}/caosdocs.html"
		"${BIN}/docs.css"
		"$<TARGET_FILE:openc2e>"
		"$<TARGET_FILE:mngdumper>"
		"$<TARGET_FILE:praybuilder>"
		"$<TARGET_FILE:praydumper>"
		"$<TARGET_FILE:spritedumper>"
		"${dist_dir}/"
	DEPENDS "${BIN}/caosdocs.html" "${BIN}/docs.css"
	COMMAND ${CMAKE_COMMAND}
		-D "DIST_DIR=${dist_dir}"
		-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Openc2eBundle.cmake
)
if(APPLE)
	list(APPEND dist_args
		COMMAND echo "sudo xattr -r -d com.apple.quarantine ." > "${dist_dir}/fix_unidentified_developer.sh"
		COMMAND chmod +x "${dist_dir}/fix_unidentified_developer.sh"
	)
endif()
if(OPENC2E_USE_QT)
	if(WIN32)
		list(APPEND dist_args
			COMMAND ${CMAKE_COMMAND} -E make_directory "${dist_dir}/platforms/"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"$<TARGET_FILE:Qt5::QWindowsIntegrationPlugin>"
			"${dist_dir}/platforms/"
		)
	elseif(APPLE)
		list(APPEND dist_args
			COMMAND ${CMAKE_COMMAND} -E make_directory "${dist_dir}/platforms/"
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"$<TARGET_FILE:Qt5::QCocoaIntegrationPlugin>"
			"${dist_dir}/platforms/"
		)
	endif()
endif()
add_custom_target(dist ${dist_args})

add_custom_target(dist-xz COMMAND ${CMAKE_COMMAND} -E tar cfvJ "${dist_name}.tar.xz" "${dist_dir}")
add_dependencies(dist-xz dist)

add_custom_target(dist-zip COMMAND ${CMAKE_COMMAND} -E tar cfv "${dist_name}.zip" --format=zip "${dist_dir}")
add_dependencies(dist-zip dist)
