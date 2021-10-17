#CFLAGS = -g `gtk-config --cflags`
CFLAGS = -O3 `gtk-config --cflags`
LIBS = `gtk-config --libs`

imageconvert: Convert.cpp Main.cpp ProgressDlg.cpp
	g++ -o imageconvert Convert.cpp Main.cpp ProgressDlg.cpp $(CFLAGS) $(LIBS)

