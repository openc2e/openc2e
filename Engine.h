/*
 *  Engine.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Nov 28 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

#ifndef _ENGINE_H
#define _ENGINE_H

#include "caosVar.h"
#include "Backend.h"
#include <map>

class Engine {
private:
	unsigned int tickdata;
	unsigned int ticktimes[10];
	unsigned int ticktimeptr;
	unsigned int lasttimestamp;

public:
	std::map<caosVar, caosVar, caosVarCompare> eame_variables; // non-serialised
	
	Backend *backend;
	bool done;
	unsigned int version;

	Engine();
	void setBackend(Backend *b);
	std::string executeNetwork(std::string in);
	void tick();
	void handleKeyboardScrolling();
	void processEvents();
	void handleResizedWindow(SomeEvent &event);
	void handleMouseMove(SomeEvent &event);
	void handleMouseButton(SomeEvent &event);
	void handleKeyDown(SomeEvent &event);
	void handleSpecialKeyDown(SomeEvent &event);
};

extern Engine engine;
#endif
/* vim: set noet: */
