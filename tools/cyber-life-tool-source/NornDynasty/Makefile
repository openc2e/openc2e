CFLAGS = -g -DC2E_SDL -DISOLATED_HISTORY -DNON_LOCALISED_ERRORS -DSSTREAM_INLINE -DMD5_NO_GAME_ENTROPY -fno-default-inline -pg -a
OBJS = NornDynasty.testo ../../common/PRAYFiles/PrayManager.testo ../../common/PRAYFiles/PrayChunk.testo\
	../../common/PRAYFiles/StringIntGroup.testo ../../common/FileLocaliser.testo\
	../../engine/Creature/History/CreatureHistory.testo ../../engine/Creature/History/HistoryStore.testo\
	../../engine/Creature/History/LifeEvent.testo\
	../../engine/CreaturesArchive.testo ../../engine/PersistentObject.testo \
	../../common/FileFuncs.testo \
	../../server/HistoryFeed/SQLInterface.testo \
	../../server/HistoryFeed/HistoryFeed.testo \
	../../engine/md5.testo\
	../../engine/UniqueIdentifier.testo \
	main.testo
SRCS = NornDynasty.cpp ../../common/PRAYFiles/PrayManager.cpp ../../common/PRAYFiles/PrayChunk.cpp\
	../../common/PRAYFiles/StringIntGroup.cpp ../../common/FileLocaliser.cpp\
	../../engine/Creature/History/CreatureHistory.cpp ../../engine/Creature/History/HistoryStore.cpp\
	../../engine/Creature/History/LifeEvent.cpp\
	../../engine/CreaturesArchive.cpp ../../engine/PersistentObject.testo \
	../../common/FileFuncs.cpp \
	../../server/HistoryFeed/SQLInterface.cpp \
	../../server/HistoryFeed/HistoryFeed.cpp \
	../../engine/md5.cpp \
	../../engine/UniqueIdentifier.cpp \
	main.cpp

LIBS = -lz -lmysqlclient

all: norndynasty 

norndynasty: $(OBJS)
	g++ $(CFLAGS) -o norndynasty $(OBJS) $(LIBS)

depend:
	gccmakedep -- $(CFLAGS) -- $(SRCS)
clean:
	rm $(OBJS) norndynasty

$(OBJS): %.testo: %.cpp
	g++ $(CFLAGS) -o $@ -c $<

# DO NOT DELETE
NornDynasty.o: NornDynasty.cpp \
 ../../../c2e/engine/Creature/History/HistoryStore.h \
 ../../../c2e/engine/Creature/History/CreatureHistory.h \
 ../../../c2e/engine/Creature/History/LifeEvent.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/include/_G_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/include/libio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/assert.h /usr/include/pthread.h \
 /usr/include/sched.h /usr/include/bits/sched.h \
 /usr/include/bits/time.h /usr/include/signal.h \
 /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 ../../../c2e/engine/Creature/History/../../../common/C2eTypes.h \
 ../../../c2e/engine/Creature/History/../../../common/../engine/mfchack.h \
 ../../../c2e/engine/Creature/History/../../../common/../engine/../common/Vector2D.h \
 /usr/include/math.h /usr/include/bits/huge_val.h \
 /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/float.h \
 ../../../c2e/engine/Creature/History/../../../common/../engine/../common/C2eDebug.h \
 ../../../c2e/engine/Creature/History/../../../common/../engine/C2eServices.h \
 ../../../c2e/engine/Creature/History/../../../common/../engine/FlightRecorder.h \
 ../../../c2e/engine/Creature/History/../../../common/../engine/../common/C2eTypes.h \
 /usr/include/stdio.h /usr/include/bits/stdio_lim.h \
 ../../../c2e/engine/Creature/History/../../../common/../engine/../common/Catalogue.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/map \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_tree.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algobase.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_pair.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/type_traits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/exception \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_construct.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_function.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_map.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multimap.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 ../../../c2e/engine/Creature/History/../../../common/../engine/../common/BasicException.h \
 ../../../c2e/engine/Creature/History/../../../common/../engine/DirectoryManager.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/vector \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_uninitialized.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_vector.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_bvector.h \
 ../../../c2e/engine/Creature/History/../../../common/../engine/../modules/ModuleAPI.h \
 ../../../c2e/engine/Creature/History/../../../common/../engine/Display/Position.h \
 ../../../c2e/engine/Creature/History/../../../common/../engine/Display/../../common/C2eTypes.h \
 ../../../c2e/engine/Creature/History/../../../modules/ModuleAPI.h \
 ../../../c2e/engine/Creature/History/../../Agents/AgentHandle.h \
 ../../../c2e/engine/Creature/History/../../Agents/../../common/BasicException.h \
 ../../../c2e/engine/Creature/History/../../Agents/../../modules/ModuleAPI.h \
 ../../../c2e/engine/UniqueIdentifier.h \
 ../../../c2e/server/HistoryFeed/HistoryFeed.h \
 ../../../c2e/server/HistoryFeed/SQLInterface.h \
 /usr/include/mysql/mysql.h /usr/include/mysql/mysql_com.h \
 /usr/include/mysql/mysql_version.h /usr/include/mysql/errmsg.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.cc \
 NornDynasty.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/strstream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/strstream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/strfile.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/fstream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/fstream.h
