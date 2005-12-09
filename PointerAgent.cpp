/*
 *  PointerAgent.cpp
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

#include "PointerAgent.h"
#include "c16Image.h"
#include "openc2e.h"

// TODO: change imagecount?
PointerAgent::PointerAgent(std::string spritefile) : SimpleAgent(2, 1, 1, UINT_MAX, spritefile, 0, 0) {
	name = "hand";
	handle_events = false;
}

/* vim: set noet: */
