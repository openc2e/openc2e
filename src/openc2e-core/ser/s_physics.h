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
	ar& obj.x& obj.y;
}

BOOST_CLASS_IMPLEMENTATION(Point, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(Point, boost::serialization::track_never);
SAVE(Line) {
	ar& obj.start& obj.end;
}

LOAD(Line) {
	Point start, end;
	ar& start& end;
	obj = Line(start, end);
}
BOOST_CLASS_IMPLEMENTATION(Line, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(Line, boost::serialization::track_never);

SERIALIZE(Vector<float>) {
	ar& obj.x& obj.y;
}

SERIALIZE(Vector<double>) {
	ar& obj.x& obj.y;
}
BOOST_CLASS_IMPLEMENTATION(Vector<float>, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(Vector<float>, boost::serialization::track_never);
BOOST_CLASS_IMPLEMENTATION(Vector<double>, boost::serialization::object_serializable);
BOOST_CLASS_TRACKING(Vector<double>, boost::serialization::track_never);


#endif
