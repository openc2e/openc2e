/*
 *  caosVM_map.cpp
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

#include "caosVM.h"
#include "World.h"
#include <assert.h>
#include <iostream>
#include <boost/format.hpp>
using std::cerr;

/**
 ADDM (integer) x (integer) y (integer) width (integer) height (integer) background (string)
 %status maybe
 
 Creates a metaroom with the given height and width, at the coordinates given.  Returns the id of the new metaroom.
 */
void caosVM::v_ADDM() {
	VM_VERIFY_SIZE(5)
	VM_PARAM_STRING(background)
	VM_PARAM_INTEGER(height)
	VM_PARAM_INTEGER(width)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	MetaRoom *r = new MetaRoom(x, y, width, height, background);
	caosVar v;
	v.setInt(world.map.addMetaRoom(r));
	result = v;
}

/**
 ADDB (command) metaroom_id (integer) background (string)
 %status stub

 Adds a new background to an existing metaroom, to be displayed with BKGD.
*/
void caosVM::c_ADDB() {
	VM_PARAM_STRING(background)
	VM_PARAM_INTEGER(metaroomid)
	
	MetaRoom *m = world.map.getMetaRoom(metaroomid);
	caos_assert(m);

	// TODO
}

/**
 BRMI (command) metaroom_base (integer) room_base (integer)
 %status maybe
 
 Sets the base ID numbers for new metarooms and rooms to the given values.
 */
void caosVM::c_BRMI() {
	VM_VERIFY_SIZE(2)
	
	VM_PARAM_INTEGER(room_base)
	VM_PARAM_INTEGER(metaroom_base)

	world.map.room_base = room_base;
	world.map.metaroom_base = metaroom_base;
}

/**
 MAPD (command) width (integer) height (integer)
 %status maybe
 
 Sets the world map dimensions, inside which metarooms are placed.
 */
void caosVM::c_MAPD() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(height)
	VM_PARAM_INTEGER(width)

	world.map.SetMapDimensions(width, height);
}

/**
 MAPW (integer)
 %status maybe
 
 Returns the width of the world map.
*/
void caosVM::v_MAPW() {
	result.setInt(world.map.getWidth());
}

/**
 MAPH (integer)
 %status maybe

 Returns the height of the world map.
*/
void caosVM::v_MAPH() {
	result.setInt(world.map.getHeight());
}

/**
 MAPK (command)  
 %status maybe
 
 Resets and empties the world map.
 */
void caosVM::c_MAPK() {
	VM_VERIFY_SIZE(0)

	world.map.Reset();
}

/**
 BKDS (string) metaroomid (integer)
 %status stub

 Determines all of the background names in use by the given metaroom, and returns them in a comma-seperated string.
*/
void caosVM::v_BKDS() {
	VM_PARAM_INTEGER(metaroomid)
	
	MetaRoom *m = world.map.getMetaRoom(metaroomid);
	caos_assert(m);

	result.setString(""); // TODO
}

/**
 ADDR (integer) metaroomid (integer) x_left (integer) x_right (integer) y_left_ceiling (integer) y_right_ceiling (integer) y_left_floor (integer) y_right_floor (integer)
 %status maybe

 Makes a new room inside the given metaroom.  Rooms can have sloped floors and ceilings, but only vertical walls.  
 The id of the new room is returned.
*/
void caosVM::v_ADDR() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(y_right_floor)
	VM_PARAM_INTEGER(y_left_floor)
	VM_PARAM_INTEGER(y_right_ceiling)
	VM_PARAM_INTEGER(y_left_ceiling)
	VM_PARAM_INTEGER(x_right)
	VM_PARAM_INTEGER(x_left)
	VM_PARAM_INTEGER(metaroomid)

	Room *r = new Room(x_left, x_right,
			y_left_ceiling, y_right_ceiling,
			y_left_floor, y_right_floor);
	MetaRoom *m = world.map.getMetaRoom(metaroomid);
	caos_assert(m);
	r->metaroom = m;
	r->id = m->addRoom(r);
	result.setInt(r->id);
}

/**
 RTYP (command) roomid (integer) roomtype (integer)
 %status maybe

 Defines the 'type' of the given room.  The types vary with different games.
*/
void caosVM::c_RTYP() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(roomtype)
	VM_PARAM_INTEGER(roomid)

	Room *room = world.map.getRoom(roomid);
	caos_assert(room);
	room->type = roomtype;
}

/**
 RTYP (integer) roomid (integer)
 %status maybe

 Returns the 'type' of the given room, or -1 if 'roomid' is invalid.
*/
void caosVM::v_RTYP() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(roomid)

	Room *room = world.map.getRoom(roomid);
	if (room)
		result.setInt(room->type);
	else
		result.setInt(-1);
}

