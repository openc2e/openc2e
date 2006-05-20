/*
 *  PointerAgent.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue Aug 30 2005.
 *  Copyright (c) 2005 Alyssa Milburn. All rights reserved.
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

#include "SimpleAgent.h"

#ifndef _C2E_POINTERAGENT_H
#define _C2E_POINTERAGENT_H

class PointerAgent : public SimpleAgent {
public:
	std::string name;
	bool handle_events;

	PointerAgent(std::string spritefile);
	void finishInit();
	void firePointerScript(unsigned short event, Agent *src);
	void physicsTick();
};

#endif
/* vim: set noet: */
