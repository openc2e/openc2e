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
using std::cerr;

/**
 ADDM (integer) x (integer) y (integer) width (integer) height (integer) background (string)
 
 create metaroom. return id of created metaroom.
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
 BRMI (command) metaroom_base (integer) room_base (integer)
 
 set metaroom/room bases, i have no idea why/if we need this
 */
void caosVM::c_BRMI() {
	VM_VERIFY_SIZE(2)
	
	VM_PARAM_INTEGER(room_base)
	VM_PARAM_INTEGER(metaroom_base)
	// todo
}

/**
 MAPD (command) width (integer) height (integer)
 
 set the map dimensions, inside which we place metarooms
 */
void caosVM::c_MAPD() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(height)
	VM_PARAM_INTEGER(width)

	world.map.SetMapDimensions(width, height);
}

/**
 MAPK (command)  
 
 reset the map (call map::reset)
 */
void caosVM::c_MAPK() {
	VM_VERIFY_SIZE(0)

	world.map.Reset();
}

/**
 ADDR (integer) metaroomid (integer) x_left (integer) x_right (integer) y_left_ceiling (integer) y_right_ceiling (integer) y_left_floor (integer) y_right_floor (integer)
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

	Room *r = new Room();
	r->x_left = x_left;
	r->x_right = x_right;
	r->y_left_ceiling = y_left_ceiling;
	r->y_right_ceiling = y_right_ceiling;
	r->y_left_floor = y_left_floor;
	r->y_right_floor = y_right_floor;
	result.setInt(world.map.getMetaRoom(metaroomid)->addRoom(r));
}

/**
 RTYP (command) roomid (integer) roomtype (integer)
*/
void caosVM::c_RTYP() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(roomtype)
	VM_PARAM_INTEGER(roomid)

	Room *room = world.map.getRoom(roomid);
	assert(room);
	room->type = roomtype;
}

/**
 RTYP (integer) roomid (integer)
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
*/
void caosVM::c_DOOR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(perm)
	VM_PARAM_INTEGER(room2)
	VM_PARAM_INTEGER(room1)

	// TODO
	Room *r1 = world.map.getRoom(room1);
	Room *r2 = world.map.getRoom(room2);
}

/**
 RATE (command) roomtype (integer) caindex (integer) gain (float) loss (float) diffusion (float)
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
 
 returns the room the midpoint of the agent is inside
*/
void caosVM::v_ROOM() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(agent)
	
	// TODO
	result.setInt(0);
}

/**
 LEFT (integer)
 
 returns the left constant (0)
*/
void caosVM::v_LEFT() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(3);
}

/**
 RGHT (integer)
 
 returns the right constant (1)
*/
void caosVM::v_RGHT() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(3);
}

/**
 _UP_ (integer)
 
 returns the up constant (2)
*/
void caosVM::v_UP() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(3);
}

/**
 DOWN (integer)
 
 returns the down constant (3)
*/
void caosVM::v_DOWN() {
	VM_VERIFY_SIZE(0)
	
	result.setInt(3);
}

/**
 PROP (command) roomid (integer) caindex (integer) cavalue (float)
*/
void caosVM::c_PROP() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(cavalue)
	VM_PARAM_INTEGER(caindex)
	VM_PARAM_INTEGER(roomid)

	Room *room = world.map.getRoom(roomid);
	// TODO
}

/**
 PROP (float) roomid (integer) caindex (integer)
*/
void caosVM::v_PROP() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(caindex)
	VM_PARAM_INTEGER(roomid)

	Room *room = world.map.getRoom(roomid);
	result.setFloat(1.0f); // TODO: don't hardcode
}