PrayManager.o: ../../common/PRAYFiles/PrayManager.cpp \
 ../../common/PRAYFiles/PrayManager.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/map \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_tree.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algobase.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/include/_G_config.h /usr/include/bits/types.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_pair.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/type_traits.h \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/stdlib.h \
 /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/bits/sigset.h \
 /usr/include/sys/sysmacros.h /usr/include/alloca.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/exception \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/include/libio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/include/assert.h /usr/include/pthread.h /usr/include/sched.h \
 /usr/include/bits/sched.h /usr/include/bits/time.h \
 /usr/include/signal.h /usr/include/bits/pthreadtypes.h \
 /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_construct.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_function.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_map.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multimap.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/vector \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_uninitialized.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_vector.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_bvector.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/list \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_list.h \
 ../../common/PRAYFiles/PrayChunk.h \
 ../../common/PRAYFiles/../C2eTypes.h \
 ../../common/PRAYFiles/../../engine/mfchack.h \
 ../../common/PRAYFiles/../../engine/../common/Vector2D.h \
 /usr/include/math.h /usr/include/bits/huge_val.h \
 /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/float.h \
 ../../common/PRAYFiles/../../engine/../common/C2eDebug.h \
 ../../common/PRAYFiles/../../engine/C2eServices.h \
 ../../common/PRAYFiles/../../engine/FlightRecorder.h \
 ../../common/PRAYFiles/../../engine/../common/C2eTypes.h \
 /usr/include/stdio.h /usr/include/bits/stdio_lim.h \
 ../../common/PRAYFiles/../../engine/../common/Catalogue.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 ../../common/PRAYFiles/../../engine/../common/BasicException.h \
 ../../common/PRAYFiles/../../engine/DirectoryManager.h \
 ../../common/PRAYFiles/../../engine/../modules/ModuleAPI.h \
 ../../common/PRAYFiles/../../engine/Display/Position.h \
 ../../common/PRAYFiles/../../engine/Display/../../common/C2eTypes.h \
 ../../common/PRAYFiles/../../modules/ModuleAPI.h \
 ../../common/PRAYFiles/../FileLocaliser.h \
 ../../common/PRAYFiles/PrayStructs.h \
 ../../common/PRAYFiles/PrayException.h /usr/include/zlib.h \
 /usr/include/zconf.h
PrayChunk.o: ../../common/PRAYFiles/PrayChunk.cpp \
 ../../common/PRAYFiles/PrayChunk.h \
 ../../common/PRAYFiles/../C2eTypes.h \
 ../../common/PRAYFiles/../../engine/mfchack.h \
 ../../common/PRAYFiles/../../engine/../common/Vector2D.h \
 /usr/include/math.h /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h \
 /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/float.h \
 ../../common/PRAYFiles/../../engine/../common/C2eDebug.h \
 /usr/include/assert.h \
 ../../common/PRAYFiles/../../engine/C2eServices.h \
 ../../common/PRAYFiles/../../engine/FlightRecorder.h \
 ../../common/PRAYFiles/../../engine/../common/C2eTypes.h \
 /usr/include/stdio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 ../../common/PRAYFiles/../../engine/../common/Catalogue.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/map \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_tree.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algobase.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_pair.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/type_traits.h \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/stdlib.h \
 /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/bits/sigset.h \
 /usr/include/sys/sysmacros.h /usr/include/alloca.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/exception \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/include/pthread.h /usr/include/sched.h /usr/include/bits/sched.h \
 /usr/include/bits/time.h /usr/include/signal.h \
 /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_construct.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_function.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_map.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multimap.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 ../../common/PRAYFiles/../../engine/../common/BasicException.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 ../../common/PRAYFiles/../../engine/DirectoryManager.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/vector \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_uninitialized.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_vector.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_bvector.h \
 ../../common/PRAYFiles/../../engine/../modules/ModuleAPI.h \
 ../../common/PRAYFiles/../../engine/Display/Position.h \
 ../../common/PRAYFiles/../../engine/Display/../../common/C2eTypes.h \
 ../../common/PRAYFiles/PrayManager.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/list \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_list.h \
 ../../common/PRAYFiles/../../modules/ModuleAPI.h
