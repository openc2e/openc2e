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
#include "OpenALBackend.h"

#if defined(_MSC_VER) && defined(_DEBUG)
#undef main // because SDL is stupid
#endif

#ifdef _WIN32
#include <shlobj.h>
#endif

extern "C" int main(int argc, char *argv[]) {
	try {
		std::cout << "openc2e (development build), built " __DATE__ " " __TIME__ "\nCopyright (c) 2004-2008 Alyssa Milburn and others\n\n";

		engine.addPossibleBackend("sdl", shared_ptr<Backend>(new SDLBackend()));
#ifdef OPENAL_SUPPORT
		engine.addPossibleAudioBackend("openal", shared_ptr<AudioBackend>(new OpenALBackend()));
#endif

		// pass command-line flags to the engine, but do no other setup
		if (!engine.parseCommandLine(argc, argv)) return 1;
		
		// get the engine to do all the startup (read catalogue, loading world, etc)
		if (!engine.initialSetup()) return 0;
	
		// do a first-pass draw of the world. TODO: correct?
		world.drawWorld();

		while (!engine.done) {
			if (!engine.tick()) // if the engine didn't need an update..
				SDL_Delay(10); // .. delay for a short while
		} // main loop

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

/* vim: set noet: */
