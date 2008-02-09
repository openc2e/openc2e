#include "qtopenc2e.h"
#include "../Engine.h"
#include "QtBackend.h"
#ifdef OPENAL_SUPPORT
#include "../OpenALBackend.h"
#endif

#include <QApplication>

#ifdef _WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
	try {
		std::cout << "openc2e (development build), built " __DATE__ " " __TIME__ "\nCopyright (c) 2004-2008 Alyssa Milburn and others\n\n";

		boost::shared_ptr<QtBackend> qtbackend = boost::shared_ptr<QtBackend>(new QtBackend());
		boost::shared_ptr<Backend> qtbackend_generic = boost::dynamic_pointer_cast<class Backend, class QtBackend>(qtbackend);

		engine.addPossibleBackend("sdl", shared_ptr<Backend>(new SDLBackend()));
		engine.addPossibleBackend("qt", qtbackend_generic); // last-added backend is default
#ifdef OPENAL_SUPPORT
		engine.addPossibleAudioBackend("openal", shared_ptr<AudioBackend>(new OpenALBackend()));
#endif

		// pass command-line flags to the engine, but do no other setup
		if (!engine.parseCommandLine(argc, argv)) return 1;

		// get the engine to do all the startup (read catalogue, loading world, etc)
		if (!engine.initialSetup()) return 0;
	
		// TODO: this backend-specific code shouldn't really be here
		if (engine.backend == qtbackend_generic) {
			QApplication app(argc, argv);

			QtOpenc2e myvat(qtbackend);
			myvat.show();

			app.exec();
		} else {
			// you *must* call this at least once before drawing, for initial creation of the window
			engine.backend->resize(800, 600);

			while (!engine.done) {
				if (!engine.tick()) // if the engine didn't need an update..
					SDL_Delay(10); // .. delay for a short while
			} // main loop
		}

		// we're done, be sure to shut stuff down
		engine.shutdown();
	} catch (std::exception &e) {
#ifdef _WIN32
		MessageBox(NULL, e.what(), "openc2e - Fatal exception encountered:", MB_ICONERROR);
#else
		std::cerr << "Fatal exception encountered: " << e.what() << "\n";
#endif
		return 1;
	}

	return 0;
}