StringIntGroup.o: ../../common/PRAYFiles/StringIntGroup.cpp \
 ../../common/PRAYFiles/StringIntGroup.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/map \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_tree.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algobase.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/include/_G_config.h /usr/include/bits/types.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_pair.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/type_traits.h \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/stdlib.h \
 /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/bits/sigset.h \
 /usr/include/sys/sysmacros.h /usr/include/alloca.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/exception \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/include/libio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/include/assert.h /usr/include/pthread.h /usr/include/sched.h \
 /usr/include/bits/sched.h /usr/include/bits/time.h \
 /usr/include/signal.h /usr/include/bits/pthreadtypes.h \
 /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_construct.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_function.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_map.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multimap.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream
FileLocaliser.o: ../../common/FileLocaliser.cpp \
 ../../common/FileLocaliser.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/include/_G_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/include/libio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/assert.h /usr/include/pthread.h \
 /usr/include/sched.h /usr/include/bits/sched.h \
 /usr/include/bits/time.h /usr/include/signal.h \
 /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/vector \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algobase.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_pair.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/type_traits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/exception \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_construct.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_uninitialized.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_vector.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_bvector.h \
 ../../common/FileFuncs.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.cc \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/algorithm \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_tempbuf.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algo.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_heap.h \
 /usr/include/dirent.h /usr/include/bits/dirent.h \
 /usr/include/sys/stat.h /usr/include/bits/stat.h \
 /usr/include/unistd.h /usr/include/bits/posix_opt.h \
 /usr/include/bits/confname.h /usr/include/getopt.h \
 /usr/include/fnmatch.h
CreatureHistory.o: ../../engine/Creature/History/CreatureHistory.cpp \
 ../../engine/Creature/History/CreatureHistory.h \
 ../../engine/Creature/History/LifeEvent.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/include/_G_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/include/libio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/assert.h /usr/include/pthread.h \
 /usr/include/sched.h /usr/include/bits/sched.h \
 /usr/include/bits/time.h /usr/include/signal.h \
 /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 ../../engine/Creature/History/../../../common/C2eTypes.h \
 ../../engine/Creature/History/../../../common/../engine/mfchack.h \
 ../../engine/Creature/History/../../../common/../engine/../common/Vector2D.h \
 /usr/include/math.h /usr/include/bits/huge_val.h \
 /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/float.h \
 ../../engine/Creature/History/../../../common/../engine/../common/C2eDebug.h \
 ../../engine/Creature/History/../../../common/../engine/C2eServices.h \
 ../../engine/Creature/History/../../../common/../engine/FlightRecorder.h \
 ../../engine/Creature/History/../../../common/../engine/../common/C2eTypes.h \
 /usr/include/stdio.h /usr/include/bits/stdio_lim.h \
 ../../engine/Creature/History/../../../common/../engine/../common/Catalogue.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/map \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_tree.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algobase.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_pair.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/type_traits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/exception \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_construct.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_function.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_map.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multimap.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 ../../engine/Creature/History/../../../common/../engine/../common/BasicException.h \
 ../../engine/Creature/History/../../../common/../engine/DirectoryManager.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/vector \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_uninitialized.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_vector.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_bvector.h \
 ../../engine/Creature/History/../../../common/../engine/../modules/ModuleAPI.h \
 ../../engine/Creature/History/../../../common/../engine/Display/Position.h \
 ../../engine/Creature/History/../../../common/../engine/Display/../../common/C2eTypes.h \
 ../../engine/Creature/History/../../../modules/ModuleAPI.h \
 ../../engine/Creature/History/../../Agents/AgentHandle.h \
 ../../engine/Creature/History/../../Agents/../../common/BasicException.h \
 ../../engine/Creature/History/../../Agents/../../modules/ModuleAPI.h \
 ../../engine/Creature/History/../../CreaturesArchive.h \
 ../../engine/Creature/History/../../../common/BasicException.h \
 ../../engine/Creature/History/../../Display/ErrorMessageHandler.h \
 ../../engine/Creature/History/../../Display/System.h \
 ../../engine/Creature/History/../../Display/../../common/C2eTypes.h \
 ../../engine/Creature/History/../../TimeFuncs.h /usr/include/zlib.h \
 /usr/include/zconf.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/list \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_list.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/set \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_set.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multiset.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/deque \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_deque.h
