/*
 *  s_physics.h
 *  openc2e
 *
 *  Created by Bryan Donlan on Sun 21 May 2006
 *  Copyright (c) 2006 Bryan Donlan. All rights reserved.
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

#ifndef S_PHYSICS_H
#define S_PHYSICS_H 1

#include "physics.h"
#include "serialization.h"

SERIALIZE(Point) {
    ar & obj.x & obj.y;
}

SAVE(Line) {
    ar & obj.start & obj.end;
}

LOAD(Line) {
    Point start, end;
    ar & start & end;
    obj = Line(start, end);
}

SERIALIZE(Vector) {
    ar & obj.x & obj.y;
}

#endif

