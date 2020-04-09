/*
 *  main.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed 02 Jun 2004.
 *  Copyright (c) 2004-2008 Alyssa Milburn. All rights reserved.
 *  Copyright (c) 2005-2008 Bryan Donlan. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "openc2e.h"
#include "version.h"
#include <iostream>
#include <memory>
#include "Engine.h"
#include "backends/SDLBackend.h"
#ifdef SDLMIXER_SUPPORT
#include "backends/SDLMixerBackend.h"
#endif
#ifdef QT_SUPPORT
#include "backends/qtgui/QtBackend.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

// SDL tries stealing main on some platforms, which we don't want.
#undef main

extern "C" int main(int argc, char *argv[]) {
	try {
		std::string version;
#ifdef DEV_BUILD
		version = "development build";
#else
		version = RELEASE_VERSION;
#endif
		std::cout << "openc2e (" << version << "), built " __DATE__ " " __TIME__ "\nCopyright (c) 2004-2008 "
			"Alyssa Milburn and others\n\n";

		engine.addPossibleBackend("sdl", shared_ptr<Backend>(new SDLBackend()));
#ifdef QT_SUPPORT
		std::shared_ptr<QtBackend> qtbackend = std::shared_ptr<QtBackend>(new QtBackend());
		std::shared_ptr<Backend> qtbackend_generic = std::dynamic_pointer_cast<class Backend, class QtBackend>(qtbackend);
		engine.addPossibleBackend("qt", qtbackend_generic); // last-added backend is default
#endif
#ifdef SDLMIXER_SUPPORT
		engine.addPossibleAudioBackend("sdlmixer", shared_ptr<AudioBackend>(new SDLMixerBackend()));
#endif

		// pass command-line flags to the engine, but do no other setup
		if (!engine.parseCommandLine(argc, argv)) return 1;
		
		// get the engine to do all the startup (read catalogue, loading world, etc)
		if (!engine.initialSetup()) return 0;
	
		int ret = engine.backend->run(argc, argv);
		
		// we're done, be sure to shut stuff down
		engine.shutdown();

		return ret;
	} catch (std::exception &e) {
#ifdef _WIN32
		MessageBox(NULL, e.what(), "openc2e - Fatal exception encountered:", MB_ICONERROR);
#else
		std::cerr << "Fatal exception encountered: " << e.what() << "\n";
#endif
		return 1;
	}
}

/* vim: set noet: */