/**
 DOOR (command) room1 (integer) room2 (integer) perm (integer)
 %status maybe

 Sets how permeable the door between the two given rooms will be. (See PERM).
*/
void caosVM::c_DOOR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(perm)
	VM_PARAM_INTEGER(room2)
	VM_PARAM_INTEGER(room1)

	Room *r1 = world.map.getRoom(room1);
	Room *r2 = world.map.getRoom(room2);
	caos_assert(r1); caos_assert(r2);
	RoomDoor *door = r1->doors[r2];
	if (!door) {
		door = new RoomDoor;
		door->first = r1;
		door->second = r2;
		door->perm = perm;
		r1->doors[r2] = door;
		r2->doors[r1] = door;
		r1->nearby.insert(r2);
		r2->nearby.insert(r1);
	} else {
		door->perm = perm;
	}
}

/**
 RATE (command) roomtype (integer) caindex (integer) gain (float) loss (float) diffusion (float)
 %status maybe

 Defines the rates of the given CA in the given room.  'gain' defines how easily the CA will be absorbed from
 agents inside the room, 'loss' defines how much will be lost into the air, and 'diffusion' defines how easily it 
 will spread to other rooms.
*/
void caosVM::c_RATE() {
	VM_VERIFY_SIZE(5)
	VM_PARAM_FLOAT(diffusion)
	VM_PARAM_FLOAT(loss)
	VM_PARAM_FLOAT(gain)
	VM_PARAM_INTEGER(caindex)
	VM_PARAM_INTEGER(roomtype)

	cainfo info;
	info.gain = gain;
	info.loss = loss;
	info.diffusion = diffusion;
	world.carates[roomtype][caindex] = info;
}

/**
 ROOM (integer) agent (agent)
 %status maybe
 
 Returns the room that contains the given agent (jugding by its center).
*/
void caosVM::v_ROOM() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(agent)
	
	Room *r = world.map.roomAt(agent->x + (agent->getWidth() / 2), agent->y + (agent->getHeight() / 2));
	if (r)
		result.setInt(r->id);
	else
		result.setInt(-1);
}

/**
 LEFT (integer)
 %status maybe
 
 Returns the left constant (0).
*/
void caosVM::v_LEFT() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(0);
}

/**
 RGHT (integer)
 %status maybe
 
 Returns the right constant (1).
*/
void caosVM::v_RGHT() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(1);
}

/**
 _UP_ (integer)
 %status maybe
 %pragma implementation caosVM::v_UP
 
 Returns the up constant (2).
*/
void caosVM::v_UP() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(2);
}

/**
 DOWN (integer)
 %status maybe
 
 Returns the down constant (3).
*/
void caosVM::v_DOWN() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(3);
}

/**
 PROP (command) roomid (integer) caindex (integer) cavalue (float)
 %status maybe

 Defines the level of the given CA in the given room.  Valid settings are between 0 and 1; if higher, it will be 
 reset to 1.
*/
void caosVM::c_PROP() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(cavalue)
	VM_PARAM_INTEGER(caindex)
	VM_PARAM_INTEGER(roomid)

	if (cavalue > 1.0f)
		cavalue = 1.0f;
	else
		caos_assert(0.0f <= cavalue);
	caos_assert(0 <= caindex && caindex <= 19);

	Room *room = world.map.getRoom(roomid);
	caos_assert(room);
	room->ca[caindex] = cavalue;
}

/**
 PROP (float) roomid (integer) caindex (integer)
 %status maybe

 Returns the level of the given CA in the given room.
*/
void caosVM::v_PROP() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(caindex)
	VM_PARAM_INTEGER(roomid)
	
	caos_assert(0 <= caindex && caindex <= 19);

	Room *room = world.map.getRoom(roomid);
	caos_assert(room);
	result.setFloat(room->ca[caindex]);
}

/**
 PERM (command) perm (integer)
 %status maybe

 Sets the TARG agent's permiability.  Valid settings are between 1 and 100.
*/
void caosVM::c_PERM() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(perm)
	
	// C3 rocklice set perm to 0, ick.
	// TODO: is perm of 0 possible?
	
	if (perm < 1) perm = 1;
	if (perm > 100) perm = 100;

	caos_assert(targ);
	targ->perm = perm;
}

/**
 PERM (integer)
 %status maybe

 Returns the TARG agent's permiability.
*/
void caosVM::v_PERM() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setInt(targ->perm);
}

