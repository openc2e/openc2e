/*
 *  Map.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Tue May 25 2004.
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

#include "Map.h"

void Map::Reset() {
	// todo
}

void Map::SetMapDimensions(unsigned int w, unsigned int h) {
	// todo: check for outlying metarooms
	width = w;
	height = h;
}

void Map::SetCurrentMetaRoom(unsigned int m) {
	// todo
}

int Map::addMetaRoom(MetaRoom *m) {
	metarooms.push_back(m);
	return (metarooms.size() - 1);
}

MetaRoom *Map::getMetaRoom(unsigned int room) {
	return metarooms[room];
}
