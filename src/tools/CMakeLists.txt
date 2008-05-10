# Note: This will rebuild some .o files.
# currently, cmake can't link object files directly.
# It would be cleanest to seperate out a libmng, libpray, etc.
# into seperate project subdirs, and then have tools depend on that.

PROJECT (OPENC2E_TOOLS CXX)

ADD_EXECUTABLE(praydumper praydumper.cpp ${SRC}/pray.cpp)

TARGET_LINK_LIBRARIES(praydumper z m pthread 
	boost_program_options-mt
	boost_serialization-mt
	boost_filesystem-mt)


# This is buggy because of generated files. mng needs to be a library!
# ADD_EXECUTABLE(mngtest mngtest.cpp ${SRC}/mngfile.cpp ${BIN}/mngparser.tab.cpp ${BIN}/lex.mng.cpp)