HistoryStore.o: ../../engine/Creature/History/HistoryStore.cpp \
 ../../engine/Creature/History/HistoryStore.h \
 ../../engine/Creature/History/CreatureHistory.h \
 ../../engine/Creature/History/LifeEvent.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/include/_G_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/include/libio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/assert.h /usr/include/pthread.h \
 /usr/include/sched.h /usr/include/bits/sched.h \
 /usr/include/bits/time.h /usr/include/signal.h \
 /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 ../../engine/Creature/History/../../../common/C2eTypes.h \
 ../../engine/Creature/History/../../../common/../engine/mfchack.h \
 ../../engine/Creature/History/../../../common/../engine/../common/Vector2D.h \
 /usr/include/math.h /usr/include/bits/huge_val.h \
 /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/float.h \
 ../../engine/Creature/History/../../../common/../engine/../common/C2eDebug.h \
 ../../engine/Creature/History/../../../common/../engine/C2eServices.h \
 ../../engine/Creature/History/../../../common/../engine/FlightRecorder.h \
 ../../engine/Creature/History/../../../common/../engine/../common/C2eTypes.h \
 /usr/include/stdio.h /usr/include/bits/stdio_lim.h \
 ../../engine/Creature/History/../../../common/../engine/../common/Catalogue.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/map \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_tree.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algobase.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_pair.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/type_traits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/exception \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_construct.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_function.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_map.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multimap.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 ../../engine/Creature/History/../../../common/../engine/../common/BasicException.h \
 ../../engine/Creature/History/../../../common/../engine/DirectoryManager.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/vector \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_uninitialized.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_vector.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_bvector.h \
 ../../engine/Creature/History/../../../common/../engine/../modules/ModuleAPI.h \
 ../../engine/Creature/History/../../../common/../engine/Display/Position.h \
 ../../engine/Creature/History/../../../common/../engine/Display/../../common/C2eTypes.h \
 ../../engine/Creature/History/../../../modules/ModuleAPI.h \
 ../../engine/Creature/History/../../Agents/AgentHandle.h \
 ../../engine/Creature/History/../../Agents/../../common/BasicException.h \
 ../../engine/Creature/History/../../Agents/../../modules/ModuleAPI.h \
 ../../engine/Creature/History/../../CreaturesArchive.h \
 ../../engine/Creature/History/../../../common/BasicException.h \
 ../../engine/Creature/History/../../Display/ErrorMessageHandler.h \
 ../../engine/Creature/History/../../Display/System.h \
 ../../engine/Creature/History/../../Display/../../common/C2eTypes.h \
 ../../engine/Creature/History/../../TimeFuncs.h /usr/include/zlib.h \
 /usr/include/zconf.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/list \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_list.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/set \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_set.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multiset.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/deque \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_deque.h
