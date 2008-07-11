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
#include "openc2e.h"
#include "Room.h"
#include "MetaRoom.h"
#include <iostream>
#include "Engine.h"

void Map::Reset() {
	for (std::vector<MetaRoom *>::iterator i = metarooms.begin(); i != metarooms.end(); i++) {
		delete *i;
	}
	metarooms.clear();
	// todo: metarooms should be responsible for deleting rooms, so use the following instead of clear:
	// assert(rooms.empty());
	rooms.clear();
}

void Map::SetMapDimensions(unsigned int w, unsigned int h) {
	// todo: check for outlying metarooms
	width = w;
	height = h;
}

int Map::addMetaRoom(MetaRoom *m) {
	// todo: check if it's outlying
	while (getMetaRoom(metaroom_base))
		metaroom_base++;
	m->id = metaroom_base++;
	metarooms.push_back(m);
	return m->id;
}

MetaRoom *Map::getMetaRoom(unsigned int room) {
	for (std::vector<MetaRoom *>::iterator i = metarooms.begin(); i != metarooms.end(); i++)
		if ((*i)->id == room)
			return *i;
	return 0;
}

shared_ptr<Room> Map::getRoom(unsigned int r) {
	for (std::vector<shared_ptr<Room> >::iterator i = rooms.begin(); i != rooms.end(); i++)
		if ((*i)->id == r)
			return *i;
	return shared_ptr<Room>();
}

unsigned int Map::getMetaRoomCount() {
	return metarooms.size();
}

unsigned int Map::getRoomCount() {
	return rooms.size();
}

void Map::tick() {
	if (engine.version < 3) return; // TODO: tick rooms in C2

	// Three passes..
	for (std::vector<MetaRoom *>::iterator m = metarooms.begin(); m != metarooms.end(); m++)
		for (std::vector<shared_ptr<Room> >::iterator i = (*m)->rooms.begin(); i != (*m)->rooms.end(); i++)
			(*i)->tick();
	for (std::vector<MetaRoom *>::iterator m = metarooms.begin(); m != metarooms.end(); m++)
		for (std::vector<shared_ptr<Room> >::iterator i = (*m)->rooms.begin(); i != (*m)->rooms.end(); i++)
			(*i)->postTick();
	for (std::vector<MetaRoom *>::iterator m = metarooms.begin(); m != metarooms.end(); m++)
		for (std::vector<shared_ptr<Room> >::iterator i = (*m)->rooms.begin(); i != (*m)->rooms.end(); i++)
			(*i)->resetTick();
}

MetaRoom *Map::metaRoomAt(unsigned int _x, unsigned int _y) {
	for (std::vector<MetaRoom *>::iterator i = metarooms.begin(); i != metarooms.end(); i++) {
		MetaRoom *r = *i;
		if ((_x >= r->x()) && (_y >= r->y()))
			if ((_x <= (r->x() + r->width())) && (_y <= (r->y() + r->height())))
				return r;
	}
	return 0;
}

shared_ptr<Room> Map::roomAt(float _x, float _y) {
	MetaRoom *m = metaRoomAt((unsigned int)_x, (unsigned int)_y); // TODO: good casts?
	if (!m) return shared_ptr<Room>();
	return m->roomAt(_x, _y);
}

std::vector<shared_ptr<Room> > Map::roomsAt(float _x, float _y) {
	MetaRoom *m = metaRoomAt((unsigned int)_x, (unsigned int)_y); // TODO: good casts?
	if (!m) return std::vector<shared_ptr<Room> >();
	return m->roomsAt(_x, _y);
}

bool Map::collideLineWithRoomSystem(Point src, Point dest, shared_ptr<Room> &room, Point &where, Line &wall, unsigned int &walldir, int perm) {
	shared_ptr<Room> newRoom;

	where = src;
	
	while (true) {
		if (!collideLineWithRoomBoundaries(where, dest, room, newRoom, where, wall, walldir, perm))
			return false; // failure
		if (!newRoom)
			return true; // collision
		if (where == dest)
			return false; // got there
		
		assert(newRoom != room); // tsk
		
		room = newRoom;
	}
}

/*
 * poss. optimisation: skip checking the rest of the lines if our distance is 0?
 */
