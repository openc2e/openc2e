#include "../World.h" // TODO: not needed?
#include "qtopenc2e.h"
#include "../Engine.h"
#ifdef _WIN32
#include "../SDLBackend.h"
#include <windows.h>
#endif

#include <QApplication>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
	try {
		std::cout << "openc2e (development build), built " __DATE__ " " __TIME__ "\nCopyright (c) 2004-2008 Alyssa Milburn and others\n\n";

		QApplication app(argc, argv);

		if (!engine.parseCommandLine(argc, argv)) return 1;

		QtOpenc2e myvat;	
		myvat.show();

		return app.exec();
	} catch (std::exception &e) {
#ifdef _WIN32
		MessageBox(NULL, e.what(), "openc2e - Fatal exception encountered:", MB_ICONERROR);
#else
		std::cerr << "Fatal exception encountered: " << e.what() << "\n";
#endif
		return 1;
	}
}