/**
 GRAP (integer) x (float) y (float)
 %status maybe

 Returns the id of the room at the coordinates (x, y), or -1 if nothing's there.
*/
void caosVM::v_GRAP() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	Room *room = world.map.roomAt(x, y);
	if (room) {
		result.setInt(room->id);
	} else {
		result.setInt(-1);
	}
}

/**
 GMAP (integer) x (float) y (float)
 %status maybe

 Returns the id of the metaroom at the coordinates (x, y), or -1 if nothing's there.
*/
void caosVM::v_GMAP() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	MetaRoom *room = world.map.metaRoomAt(x, y);
	if (room) {
		result.setInt(room->id);
	} else {
		result.setInt(-1);
	}
}

/**
 LINK (command) room1 (integer) room2 (integer) perm (integer)
 %status stub

 Defines the permeability of the link between the two given rooms.  This is used for CA diffusion.
*/
void caosVM::c_LINK() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(perm)
	VM_PARAM_INTEGER(room2)
	VM_PARAM_INTEGER(room1)

	// TODO
}

/**
 GRID (integer) agent (agent) direction (integer)
 %status maybe

 Returns the nearest adjacent room to the specified agent in the given direction (one of the direction constants), or 
 -1 otherwise.
*/
void caosVM::v_GRID() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(direction)
	VM_PARAM_VALIDAGENT(agent)

	float agentx = agent->x + (agent->getWidth() / 2);
	float agenty = agent->y + (agent->getHeight() / 2);
	Room *sourceroom = world.map.roomAt(agentx, agenty);
	if (!sourceroom) {
		// (should we REALLY check for it being in the room system, here?)
		cerr << agent->identify() << " tried using GRID but isn't in the room system!\n";
		result.setInt(-1);
		return;
	}
	Room *foundroom = 0;

	if ((direction == 0) || (direction == 1)) {
		int movement = (direction == 0 ? -1 : 1);

		int x = agentx;
		while (true) {
			x += movement;
			Room *r = world.map.roomAt(x, agenty);
			if (r != sourceroom) {
				foundroom = r;
				break;
			}
		}
	} else if ((direction == 2) || (direction == 3)) {
		int movement = (direction == 2 ? -1 : 1);
	
		int y = agenty;
		while (true) {
			y += movement;
			Room *r = world.map.roomAt(agentx, y);
			if (r != sourceroom) {
				foundroom = r;
				break;
			}
		}
	} else cerr << "GRID got an unknown direction!\n";

	if (foundroom)
		result.setInt(foundroom->id);
	else
		result.setInt(-1);
}

/**
 EMIT (command) caindex (integer) amount (float)
 %status maybe

 Makes the TARG agent continually emit the specified amount of the specified CA into the room.
*/
void caosVM::c_EMIT() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(amount)
	VM_PARAM_INTEGER(caindex)
	
	caos_assert((0 <= caindex && caindex <= 19) || caindex == -1);
	caos_assert(targ);

	targ->emitca_index = caindex;
	targ->emitca_amount = amount;
}

/**
 WALL (integer)
 %status maybe

 Returns the direction of the last wall the TARG agent collided with.
*/
void caosVM::v_WALL() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setInt(targ->lastcollidedirection);
}

/**
 ALTR (command) roomid (integer) caindex (integer) delta (float)
 %status maybe

 Modifies the level of the given CA in the room specified.
 If 'roomid' is -1, the room containing the TARG agent will be used.
*/
void caosVM::c_ALTR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(delta);
	VM_PARAM_INTEGER(caindex);
	VM_PARAM_INTEGER(roomid);
	
	caos_assert(0 <= caindex && caindex <= 19);

	Room *room;
	if (roomid == -1) {
		caos_assert(targ);
		room = world.map.roomAt(targ->x + (targ->getWidth() / 2), targ->y + (targ->getHeight() / 2));
	} else
		room = world.map.getRoom(roomid);
	caos_assert(room);
	float newvalue = room->ca[caindex] + delta;
	if (newvalue < 0.0f) newvalue = 0.0f;
	else if (newvalue > 1.0f) newvalue = 1.0f;
	room->ca[caindex] = newvalue;
}

/**
 RLOC (string) roomid (integer)
 %status maybe

 Returns a string containing the location of the given room in the following format: x_left, x_right, y_left_ceiling, 
 y_right_ceiling, y_left_floor, y_right_floor.
*/
void caosVM::v_RLOC() {
	VM_PARAM_INTEGER(roomid)

	Room *r = world.map.getRoom(roomid);
	caos_assert(r);

	result.setString(boost::str(boost::format("%d %d %d %d %d %d") % r->x_left % r->x_right % r->y_left_ceiling % r->y_right_ceiling % r->y_left_floor % r->y_right_floor));
}

