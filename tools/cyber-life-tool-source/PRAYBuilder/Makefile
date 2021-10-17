SRCS = Interface.cpp \
	../../common/PRAYFiles/PrayChunk.cpp \
	../../common/PRAYFiles/PrayManager.cpp \
	../../common/PRAYFiles/StringIntGroup.cpp \
	../../common/FileLocaliser.cpp \
	../../common/SimpleLexer.cpp \
	../../common/FileFuncs.cpp

OBJS = $(patsubst %.cpp,%.praybuildero,$(SRCS) )

CFLAGS = -DSSTREAM_INLINE -g -DPRAY_BUILDER
LIBS = -lpthread -lz

praybuilder: $(OBJS)
	g++ $(CFLAGS) $(LIBS) -o praybuilder $(OBJS)

clean:
	rm -fr $(OBJS)

%.praybuildero : %.cpp
	g++ $(CFLAGS) -o $@ -c $<

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

