/*
 *  s_metaroom.h
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

#ifndef S_METAROOM_H
#define S_METAROOM_H 1

#include "ser/s_room.h"
#include "MetaRoom.h"
#include "serialization.h"

SAVE(MetaRoom) {
    ar & obj.xloc & obj.yloc & obj.wid & obj.hei;
    if (obj.background)
        ar & obj.background->name;
    else {
        std::string s("");
        ar & s;
    }
    ar & obj.rooms & obj.id;
}

LOAD(MetaRoom) {
    ar & obj.xloc & obj.yloc & obj.wid & obj.hei;
    std::string backname;
    ar & backname;

    obj.setup(obj.xloc, obj.yloc, obj.wid, obj.hei, backname);
    ar & obj.rooms & obj.id;
}

#endif

