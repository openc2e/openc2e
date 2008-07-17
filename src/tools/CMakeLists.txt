# Note: This will rebuild some .o files.
# currently, cmake can't link object files directly.
# It would be cleanest to seperate out a libmng, libpray, etc.
# into seperate project subdirs, and then have tools depend on that.

PROJECT (OPENC2E_TOOLS CXX)

#ADD_EXECUTABLE(praydumper praydumper.cpp ${SRC}/pray.cpp)

# This is buggy because of generated files. mng needs to be a library!
ADD_DEFINITIONS("-I..")
ADD_EXECUTABLE(mngtest mngtest.cpp ../mngfile.cpp ../../build/generated/mngparser.tab.cpp ../../build/generated/mnglexer.cpp ../mmapifstream.cpp)
TARGET_LINK_LIBRARIES(mngtest z m pthread
	boost_program_options-mt
	boost_serialization-mt
	boost_filesystem-mt)

