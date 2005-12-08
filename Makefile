CXXFILES = $(wildcard *.cpp) $(wildcard tools/*.cpp)
CFILES = $(wildcard *.c) $(wildcard tools/*.c) 

SOURCES = $(CXXFILES) $(CFILES)
HEADERS = $(wildcard *.h)
EVERYTHING = $(SOURCES) $(HEADERS)

SOURCEDEPS = $(patsubst %.cpp,.deps/%.dpp,$(CXXFILES)) \
			 $(patsubst %.c,.deps/%.d,$(CFILES))

OPENC2E = \
	Agent.o \
	AgentRef.o \
	attFile.o \
	blkImage.o \
	c16Image.o \
	Camera.o \
	caosScript.o \
	caosVar.o \
	caosVM_agent.o \
	caosVM_camera.o \
	caosVM_compound.o \
	caosVM_core.o \
	caosVM.o \
	caosVM_creatures.o \
	caosVM_debug.o \
	caosVM_files.o \
	caosVM_flow.o \
	caosVM_input.o \
	caosVM_map.o \
	caosVM_motion.o \
	caosVM_net.o \
	caosVM_ports.o \
	caosVM_resources.o \
	caosVM_scripts.o \
	caosVM_sounds.o \
	caosVM_time.o \
	caosVM_variables.o \
	caosVM_vehicles.o \
	caosVM_world.o \
	Catalogue.o \
	CompoundAgent.o \
	CompoundPart.o \
	Creature.o \
	creaturesImage.o \
	fileSwapper.o \
	genomeFile.o \
	main.o \
	Map.o \
	MetaRoom.o \
	mmapifstream.o \
	mngfile.o \
	physics.o \
	PointerAgent.o \
	pray.o \
	Room.o \
	Scriptorium.o \
	SDLBackend.o \
	SDL_gfxPrimitives.o \
	SimpleAgent.o \
	SkeletalCreature.o \
	streamutils.o \
	Vehicle.o \
	World.o \
	PathResolver.o \
	cmddata.o \
	lex.yy.o \
	lexutil.o \
	dialect.o \
	lex.mng.o \
	mngparser.tab.o \
	gc.o \
	bytecode.o

CFLAGS += -W -Wall -Wno-conversion -Wno-unused
XLDFLAGS=$(LDFLAGS) -lboost_filesystem $(shell sdl-config --libs) -lz -lm -lSDL_net -lSDL_mixer
COREFLAGS=-ggdb3 $(shell sdl-config --cflags) -I.
XCFLAGS=$(CFLAGS) $(COREFLAGS)
XCPPFLAGS=$(COREFLAGS) $(CPPFLAGS) $(CFLAGS)

all: openc2e tools/mngtest tools/filetests tools/praydumper docs tools/pathtest tools/gctest tools/memstats

docs: docs.html

commandinfo.yml: $(wildcard caosVM_*.cpp) parsedocs.pl
	perl parsedocs.pl $(wildcard caosVM_*.cpp) > commandinfo.yml

docs.html: writehtml.pl commandinfo.yml
	perl writehtml.pl > docs.html

cmddata.cpp: commandinfo.yml writecmds.pl
	perl writecmds.pl commandinfo.yml > cmddata.cpp

lex.mng.cpp lex.mng.h: mng.l mngparser.tab.hpp
	flex -+ --prefix=mng -d -o lex.mng.cpp --header-file=lex.mng.h mng.l

mngfile.o: lex.mng.cpp

mngparser.tab.cpp mngparser.tab.hpp: mngparser.ypp
	bison -d --name-prefix=mng mngparser.ypp

lex.yy.cpp lex.yy.h: caos.l
	flex -+ -d -o lex.yy.cpp --header-file=lex.yy.h caos.l

## lex.yy.h deps aren't detected evidently
caosScript.o: lex.yy.h lex.yy.cpp

## based on automake stuff
%.o: %.cpp
	mkdir -p .deps/`dirname $<` && \
	$(CXX) $(XCPPFLAGS) -MP -MD -MF .deps/$<.Td -o $@ -c $< && \
	mv .deps/$<.Td .deps/$<.d

%.o: %.c
	mkdir -p .deps/`dirname $<` && \
	$(CC) $(XCFLAGS) -MP -MD -MF .deps/$<.Td -o $@ -c $< && \
	mv .deps/$<.Td .deps/$<.d

include $(shell find .deps -name '*.d' -type f 2>/dev/null || true)

openc2e: $(OPENC2E)
	$(CXX) $(XLDFLAGS) $(XCXXFLAGS) -o $@ $^

tools/filetests: tools/filetests.o genomeFile.o streamutils.o Catalogue.o
	$(CXX) $(XLDFLAGS) $(XCXXFLAGS) -o $@ $^

tools/praydumper: tools/praydumper.o pray.o
	$(CXX) $(XLDFLAGS) $(XCXXFLAGS) -o $@ $^

tools/mngtest: tools/mngtest.o mngfile.o mngparser.tab.o lex.mng.o
	$(CXX) $(XLDFLAGS) $(XCXXFLAGS) -o $@ $^

tools/pathtest: tools/pathtest.o PathResolver.o
	$(CXX) $(XLDFLAGS) $(XCXXFLAGS) -o $@ $^

tools/gctest: tools/gctest.o gc.o
	$(CXX) $(XLDFLAGS) $(XCXXFLAGS) -o $@ $^

tools/memstats: tools/memstats.o $(patsubst main.o,,$(OPENC2E))
	$(CXX) $(XLDFLAGS) $(XCXXFLAGS) -o $@ $^

clean:
	rm -f *.o openc2e filetests praydumper tools/*.o
	rm -rf .deps
	rm -f commandinfo.yml lex.yy.cpp lex.yy.h lex.mng.cpp lex.mng.h mngparser.tab.cpp mngparser.tab.hpp cmddata.cpp

test: openc2e 
	perl -MTest::Harness -e 'runtests(glob("unittests/*.t"))'

.PHONY: clean all dep docs