bool Map::collideLineWithRoomBoundaries(Point src, Point dest, shared_ptr<Room> room, shared_ptr<Room> &newroom, Point &where, Line &wall, unsigned int &walldir, int perm) {
	assert(room);
	// TODO: this assert fails. why? 'where' is presumably outside the dest room sometimes.. mmh
	//assert(room->containsPoint(src.x, src.y));
	if (src == dest) return false;

	newroom.reset();

	/* if (room->containsPoint(dest.x, dest.y)) {
		where = dest;
		return false;
	}*/

	float distance = 100000000.0f; // TODO: lots.
	bool foundsomething = false;
	bool previousroom = (newroom);
	Point oldpoint = where;
	Line movement(src, dest);

	Line x[4] = { room->left, room->right, room->top, room->bot };
	for (unsigned int i = 0; i < 4; i++) {
		/*if (previousroom)
			if (x[i].containsPoint(oldpoint)) continue; */

		Point temppoint;
		if (x[i].intersect(movement, temppoint)) {
			if (!movement.containsPoint(temppoint)) {
				std::cout << "physics bug: intersect wasn't on movement line at (" << temppoint.x << ", " << temppoint.y << ")" << std::endl;
				std::cout << "room line: (" << x[i].getStart().x << ", " << x[i].getStart().y << ") to ";
				std::cout << "(" << x[i].getEnd().x << ", " << x[i].getEnd().y << ")" << std::endl;
				std::cout << "movement line: (" << src.x << ", " << src.y << ") to ";
				std::cout << "(" << dest.x << ", " << dest.y << ")" << std::endl;
				return false; // go away
			}
			assert(movement.containsPoint(temppoint));

			//if (temppoint == src) return false; // for debug use: sneakily fail all movement between rooms

			// see if this is nearer than any previous points we've found
			double distx = temppoint.x - src.x;
			double disty = temppoint.y - src.y;
			double d = distx*distx + disty*disty;
			if (d > distance)
				continue;
		
			// work out which room is next along our movement vector
			// TODO: this code utterly sucks, doesn't work properly
			float newx, newy;
			/*if (temppoint == src) { // TODO: this is not an accurate check!! likely cause of falling through PERM
				// we might be on a PERM line! check backwards.
				newx = temppoint.x + (src.x <= dest.x ? (src.x == dest.x ? 0.0 : -0.5) : 0.5);
				newy = temppoint.y + (src.y <= dest.y ? (src.y == dest.y ? 0.0 : -0.5) : 0.5);
			} else {*/
				newx = temppoint.x + (src.x <= dest.x ? (src.x == dest.x ? 0.0 : 0.5) : -0.5);
				newy = temppoint.y + (src.y <= dest.y ? (src.y == dest.y ? 0.0 : 0.5) : -0.5);
			//}

			if (room->containsPoint(newx, newy)) { // if a little along our movement vector is still in our room, forget it
				/*std::cout << "physics debug: next room is original room at (" << temppoint.x << ", " << temppoint.y << ")" << std::endl;
				std::cout << "room line: (" << x[i].getStart().x << ", " << x[i].getStart().y << ") to ";
				std::cout << "(" << x[i].getEnd().x << ", " << x[i].getEnd().y << ")" << std::endl;
				std::cout << "movement line: (" << movement.getStart().x << ", " << movement.getStart().y << ") to ";
				std::cout << "(" << movement.getEnd().x << ", " << movement.getEnd().y << ")" << std::endl;
				return false;*/
				continue; // continue, bad collision
			}

			shared_ptr<Room> nextroom;
			bool foundroom = false;

			for (std::map<boost::weak_ptr<Room>,RoomDoor *>::iterator r = room->doors.begin(); r != room->doors.end(); r++) {
				shared_ptr<Room> otherroom = r->first.lock();
				assert(otherroom);
				assert(r->second);
				if (otherroom->containsPoint(newx, newy)) {
					// this is our next room!
					foundroom = true;
					// if boundary perm is going to let us through..
					if (perm <= r->second->perm) // TODO: right?
						nextroom = otherroom;
					break;
				}
			}
			
			if (!foundroom && !nextroom)
				nextroom = roomAt(newx, newy);

			/*if (temppoint == src) { // if we're just doing the backwards PERM check
				if (!foundroom || nextroom != 0) {
					// either there's no previous room or PERM let us into the previous room..
					// ..so forget this collision
					break;
				}
			}*/
		
			shared_ptr<Room> z = roomAt(temppoint.x, temppoint.y); // TODO: evil performance-killing debug check
			if (!z) {
				// TODO: commented out this error message for sake of fuzzie's sanity, but it's still an issue
				/*std::cout << "physics bug: fell out of room system at (" << where.x << ", " << where.y << ")" << std::endl;
				std::cout << "room line: (" << x[i].getStart().x << ", " << x[i].getStart().y << ") to ";
				std::cout << "(" << x[i].getEnd().x << ", " << x[i].getEnd().y << ")" << std::endl;
				std::cout << "movement line: (" << movement.getStart().x << ", " << movement.getStart().y << ") to ";
				std::cout << "(" << movement.getEnd().x << ", " << movement.getEnd().y << ")" << std::endl;*/
				return false; // go away
			}

			// it is nearer and not the same room, so make it our priority
			distance = d;
			foundsomething = true;
			where = temppoint;
			wall = x[i];
			walldir = i;
			newroom = nextroom;
		}
	}

	if (!foundsomething) {
		if (room->containsPoint(dest.x, dest.y)) {
			where = dest;
		} else {
			where = src; // TODO: can't we assume this anyway?
			// TODO: commented out this error message for sake of fuzzie's sanity, but it's still an issue
			/*std::cout << "physics bug: didn't collide with a line, nor stay in the current room!" << std::endl;
			std::cout << "movement line: (" << movement.getStart().x << ", " << movement.getStart().y << ") to ";
			std::cout << "(" << movement.getEnd().x << ", " << movement.getEnd().y << ")" << std::endl;*/
		}
		return false;
	}

	return true;
}
	
/* vim: set noet: */
