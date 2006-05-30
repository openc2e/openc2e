/*
 *  s_room.h
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

#ifndef S_ROOM_H
#define S_ROOM_H 1

#include "Room.h"
#include "ser/s_physics.h"
#include "ser/s_metaroom.h"
#include "serialization.h"
#include <boost/serialization/set.hpp>
#include <iostream>
#include <cassert>

SERIALIZE(RoomDoor) {
    ar & obj.first & obj.second & obj.perm;
}

SERIALIZE(Room) {
    ar & obj.doors & obj.nearby;
    ar & obj.x_left & obj.x_right;
    ar & obj.y_left_ceiling & obj.y_right_ceiling;
    ar & obj.y_left_floor & obj.y_right_floor;

    ar & obj.left & obj.right & obj.top & obj.bot;

    ar & obj.type & obj.id;
    ar & obj.metaroom;
    std::string t("CA PREMARK");
    ar & t;
    assert(t == "CA PREMARK");
#if 0
    std::vector<float> hack;
    hack.resize(CA_COUNT);
    for (int i = 0; i < CA_COUNT; i++) {
/*        std::cerr << "CA loop, i=" << i << " pre=" << obj.ca[i] << std::endl;
        ar & obj.ca[i];
        std::cerr << "Post=" << obj.ca[i] << std::endl;*/
    }
#endif
    ar & obj.ca;
    std::string t2("CA POSTMARK");
    ar & t2;
    assert(t2 == "CA POSTMARK");
}

#endif