LifeEvent.o: ../../engine/Creature/History/LifeEvent.cpp \
 ../../engine/Creature/History/LifeEvent.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/include/_G_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/include/libio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/assert.h /usr/include/pthread.h \
 /usr/include/sched.h /usr/include/bits/sched.h \
 /usr/include/bits/time.h /usr/include/signal.h \
 /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 ../../engine/Creature/History/../../../common/C2eTypes.h \
 ../../engine/Creature/History/../../../common/../engine/mfchack.h \
 ../../engine/Creature/History/../../../common/../engine/../common/Vector2D.h \
 /usr/include/math.h /usr/include/bits/huge_val.h \
 /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/float.h \
 ../../engine/Creature/History/../../../common/../engine/../common/C2eDebug.h \
 ../../engine/Creature/History/../../../common/../engine/C2eServices.h \
 ../../engine/Creature/History/../../../common/../engine/FlightRecorder.h \
 ../../engine/Creature/History/../../../common/../engine/../common/C2eTypes.h \
 /usr/include/stdio.h /usr/include/bits/stdio_lim.h \
 ../../engine/Creature/History/../../../common/../engine/../common/Catalogue.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/map \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_tree.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algobase.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_pair.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/type_traits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/exception \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_construct.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_function.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_map.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multimap.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 ../../engine/Creature/History/../../../common/../engine/../common/BasicException.h \
 ../../engine/Creature/History/../../../common/../engine/DirectoryManager.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/vector \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_uninitialized.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_vector.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_bvector.h \
 ../../engine/Creature/History/../../../common/../engine/../modules/ModuleAPI.h \
 ../../engine/Creature/History/../../../common/../engine/Display/Position.h \
 ../../engine/Creature/History/../../../common/../engine/Display/../../common/C2eTypes.h \
 ../../engine/Creature/History/../../../modules/ModuleAPI.h \
 ../../engine/Creature/History/../../CreaturesArchive.h \
 ../../engine/Creature/History/../../../common/BasicException.h \
 ../../engine/Creature/History/../../Display/ErrorMessageHandler.h \
 ../../engine/Creature/History/../../Display/System.h \
 ../../engine/Creature/History/../../Display/../../common/C2eTypes.h \
 ../../engine/Creature/History/../../TimeFuncs.h /usr/include/zlib.h \
 /usr/include/zconf.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/list \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_list.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/set \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_set.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multiset.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/deque \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_deque.h
CreaturesArchive.o: ../../engine/CreaturesArchive.cpp \
 ../../engine/CreaturesArchive.h ../../engine/../common/C2eTypes.h \
 ../../engine/../common/../engine/mfchack.h \
 ../../engine/../common/../engine/../common/Vector2D.h \
 /usr/include/math.h /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/huge_val.h \
 /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/float.h \
 ../../engine/../common/../engine/../common/C2eDebug.h \
 /usr/include/assert.h ../../engine/../common/../engine/C2eServices.h \
 ../../engine/../common/../engine/FlightRecorder.h \
 ../../engine/../common/../engine/../common/C2eTypes.h \
 /usr/include/stdio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/include/bits/types.h /usr/include/libio.h \
 /usr/include/_G_config.h /usr/include/bits/stdio_lim.h \
 ../../engine/../common/../engine/../common/Catalogue.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/map \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_tree.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algobase.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_pair.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/type_traits.h \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h /usr/include/stdlib.h \
 /usr/include/sys/types.h /usr/include/time.h /usr/include/endian.h \
 /usr/include/bits/endian.h /usr/include/sys/select.h \
 /usr/include/bits/select.h /usr/include/bits/sigset.h \
 /usr/include/sys/sysmacros.h /usr/include/alloca.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/exception \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/include/pthread.h /usr/include/sched.h /usr/include/bits/sched.h \
 /usr/include/bits/time.h /usr/include/signal.h \
 /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_construct.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_function.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_map.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multimap.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 ../../engine/../common/../engine/../common/BasicException.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 ../../engine/../common/../engine/DirectoryManager.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/vector \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_uninitialized.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_vector.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_bvector.h \
 ../../engine/../common/../engine/../modules/ModuleAPI.h \
 ../../engine/../common/../engine/Display/Position.h \
 ../../engine/../common/../engine/Display/../../common/C2eTypes.h \
 ../../engine/../common/BasicException.h \
 ../../engine/Display/ErrorMessageHandler.h \
 ../../engine/Display/System.h \
 ../../engine/Display/../../common/C2eTypes.h \
 ../../engine/../modules/ModuleAPI.h ../../engine/TimeFuncs.h \
 /usr/include/zlib.h /usr/include/zconf.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/list \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_list.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/set \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_set.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multiset.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/deque \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_deque.h \
 ../../engine/PersistentObject.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/typeinfo
FileFuncs.o: ../../common/FileFuncs.cpp ../../common/FileFuncs.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/include/_G_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/include/libio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/assert.h /usr/include/pthread.h \
 /usr/include/sched.h /usr/include/bits/sched.h \
 /usr/include/bits/time.h /usr/include/signal.h \
 /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.cc \
 /usr/include/sys/stat.h /usr/include/bits/stat.h /usr/include/stdio.h \
 /usr/include/bits/stdio_lim.h /usr/include/unistd.h \
 /usr/include/bits/posix_opt.h /usr/include/bits/confname.h \
 /usr/include/getopt.h /usr/include/fcntl.h /usr/include/bits/fcntl.h \
 /usr/include/sys/mman.h /usr/include/bits/mman.h
