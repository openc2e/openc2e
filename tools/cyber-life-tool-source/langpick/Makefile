CFLAGS = `gtk-config --cflags` -g
LIBS = `gtk-config --libs`

SRCS = langpick.cpp PickWindow.cpp ../../common/SimpleLexer.cpp


langpick: $(SRCS)
	g++ -o langpick $(SRCS) $(CFLAGS) $(LIBS)