/**
 MLOC (string) metaroomid (integer)
 %status maybe

 Returns a string containing the location of the given metaroom in the following format: x y width height
*/
void caosVM::v_MLOC() {
	VM_PARAM_INTEGER(metaroomid)

	MetaRoom *r = world.map.getMetaRoom(metaroomid);
	caos_assert(r);

	result.setString(boost::str(boost::format("%d %d %d %d") % r->x() % r->y() % r->width() % r->height()));
}

/**
 DMAP (command) mapon (integer)
 %status maybe

 Turns the debug map on and off, which shows the edges of rooms and vehicles.
*/
void caosVM::c_DMAP() {
	VM_PARAM_INTEGER(mapon)
		
	world.showrooms = mapon;
}

/**
 ERID (string) metaroom_id (integer)
 %status maybe

 Returns a space-seperated list of all room id's contained by the given metaroom.
*/
void caosVM::v_ERID() {
	VM_PARAM_INTEGER(metaroom_id)

	std::string out;

	if (metaroom_id == -1) {
		// TODO
	} else {
		MetaRoom *r = world.map.getMetaRoom(metaroom_id);
		for (std::vector<Room *>::iterator i = r->rooms.begin(); i != r->rooms.end(); i++) {
			if (out.size() > 0) out = out + " ";
			out = out + boost::str(boost::format("%d") % (*i)->id);
		}
	}

	result.setString(out);
}

/**
 DELR (command) room_id (integer)
 %status stub

 Removes the given room from the map.
*/
void caosVM::c_DELR() {
	VM_PARAM_INTEGER(room_id)

	Room *r = world.map.getRoom(room_id);
	caos_assert(r);

	// TODO
}

/**
 DELM (command) metaroom_id (integer)
 %status stub

 Removes the given metaroom from the map.
*/
void caosVM::c_DELM() {
	VM_PARAM_INTEGER(metaroom_id)

	MetaRoom *r = world.map.getMetaRoom(metaroom_id);
	caos_assert(r);

	// TODO
}

/**
 HIRP (integer) roomid (integer) caindex (integer) direction (integer)
 %status stub
*/
void caosVM::v_HIRP() {
	VM_PARAM_INTEGER(direction)
	VM_PARAM_INTEGER(caindex) caos_assert(0 <= caindex && caindex <= 19);
	VM_PARAM_INTEGER(roomid)
	
	Room *r = world.map.getRoom(roomid);
	caos_assert(r);

	result.setInt(roomid); // TODO
}

/**
 LORP (integer) roomid (integer) caindex (integer) direction (integer)
 %status stub
*/
void caosVM::v_LORP() {
	VM_PARAM_INTEGER(direction)
	VM_PARAM_INTEGER(caindex) caos_assert(0 <= caindex && caindex <= 19);
	VM_PARAM_INTEGER(roomid)
	
	Room *r = world.map.getRoom(roomid);
	caos_assert(r);

	result.setInt(roomid); // TODO
}

/**
 TORX (float) roomid (integer)
 %status maybe
*/
void caosVM::v_TORX() {
	VM_PARAM_INTEGER(roomid)

	Room *r = world.map.getRoom(roomid);
	caos_assert(r);
	caos_assert(targ);

	float centrex = r->x_left + ((r->x_right - r->x_left) / 2);
	result.setFloat(centrex - targ->x);
}

/**
 TORY (float) roomid (integer)
 %status maybe
*/
void caosVM::v_TORY() {
	VM_PARAM_INTEGER(roomid)

	Room *r = world.map.getRoom(roomid);
	caos_assert(r);
	caos_assert(targ);

	// TODO: calculate this however c2e does it.. or at least check this is right
	float topy = (r->y_left_ceiling - r->y_right_ceiling) / 2;
	if (topy >= 0.0f) topy = r->y_left_ceiling + topy;
	else topy = r->y_right_ceiling - topy;

	float bottomy = (r->y_left_floor - r->y_right_floor) / 2;
	if (bottomy >= 0.0f) topy = r->y_left_floor + bottomy;
	else bottomy = r->y_right_floor - bottomy;

	float centrey = topy + ((bottomy - topy) / 2);
	result.setFloat(centrey - targ->y);
}

/**
 CACL (command) family (integer) genus (integer) species (integer) caindex (integer)
 %status stub
*/
void caosVM::c_CACL() {
	VM_PARAM_INTEGER(caindex) caos_assert(0 <= caindex && caindex <= 19);
	VM_PARAM_INTEGER(species) caos_assert(0 <= species && species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(0 <= genus && genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(0 <= family && family <= 255);

	// TODO
}

/* vim: set noet: */
