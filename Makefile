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
	caosdata.o \
	caoshashes.o \
	caosScript.o \
	caosVar.o \
	caosVM_agent.o \
	caosVM_camera.o \
	caosVM_cmdinfo.o \
	caosVM_compound.o \
	caosVM_core.o \
	caosVM.o \
	caosVM_creatures.o \
	caosVM_debug.o \
	caosVM_flow.o \
	caosVM_input.o \
	caosVM_map.o \
	caosVM_motion.o \
	caosVM_ports.o \
	caosVM_resources.o \
	caosVM_scripts.o \
	caosVM_sounds.o \
	caosVM_time.o \
	caosVM_variables.o \
	caosVM_vehicles.o \
	Catalogue.o \
	CompoundAgent.o \
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
	PathResolver.o

XLDFLAGS=$(LDFLAGS) -lboost_filesystem $(shell sdl-config --static-libs) -lz -lm -lSDL_net -lSDL_mixer
COREFLAGS=-ggdb3 $(shell sdl-config --cflags) -I.
XCFLAGS=$(CFLAGS) $(COREFLAGS)
XCPPFLAGS=$(COREFLAGS) $(CPPFLAGS) $(CFLAGS)

all: openc2e tools/filetests tools/praydumper

%.o: %.cpp
	$(CXX) $(XCPPFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(XCFLAGS) -o $@ -c $<

caosdata.cpp caoshashes.cpp: $(wildcard caosVM_*.cpp)
	cd caosdata; \
	chmod u+x *.sh *.pl; \
	./build.sh

# shamelessly ripped from info make, with tweaks
.deps/%.d: %.c
	mkdir -p `dirname $@`; \
	$(CC) -M $(XCPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.deps/%.dpp: %.cpp
	mkdir -p `dirname $@`; \
	$(CXX) -M $(XCPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.deps/meta: $(SOURCEDEPS)
	mkdir -p .deps; \
	cat $(SOURCEDEPS) > .deps/meta

include .deps/meta
#include $($(wildcard *.c):.c=.d) \
#		$($(wildcard *.cpp):.cpp=.dpp)

openc2e: $(OPENC2E)
	$(CXX) $(XLDFLAGS) $(XCXXFLAGS) -o $@ $^

tools/filetests: tools/filetests.o genomeFile.o streamutils.o Catalogue.o
	$(CXX) $(XLDFLAGS) $(XCXXFLAGS) -o $@ $^

tools/praydumper: tools/praydumper.o pray.o
	$(CXX) $(XLDFLAGS) $(XCXXFLAGS) -o $@ $^

clean:
	rm -f *.o openc2e filetests praydumper tools/*.o
	rm -rf .deps

.PHONY: clean all dep
