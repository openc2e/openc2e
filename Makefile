SOURCES = $(wildcard *.c) $(wildcard *.cpp) 
HEADERS = $(wildcard *.h)
EVERYTHING = $(SOURCES) $(HEADERS)

SOURCEDEPS = $(patsubst %.cpp,.deps/%.dpp,$(wildcard *.cpp)) \
			 $(patsubst %.c,.deps/%.d,$(wildcard *.c))

OPENC2E = \
	Agent.o \
	attFile.o \
	blkImage.o \
	c16Image.o \
	Camera.o \
	caosdata.o \
	caoshashes.o \
	caosScript.o \
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
	streamutils.o \
	Vehicle.o \
	World.o

LDFLAGS=-lboost_filesystem $(shell sdl-config --static-libs) -lz
CFLAGS=-ggdb3 $(shell sdl-config --cflags)
CPPFLAGS=$(CFLAGS)

all: openc2e filetests praydumper

#.deps/%.d: %.c .deps
#	$(CC) -M $(CFLAGS) -o "$@.tmp" $<
#	(cat "$@.tmp"; echo; sed "s|$<|$@|g" < "$@.tmp") > $@

#.deps/%.dpp: %.cpp .deps
#	$(CC) -M $(CFLAGS) -o "$@.tmp" $<
#	(cat "$@.tmp"; echo; sed "s|$<|$@|g" < "$@.tmp") > $@

# shamelessly ripped from info make, with tweaks
.deps/%.d: %.c
	mkdir -p .deps; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.deps/%.dpp: %.cpp
	mkdir -p .deps; \
	$(CXX) -M $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

.deps/meta: $(SOURCEDEPS)
	mkdir -p .deps; \
	cat $(SOURCEDEPS) > .deps/meta

include .deps/meta
#include $($(wildcard *.c):.c=.d) \
#		$($(wildcard *.cpp):.cpp=.dpp)

openc2e: $(OPENC2E)
	g++ $(LDFLAGS) $(CXXFLAGS) -o $@ $^

filetests: filetests.o genomeFile.o streamutils.o Catalogue.o
	g++ $(LDFLAGS) $(CXXFLAGS) -o $@ $^

praydumper: praydumper.o pray.o
	g++ $(LDFLAGS) $(CXXFLAGS) -o $@ $^

clean:
	rm -f *.o openc2e filetests praydumper
	rm -rf .deps


.PHONY: clean all dep
