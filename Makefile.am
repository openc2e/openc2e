bin_PROGRAMS = openc2e

noinst_PROGRAMS = filetests praydumper gctest mngtest pathtest

noinst_LTLIBRARIES = libopenc2e_core.la libopenc2e_mng.la

libopenc2e_core_la_SOURCES = \
	mmapifstream.cpp

libopenc2e_core_la_LDFLAGS = -version-info 1:0:0

libopenc2e_mng_la_SOURCES = \
	mngfile.cpp mngparser.tab.cpp lex.mng.cpp

libopenc2e_mng_la_LDFLAGS = -version-info 1:0:0

filetests_SOURCES = \
	tools/filetests.cpp genomeFile.cpp streamutils.cpp Catalogue.cpp

praydumper_SOURCES = \
	tools/praydumper.cpp pray.cpp

# XXX: put output file in tools/
gctest_SOURCES = \
				 tools/gctest.cpp gc.cpp
mngtest_SOURCES = \
	tools/mngtest.cpp
mngtest_LDADD = libopenc2e_mng.la

pathtest_SOURCES = \
	tools/pathtest.cpp PathResolver.cpp

VMFILES = \
	caosVM_agent.cpp \
	caosVM_camera.cpp \
	caosVM_compound.cpp \
	caosVM_core.cpp \
	caosVM_creatures.cpp \
	caosVM_debug.cpp \
	caosVM_files.cpp \
	caosVM_flow.cpp \
	caosVM_input.cpp \
	caosVM_net.cpp \
	caosVM_map.cpp \
	caosVM_motion.cpp \
	caosVM_ports.cpp \
	caosVM_resources.cpp \
	caosVM_scripts.cpp \
	caosVM_sounds.cpp \
	caosVM_time.cpp \
	caosVM_world.cpp \
	caosVM_variables.cpp \
	caosVM_vehicles.cpp

#openc2e_DATA = docs.html

openc2e_LDADD = libopenc2e_core.la libopenc2e_mng.la

openc2e_SOURCES = $(VMFILES) \
	Agent.cpp \
	AgentRef.cpp \
	attFile.cpp \
	blkImage.cpp \
	c16Image.cpp \
	Camera.cpp \
	caosScript.cpp \
	caosVar.cpp \
	caosVM.cpp \
	Catalogue.cpp \
	cmddata.cpp \
	CompoundAgent.cpp \
	Creature.cpp \
	creaturesImage.cpp \
	dialect.cpp \
	fileSwapper.cpp \
	gc.cpp \
	genomeFile.cpp \
	lex.yy.cpp \
	lexutil.cpp \
	main.cpp \
	Map.cpp \
	MetaRoom.cpp \
	PathResolver.cpp \
	physics.cpp \
	PointerAgent.cpp \
	pray.cpp \
	Room.cpp \
	Scriptorium.cpp \
	SDLBackend.cpp \
	SDL_gfxPrimitives.c \
	SimpleAgent.cpp \
	SkeletalCreature.cpp \
	streamutils.cpp \
	Vehicle.cpp \
	World.cpp

cmddata.cpp: commandinfo.yml writecmds.pl
	perl writecmds.pl commandinfo.yml > cmddata.cpp || rm -f cmddata.cpp

commandinfo.yml: $(VMFILES) parsedocs.pl
	perl parsedocs.pl $(VMFILES) > commandinfo.yml || rm -f commandinfo.yml

docs.html: writehtml.pl commandinfo.yml
	perl writehtml.pl > docs.html || rm -f docs.html

# Automake doesn't seem to support C++ lexers
lex.yy.cpp lex.yy.h: caos.l
	flex -+ -d -o lex.yy.cpp --header-file=lex.yy.h caos.l
mngparser.tab.cpp mngparser.tab.hpp: mngparser.ypp
	bison -d --name-prefix=mng mngparser.ypp
lex.mng.cpp lex.mng.h: mng.l mngparser.tab.hpp
	flex -+ --prefix=mng -d -o lex.mng.cpp --header-file=lex.mng.h mng.l

caosScript.o: lex.yy.h
