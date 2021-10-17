SRCS = BrainDlgContainer.cpp \
	BrainViewport.cpp \
	Edit.cpp \
	EnginePatch.cpp \
	GeneAgeDlg.cpp \
	GraphDlg.cpp \
	LobeGraphDlg.cpp \
	LobeInputDlg.cpp \
	LobeNames.cpp \
	MainFrame.cpp \
	SVCaptions.cpp \
	SVRuleDlg.cpp \
	TextViewport.cpp \
	ThreshHoldDlg.cpp \
	Tips.cpp \
	VarDlg.cpp \
	VarGraphDlg.cpp \
	Viewport.cpp \
	WinMain.cpp \
	Wnd.cpp \
	../../engine/Creature/Brain/Brain.cpp \
	../../engine/Creature/Brain/BrainComponent.cpp \
	../../engine/Creature/Brain/Dendrite.cpp \
	../../engine/Creature/Faculty.cpp \
	../../engine/Creature/Brain/Instinct.cpp \
	../../engine/Creature/Brain/Lobe.cpp \
	../../engine/Creature/Brain/Neuron.cpp \
	../../engine/Creature/Brain/SVRule.cpp \
	../../engine/Creature/Brain/Tract.cpp \
	../../engine/Creature/Genome.cpp \
	../../engine/Creature/Brain/BrainScriptFunctions.cpp \
	../../engine/C2eServices.cpp \
	../../engine/Caos/CAOSVar.cpp \
	../../common/Configurator.cpp \
	../../engine/CreaturesArchive.cpp \
	../../engine/unix/File.cpp \
	../../common/FileFuncs.cpp \
	../../engine/FlightRecorder.cpp \
	../../engine/Maths.cpp \
	../../engine/PersistentObject.cpp \
	../../common/Catalogue.cpp \
	../../common/clientside.cpp \
	../../common/FileLocaliser.cpp \
	../../common/GameInterface.cpp \
	../../common/Grapher.cpp \
	../../common/SimpleLexer.cpp \
	../../common/WhichEngine.cpp \
	../../engine/Agents/AgentHandle.cpp

OBJS = $(patsubst %.cpp,%.vatkito,$(SRCS) )

# We turn of warnings with -w - too many about NULL pointers to care
CFLAGS = -I/usr/local/include/wine -DSSTREAM_INLINE -DWINELIB -DVAT_KIT \
		-DC2E_SDL -DC2E_MODULE_NO -g -w
LIBS = -lwine -lwinmm -lwineps -lz -lcomctl32 -lcomdlg32

vatkit: $(OBJS)
	g++ $(CFLAGS) $(LIBS) -o vatkit $(OBJS)

clean:
	rm -fr $(OBJS)

%.vatkito : %.cpp
	g++ $(CFLAGS) -o $@ -c $<

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)

