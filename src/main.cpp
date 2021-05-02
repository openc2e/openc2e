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

#include "Engine.h"
#include "backends/SDLBackend.h"
#include "backtrace.h"
#include "version.h"

#include <iostream>
#include <memory>
#ifdef SDLMIXER_SUPPORT
#include "audiobackend/SDLMixerBackend.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

// SDL tries stealing main on some platforms, which we don't want.
#undef main

extern "C" int main(int argc, char* argv[]) {
	install_backtrace_printer();

	std::string version;
#ifdef DEV_BUILD
	version = "development build";
#else
	version = RELEASE_VERSION;
#endif
	std::cout << "openc2e (" << version << "), built " __DATE__ " " __TIME__ "\nCopyright (c) 2004-2008 "
										   "Alyssa Milburn and others\n\n";

	engine.addPossibleBackend("sdl", std::shared_ptr<Backend>(new SDLBackend()));
#ifdef SDLMIXER_SUPPORT
	engine.addPossibleAudioBackend("sdlmixer", SDLMixerBackend::getInstance());
#endif

	// pass command-line flags to the engine, but do no other setup
	if (!engine.parseCommandLine(argc, argv))
		return 1;

	// get the engine to do all the startup (read catalogue, loading world, etc)
	if (!engine.initialSetup())
		return 0;

	int ret = engine.backend->run();

	// we're done, be sure to shut stuff down
	engine.shutdown();

	return ret;
}

/* vim: set noet: */
