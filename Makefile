CXXFILES = $(wildcard *.cpp) $(wildcard tools/*.cpp)
CFILES = $(wildcard *.c) $(wildcard tools/*.c) 

SOURCES = $(CXXFILES) $(CFILES)
HEADERS = $(wildcard *.h)
EVERYTHING = $(SOURCES) $(HEADERS)

SOURCEDEPS = $(patsubst %.cpp,.deps/%.dpp,$(CXXFILES)) \
			 $(patsubst %.c,.deps/%.d,$(CFILES))

OPENC2E_CORE = \
	Agent.o \
	AgentHelpers.o \
	AgentRef.o \
	attFile.o \
	Blackboard.o \
	blkImage.o \
	bytecode.o \
	c16Image.o \
	c2eBrain.o \
	CallButton.o \
	Camera.o \
	caosScript.o \
	caosVar.o \
	caosVM_agent.o \
	caosVM_camera.o \
	caosVM_compound.o \
	caosVM_core.o \
	caosVM_creatures.o \
	caosVM_debug.o \
	caosVM_files.o \
	caosVM_flow.o \
	caosVM_genetics.o \
	caosVM_history.o \
	caosVM_input.o \
	caosVM_map.o \
	caosVM_motion.o \
	caosVM_net.o \
	caosVM.o \
	caosVM_ports.o \
	caosVM_resources.o \
	caosVM_scripts.o \
	caosVM_sounds.o \
	caosVM_time.o \
	caosVM_variables.o \
	caosVM_vectors.o \
	caosVM_vehicles.o \
	caosVM_world.o \
	catalogue.lex.o \
	Catalogue.o \
	catalogue.tab.o \
	cmddata.o \
	cobFile.o \
	CompoundAgent.o \
	CompoundPart.o \
	Creature.o \
	CreatureAgent.o \
	creaturesImage.o \
	dialect.o \
	Engine.o \
	exceptions.o \
	fileSwapper.o \
	genomeFile.o \
	historyManager.o \
	lex.c2.o \
	lex.mng.o \
	lexutil.o \
	lex.yy.o \
	Lift.o \
	Map.o \
	MetaRoom.o \
	mmapifstream.o \
	mngfile.o \
	mngparser.tab.o \
	oldBrain.o \
	OpenALBackend.o \
	PathResolver.o \
	physics.o \
	PointerAgent.o \
	pray.o \
	prayManager.o \
	renderable.o \
	Room.o \
	Scriptorium.o \
	SDLBackend.o \
	SFCFile.o \
	SimpleAgent.o \
	SkeletalCreature.o \
	sprImage.o \
	streamutils.o \
	Vehicle.o \
	World.o

SERIALIZATION = caosVM_ser_real.o
SERSTUB = caosVM_ser_stub.o
OPENC2E = $(OPENC2E_CORE) $(SERSTUB) main.o
OPENC2E_S = $(OPENC2E_CORE) $(SERIALIZATION) main.o

DEBUGFLAGS=-ggdb3 -O2
CFLAGS += -W -Wall -Wno-conversion -Wno-unused -pthread -D_REENTRANT -DYYERROR_VERBOSE -DOPENAL_SUPPORT
XLDFLAGS=$(LDFLAGS) -lboost_program_options-mt -lboost_serialization-mt -lboost_filesystem-mt -lboost_thread-mt -lSDL -lz -lm -lSDL_net -lSDL_gfx -lpthread -lopenal -lalut -lboost_regex-mt
COREFLAGS=$(DEBUGFLAGS) -I/usr/include/SDL -I/usr/include/AL -I.
XCFLAGS=$(CFLAGS) $(COREFLAGS)
XCPPFLAGS=$(COREFLAGS) $(CPPFLAGS) $(CFLAGS)

default: openc2e tools/praydumper docs
all: openc2e tools/mngtest tools/filetests tools/praydumper docs tools/pathtest tools/memstats tools/wildtest

docs: docs.html

commandinfo.yml: $(wildcard caosVM_*.cpp) parsedocs.pl
	perl parsedocs.pl $(wildcard caosVM_*.cpp) > commandinfo.yml

docs.html: writehtml.pl commandinfo.yml
	perl writehtml.pl commandinfo.yml > docs.html

cmddata.cpp: commandinfo.yml writecmds.pl
	perl writecmds.pl commandinfo.yml > cmddata.cpp

lex.mng.cpp lex.mng.h: mng.l
	flex -+ --prefix=mng -d -o lex.mng.cpp --header-file=lex.mng.h mng.l

catalogue.lex.cpp catalogue.lex.h: catalogue.l catalogue.tab.hpp
	flex -+ --prefix=catalogue -d -o catalogue.lex.cpp --header-file=catalogue.lex.h catalogue.l 
	
mngfile.o: lex.mng.cpp

mngparser.tab.cpp mngparser.tab.hpp: mngparser.ypp
	bison -d --name-prefix=mng mngparser.ypp

catalogue.tab.cpp catalogue.tab.hpp: catalogue.ypp
	bison -d --name-prefix=cata catalogue.ypp

lex.yy.cpp lex.yy.h: caos.l
	flex -+ -d -o lex.yy.cpp --header-file=lex.yy.h --prefix=c2e caos.l

lex.c2.cpp lex.c2.h: c2caos.l
	flex -+ -d -o lex.c2.cpp --header-file=lex.c2.h --prefix=c2 c2caos.l

## lex.yy.h deps aren't detected evidently
caosScript.o: lex.yy.h lex.yy.cpp lex.c2.h lex.c2.cpp

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
Catalogue.o: catalogue.lex.h catalogue.tab.hpp
lex.mng.o: mngparser.tab.hpp

Creature_standalone.o: Creature.cpp Creature.o
	$(CC) $(XCFLAGS) -D_CREATURE_STANDALONE -o $@ -c $<

openc2e: $(OPENC2E)
	$(CXX) -o $@ $^ $(XLDFLAGS) $(XCXXFLAGS)

openc2e_s: $(OPENC2E_S)
	$(CXX) -o $@ $^ $(XLDFLAGS) $(XCXXFLAGS)

tools/filetests: tools/filetests.o genomeFile.o streamutils.o Catalogue.o catalogue.lex.o catalogue.tab.o
	$(CXX) -o $@ $^ $(XLDFLAGS) $(XCXXFLAGS)

tools/praydumper: tools/praydumper.o pray.o
	$(CXX) -o $@ $^ $(XLDFLAGS) $(XCXXFLAGS)

tools/mngtest: tools/mngtest.o mngfile.o mngparser.tab.o lex.mng.o
	$(CXX) -o $@ $^ $(XLDFLAGS) $(XCXXFLAGS)

tools/pathtest: tools/pathtest.o PathResolver.o
	$(CXX) -o $@ $^ $(XLDFLAGS) $(XCXXFLAGS)

tools/wildtest: tools/wildtest.o PathResolver.o
	$(CXX) -o $@ $^ $(XLDFLAGS) $(XCXXFLAGS)

tools/memstats: tools/memstats.o $(OPENC2E_CORE) $(SERSTUB)
	$(CXX) -o $@ $^ $(XLDFLAGS) $(XCXXFLAGS)

tools/serialtest: tools/serialtest.o $(OPENC2E_CORE) $(SERIALIZATION)
	$(CXX) -o $@ $^ $(XLDFLAGS) $(XCXXFLAGS)

tools/braininavat/braininavat: Creature_standalone.o c2eBrain.o streamutils.o genomeFile.o
	cd tools/braininavat && \
	(qmake-qt4 || qmake) && \
	make

clean:
	rm -f *.o openc2e openc2e_s filetests praydumper tools/*.o
	rm -rf .deps
	rm -f commandinfo.yml lex.yy.cpp lex.yy.h lex.c2.cpp lex.c2.h lex.mng.cpp lex.mng.h mngparser.tab.cpp mngparser.tab.hpp cmddata.cpp
	rm -f tools/filetests tools/memstats tools/mngtest tools/pathtest tools/praydumper tools/serialtest
	rm -f headerdeps.dot headerdeps.png
	rm -f .shake/dump.pl
	rm -f tools/braininavat/Makefile tools/braininavat/*.o tools/braininavat/moc_*.cpp tools/braininavat/braininavat
	rm -f catalogue.lex.cpp catalogue.lex.h catalogue.tab.cpp catalogue.tab.hpp docs.html

test: openc2e 
	perl runtests.pl

headerdeps.dot: $(wildcard *.h) $(wildcard *.hpp) $(wildcard ser/*.h) mngparser.tab.hpp catalogue.tab.hpp lex.mng.h lex.yy.h catalogue.lex.h
	tools/depgraph.sh $^ > $@

headerdeps.png: headerdeps.dot
	dot -Tpng -o $@ $^

generated: catalogue.tab.hpp lex.yy.h lex.c2.h mngparser.tab.hpp catalogue.lex.h lex.mng.h docs.html cmddata.cpp

.PHONY: clean all dep docs test
