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
#include "common/backtrace.h"
#include "sdlbackend/SDLBackend.h"
#include "sdlbackend/SDLMixerBackend.h"
#include "version.h"

#include <fmt/core.h>
#include <memory>

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
	fmt::print("openc2e ({}), built " __DATE__ " " __TIME__ "\nCopyright (c) 2004-2008 Alyssa Milburn and others\n\n", version);

	engine.addPossibleBackend("sdl", SDLBackend::get_instance());
	engine.addPossibleAudioBackend("sdlmixer", SDLMixerBackend::getInstance());

	// pass command-line flags to the engine, but do no other setup
	if (!engine.parseCommandLine(argc, argv))
		return 1;

	// get the engine to do all the startup (read catalogue, loading world, etc)
	if (!engine.initialSetup())
		return 0;

	// run
	while (true) {
		get_backend()->waitForNextDraw();
		if (engine.done) {
			break;
		}

		engine.tick();
		engine.drawWorld();

		get_backend()->drawDone();
	}

	// we're done, be sure to shut stuff down
	engine.shutdown();

	return 0;
}

/* vim: set noet: */
