/*
 *  openc2e.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Thu Jun 03 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
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

#include <iostream>

// assertFailure segfaults the app
class assertFailure { public: assertFailure() { char *n = 0; *n = 0; } };

#define assert(x) if (!(x)) { std::cerr << "openc2e assert failed: " << #x << "\n"; throw assertFailure(); }
