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
#include <iostream>
#include "Engine.h"
#include "World.h"
#include "SDLBackend.h"
#include "NullBackend.h"
#ifdef OPENAL_SUPPORT
#include "OpenALBackend.h"
#endif
#ifdef QT_SUPPORT
#include "qtopenc2e.h"
#include "QtBackend.h"
#endif

#ifdef _WIN32
#include <shlobj.h>
#endif

extern "C" int main(int argc, char *argv[]) {
	try {
		std::cout << "openc2e (development build), built " __DATE__ " " __TIME__ "\nCopyright (c) 2004-2008 Alyssa Milburn and others\n\n";

		engine.addPossibleBackend("sdl", shared_ptr<Backend>(new SDLBackend()));
#ifdef QT_SUPPORT
		boost::shared_ptr<QtBackend> qtbackend = boost::shared_ptr<QtBackend>(new QtBackend());
		boost::shared_ptr<Backend> qtbackend_generic = boost::dynamic_pointer_cast<class Backend, class QtBackend>(qtbackend);
		engine.addPossibleBackend("qt", qtbackend_generic); // last-added backend is default
#endif
#ifdef OPENAL_SUPPORT
		engine.addPossibleAudioBackend("openal", shared_ptr<AudioBackend>(new OpenALBackend()));
#endif

		// pass command-line flags to the engine, but do no other setup
		if (!engine.parseCommandLine(argc, argv)) return 1;
		
		// get the engine to do all the startup (read catalogue, loading world, etc)
		if (!engine.initialSetup()) return 0;
	
		int ret = engine.backend->main(argc, argv);
		
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