SQLInterface.o: ../../server/HistoryFeed/SQLInterface.cpp \
 ../../server/HistoryFeed/SQLInterface.h /usr/include/mysql/mysql.h \
 /usr/include/sys/types.h /usr/include/features.h \
 /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h \
 /usr/include/bits/types.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/mysql/mysql_com.h /usr/include/mysql/mysql_version.h \
 /usr/include/mysql/errmsg.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/include/libio.h /usr/include/_G_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/include/stdlib.h /usr/include/alloca.h /usr/include/assert.h \
 /usr/include/pthread.h /usr/include/sched.h /usr/include/bits/sched.h \
 /usr/include/bits/time.h /usr/include/signal.h \
 /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.cc \
 /usr/include/mysql/mysqld_error.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/vector \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algobase.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_pair.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/type_traits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/exception \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_construct.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_uninitialized.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_vector.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_bvector.h
HistoryFeed.o: ../../server/HistoryFeed/HistoryFeed.cpp \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/HistoryStore.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/CreatureHistory.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/LifeEvent.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/include/_G_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/include/libio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/assert.h /usr/include/pthread.h \
 /usr/include/sched.h /usr/include/bits/sched.h \
 /usr/include/bits/time.h /usr/include/signal.h \
 /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/C2eTypes.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/mfchack.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/../common/Vector2D.h \
 /usr/include/math.h /usr/include/bits/huge_val.h \
 /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/float.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/../common/C2eDebug.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/C2eServices.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/FlightRecorder.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/../common/C2eTypes.h \
 /usr/include/stdio.h /usr/include/bits/stdio_lim.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/../common/Catalogue.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/map \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_tree.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_algobase.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_pair.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/type_traits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/limits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/syslimits.h \
 /usr/include/limits.h /usr/include/bits/posix1_lim.h \
 /usr/include/bits/local_lim.h /usr/include/linux/limits.h \
 /usr/include/bits/posix2_lim.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/new \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/exception \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_construct.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_function.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_map.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_multimap.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/../common/BasicException.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/DirectoryManager.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/vector \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_uninitialized.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_vector.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_bvector.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/../modules/ModuleAPI.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/Display/Position.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../common/../engine/Display/../../common/C2eTypes.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../../modules/ModuleAPI.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../Agents/AgentHandle.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../Agents/../../common/BasicException.h \
 ../../server/HistoryFeed/../../../c2e/engine/Creature/History/../../Agents/../../modules/ModuleAPI.h \
 ../../server/HistoryFeed/HistoryFeed.h \
 ../../server/HistoryFeed/SQLInterface.h /usr/include/mysql/mysql.h \
 /usr/include/mysql/mysql_com.h /usr/include/mysql/mysql_version.h \
 /usr/include/mysql/errmsg.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.cc \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/fstream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/fstream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/strstream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/strstream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/strfile.h
md5.o: ../../engine/md5.cpp ../../engine/md5.h /usr/include/memory.h \
 /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h
UniqueIdentifier.o: ../../engine/UniqueIdentifier.cpp \
 ../../engine/UniqueIdentifier.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/string \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstddef \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stddef.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/straits.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cctype \
 /usr/include/ctype.h /usr/include/features.h /usr/include/sys/cdefs.h \
 /usr/include/gnu/stubs.h /usr/include/bits/types.h \
 /usr/include/endian.h /usr/include/bits/endian.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/cstring \
 /usr/include/string.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_config.h \
 /usr/include/_G_config.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_alloc.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/streambuf.h \
 /usr/include/libio.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/include/stdarg.h \
 /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h \
 /usr/include/sys/select.h /usr/include/bits/select.h \
 /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h \
 /usr/include/alloca.h /usr/include/assert.h /usr/include/pthread.h \
 /usr/include/sched.h /usr/include/bits/sched.h \
 /usr/include/bits/time.h /usr/include/signal.h \
 /usr/include/bits/pthreadtypes.h /usr/include/bits/sigthread.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iterator \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_relops.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/stl_iterator.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/std/bastring.cc \
 ../../engine/md5.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.h \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iostream \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/sstream.cc \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iomanip \
 /usr/local/lib/gcc-lib/i686-pc-linux-gnu/2.95.2/../../../../include/g++-3/iomanip.h \
 /usr/include/stdio.h /usr/include/bits/stdio_lim.h
