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

#include "Agent.h"
#include "AgentHelpers.h"
#include "Engine.h" // version
#include "Map.h"
#include "MetaRoom.h"
#include "Room.h"
#include "World.h"
#include "caosVM.h"
#include "caos_assert.h"

#include <assert.h>
#include <fmt/core.h>
#include <memory>

#define CAOS_LVALUE_TARG_ROOM(name, check, get, set) \
	CAOS_LVALUE_TARG(name, \
					 std::shared_ptr<Room> r = roomContainingAgent(vm->targ); \
					 caos_assert(r); \
					 check, \
					 get, \
					 set)

#define CAOS_LVALUE_ROOM_SIMPLE(name, expr) \
	CAOS_LVALUE_TARG_ROOM(name, (void)0, expr, expr = newvalue)

/**
 ADDM (integer) x (integer) y (integer) width (integer) height (integer) background (string)
 %status maybe
 
 Creates a metaroom with the given height and width, at the coordinates given.  Returns the id of the new metaroom.
 */
void v_ADDM(caosVM* vm) {
	VM_VERIFY_SIZE(5)
	VM_PARAM_STRING(background)
	VM_PARAM_INTEGER(height)
	VM_PARAM_INTEGER(width)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	MetaRoom* r = world.map->addMetaRoom(x, y, width, height, background);
	vm->result.setInt(r->id);
}

/**
 ADDB (command) metaroom_id (integer) background (string)
 %status maybe

 Adds a new background to an existing metaroom, to be displayed with BKGD.
*/
void c_ADDB(caosVM* vm) {
	VM_PARAM_STRING(background)
	VM_PARAM_INTEGER(metaroomid)

	MetaRoom* m = world.map->getMetaRoom(metaroomid);
	caos_assert(m);

	m->addBackground(background);
}

/**
 BRMI (command) metaroom_base (integer) room_base (integer)
 %status maybe
 
 Sets the base ID numbers for new metarooms and rooms to the given values.
 */
void c_BRMI(caosVM* vm) {
	VM_VERIFY_SIZE(2)

	VM_PARAM_INTEGER(room_base)
	VM_PARAM_INTEGER(metaroom_base)

	world.map->room_base = room_base;
	world.map->metaroom_base = metaroom_base;
}

/**
 MAPD (command) width (integer) height (integer)
 %status maybe
 
 Sets the world map dimensions, inside which metarooms are placed.
 */
void c_MAPD(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(height)
	VM_PARAM_INTEGER(width)

	world.map->SetMapDimensions(width, height);
}

/**
 MAPW (integer)
 %status maybe
 
 Returns the width of the world map.
*/
void v_MAPW(caosVM* vm) {
	vm->result.setInt(world.map->getWidth());
}

/**
 MAPH (integer)
 %status maybe

 Returns the height of the world map.
*/
void v_MAPH(caosVM* vm) {
	vm->result.setInt(world.map->getHeight());
}

/**
 MAPK (command)  
 %status maybe
 
 Resets and empties the world map.
 */
void c_MAPK(caosVM*) {
	VM_VERIFY_SIZE(0)

	world.map->Reset();
}

/**
 BKDS (string) metaroomid (integer)
 %status maybe

 Determines all of the background names in use by the given metaroom, and returns them in a comma-seperated string.
*/
void v_BKDS(caosVM* vm) {
	VM_PARAM_INTEGER(metaroomid)

	MetaRoom* m = world.map->getMetaRoom(metaroomid);
	caos_assert(m);

	std::vector<std::string> backs = m->backgroundList();
	std::string s;
	for (auto& back : backs) {
		if (s.empty())
			s = back;
		else
			s = s + "," + back;
	}

	vm->result.setString(s);
}

/**
 ADDR (integer) metaroomid (integer) x_left (integer) x_right (integer) y_left_ceiling (integer) y_right_ceiling (integer) y_left_floor (integer) y_right_floor (integer)
 %status maybe

 Makes a new room inside the given metaroom.  Rooms can have sloped floors and ceilings, but only vertical walls.
 The id of the new room is returned.
*/
void v_ADDR(caosVM* vm) {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(y_right_floor)
	VM_PARAM_INTEGER(y_left_floor)
	VM_PARAM_INTEGER(y_right_ceiling)
	VM_PARAM_INTEGER(y_left_ceiling)
	VM_PARAM_INTEGER(x_right)
	VM_PARAM_INTEGER(x_left)
	VM_PARAM_INTEGER(metaroomid)

	std::shared_ptr<Room> r(new Room(x_left, x_right,
		y_left_ceiling, y_right_ceiling,
		y_left_floor, y_right_floor));
	MetaRoom* m = world.map->getMetaRoom(metaroomid);
	caos_assert(m);
	r->id = m->addRoom(r);
	vm->result.setInt(r->id);
}

/**
 RTYP (command) roomid (integer) roomtype (integer)
 %status maybe

 Defines the 'type' of the given room.  The types vary with different games.
*/
void c_RTYP(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(roomtype)
	VM_PARAM_INTEGER(roomid)

	std::shared_ptr<Room> room = world.map->getRoom(roomid);
	caos_assert(room);
	room->type = roomtype;
}

/**
 RTYP (integer) roomid (integer)
 %status maybe

 Returns the 'type' of the given room, or -1 if 'roomid' is invalid.
*/
void v_RTYP(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(roomid)

	std::shared_ptr<Room> room = world.map->getRoom(roomid);
	if (room)
		vm->result.setInt(room->type);
	else
		vm->result.setInt(-1);
}

/**
 RTYP (integer)
 %status maybe
 %variants c2

 Returns the room type of the room at the centre point of targ.
*/
void v_RTYP_c2(caosVM* vm) {
	valid_agent(vm->targ);
	std::shared_ptr<Room> r = world.map->roomAt(vm->targ->x + (vm->targ->getWidth() / 2.0f), vm->targ->y + (vm->targ->getHeight() / 2.0f));
	if (!r)
		vm->result.setInt(-1);
	else {
		vm->result.setInt(r->type);
	}
}

/**
 SETV RTYP (command) roomtype (integer)
 %status maybe
 %variants c2

 Sets the type of the given room to roomtype.
*/
void c_SETV_RTYP(caosVM* vm) {
	VM_VERIFY_SIZE(1);
	VM_PARAM_INTEGER(roomtype);

	// TODO: this does actually work on targ, right?
	// seems to work for the airlock, anyway  -nornagon
	valid_agent(vm->targ);
	std::shared_ptr<Room> r = world.map->roomAt(vm->targ->x + (vm->targ->getWidth() / 2.0f), vm->targ->y + (vm->targ->getHeight() / 2.0f));
	if (!r)
		return; // TODO: correct behaviour?
	else
		r->type = roomtype;
}

/**
 DOOR (command) room1 (integer) room2 (integer) perm (integer)
 %status maybe

 Sets how permeable the door between the two given rooms will be. (See PERM).
*/
void c_DOOR(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(perm)
	VM_PARAM_INTEGER(room2)
	VM_PARAM_INTEGER(room1)

	std::shared_ptr<Room> r1 = world.map->getRoom(room1);
	std::shared_ptr<Room> r2 = world.map->getRoom(room2);
	caos_assert(r1);
	caos_assert(r2);

	world.map->setDoorPerm(r1, r2, perm);
}

/**
 DOOR (integer) room1 (integer) room2 (integer)
 %status stub
*/
void v_DOOR(caosVM* vm) {
	VM_PARAM_INTEGER(room2)
	VM_PARAM_INTEGER(room1)

	vm->result.setInt(-1); // TODO
}

/**
 RATE (command) roomtype (integer) caindex (integer) gain (float) loss (float) diffusion (float)
 %status maybe

 Defines the rates of the given CA in the given room.  'gain' defines how easily the CA will be absorbed from
 agents inside the room, 'loss' defines how much will be lost into the air, and 'diffusion' defines how easily it 
 will spread to other rooms.
*/
void c_RATE(caosVM* vm) {
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
void v_ROOM(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VALIDAGENT(agent)

	std::shared_ptr<Room> r = roomContainingAgent(agent);
	if (r)
		vm->result.setInt(r->id);
	else
		vm->result.setInt(-1);
}

/**
 LEFT (integer)
 %status maybe
 
 Returns the left constant (0).
*/
void v_LEFT(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(0);
}

/**
 RGHT (integer)
 %status maybe
 
 Returns the right constant (1).
*/
void v_RGHT(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(1);
}

/**
 _UP_ (integer)
 %status maybe
 
 Returns the up constant (2).
*/
void v_UP(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(2);
}

/**
 DOWN (integer)
 %status maybe
 
 Returns the down constant (3).
*/
void v_DOWN(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	vm->result.setInt(3);
}

/**
 PROP (command) roomid (integer) caindex (integer) cavalue (float)
 %status maybe

 Defines the level of the given CA in the given room.  Valid settings are between 0 and 1; if higher, it will be 
 reset to 1.
*/
void c_PROP(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(cavalue)
	VM_PARAM_INTEGER(caindex)
	VM_PARAM_INTEGER(roomid)

	if (cavalue > 1.0f)
		cavalue = 1.0f;
	else
		caos_assert(0.0f <= cavalue);
	caos_assert(0 <= caindex && caindex <= 19);

	std::shared_ptr<Room> room = world.map->getRoom(roomid);
	caos_assert(room);
	room->ca[caindex] = cavalue;
}

/**
 PROP (float) roomid (integer) caindex (integer)
 %status maybe

 Returns the level of the given CA in the given room, or 0 if a roomid of -1 is passed.
*/
void v_PROP(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(caindex)
	VM_PARAM_INTEGER(roomid)

	caos_assert(0 <= caindex && caindex <= 19);

	if (roomid == -1) {
		vm->result.setFloat(0.0f);
		return;
	}

	std::shared_ptr<Room> room = world.map->getRoom(roomid);
	caos_assert(room);
	vm->result.setFloat(room->ca[caindex]);
}

/**
 PERM (command) perm (integer)
 %status maybe

 Sets the TARG agent's permiability.  Valid settings are between 1 and 100.
*/
void c_PERM(caosVM* vm) {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(perm)

	// TODO: setting of 0 valid?
	if (perm < 0)
		perm = 0;
	if (perm > 100)
		perm = 100;

	valid_agent(vm->targ);
	vm->targ->perm = perm;
}

/**
 PERM (integer)
 %status maybe

 Returns the TARG agent's permiability.
*/
void v_PERM(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->perm);
}

/**
 GRAP (integer) x (float) y (float)
 %status maybe

 Returns the id of the room at the coordinates (x, y), or -1 if nothing's there.
*/
void v_GRAP(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	std::shared_ptr<Room> room = world.map->roomAt(x, y);
	if (room) {
		vm->result.setInt(room->id);
	} else {
		vm->result.setInt(-1);
	}
}

/**
 GMAP (integer) x (float) y (float)
 %status maybe

 Returns the id of the metaroom at the coordinates (x, y), or -1 if nothing's there.
*/
void v_GMAP(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(y)
	VM_PARAM_FLOAT(x)

	MetaRoom* room = world.map->metaRoomAt(x, y);
	if (room) {
		vm->result.setInt(room->id);
	} else {
		vm->result.setInt(-1);
	}
}

/**
 LINK (command) room1 (integer) room2 (integer) perm (integer)
 %status stub

 Defines the permeability of the link between the two given rooms.  This is used for CA diffusion.
*/
void c_LINK(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(perm)
	VM_PARAM_INTEGER(room2)
	VM_PARAM_INTEGER(room1)

	std::shared_ptr<Room> one = world.map->getRoom(room1);
	std::shared_ptr<Room> two = world.map->getRoom(room2);
	caos_assert(one && two);

	// TODO
}

/**
 LINK (integer) room1 (integer) room2 (integer)
 %status stub

 Returns the permeability of the link between the given two rooms, or 0 if no link exists.
*/
void v_LINK(caosVM* vm) {
	VM_PARAM_INTEGER(room2)
	VM_PARAM_INTEGER(room1)

	std::shared_ptr<Room> one = world.map->getRoom(room1);
	std::shared_ptr<Room> two = world.map->getRoom(room2);
	caos_assert(one && two);

	vm->result.setInt(0); // TODO
}

/**
 GRID (integer) agent (agent) direction (integer)
 %status maybe

 Returns the nearest adjacent room to the specified agent in the given direction (one of the direction constants), or 
 -1 otherwise.
*/
void v_GRID(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(direction)
	caos_assert(direction >= 0);
	caos_assert(direction <= 3);
	VM_PARAM_VALIDAGENT(agent)

	valid_agent(vm->targ);
	Point src = vm->targ->boundingBoxPoint(direction);
	Point dest = src;

	switch (direction) {
		case 0: // left
			dest.x -= vm->targ->range;
			break;
		case 1: // right
			dest.x += vm->targ->range;
			break;
		case 2: // top
			dest.y -= vm->targ->range;
			break;
		case 3: // bottom
			dest.y += vm->targ->range;
			break;
	}

	std::shared_ptr<Room> ourRoom = world.map->roomAt(src.x, src.y);
	if (!ourRoom) {
		// (should we REALLY check for it being in the room system, here?)
		fmt::print(stderr, "{} tried using GRID but isn't in the room system!\n", agent->identify());
		vm->result.setInt(-1);
		return;
	}

	unsigned int dummy1;
	Line dummy2;
	Point point;
	std::shared_ptr<Room> room;
	bool collided = world.map->collideLineWithRoomBoundaries(src, dest, ourRoom, room, point, dummy2, dummy1, vm->targ->perm);
	// TODO: do something with collided?
	(void)collided;

	if (!room)
		vm->result.setInt(-1);
	else
		vm->result.setInt(room->id);
}

/**
 EMIT (command) caindex (integer) amount (float)
 %status maybe

 Makes the TARG agent continually emit the specified amount of the specified CA into the room.
*/
void c_EMIT(caosVM* vm) {
	VM_VERIFY_SIZE(2)
	VM_PARAM_FLOAT(amount)
	VM_PARAM_INTEGER(caindex)

	caos_assert((0 <= caindex && caindex <= 19) || caindex == -1);
	valid_agent(vm->targ);

	vm->targ->emitca_index = caindex;
	vm->targ->emitca_amount = amount;
}

/**
 WALL (integer)
 %status maybe
 %variants c2 cv c3 sm

 Returns the direction of the last wall the TARG agent collided with.
*/
void v_WALL(caosVM* vm) {
	VM_VERIFY_SIZE(0)

	valid_agent(vm->targ);
	vm->result.setInt(vm->targ->lastcollidedirection);
}

/**
 ALTR (command) roomid (integer) caindex (integer) delta (float)
 %status maybe

 Modifies the level of the given CA in the room specified.
 If 'roomid' is -1, the room containing the TARG agent will be used.
*/
void c_ALTR(caosVM* vm) {
	VM_VERIFY_SIZE(3)
	VM_PARAM_FLOAT(delta);
	VM_PARAM_INTEGER(caindex);
	VM_PARAM_INTEGER(roomid);

	caos_assert(0 <= caindex && caindex <= 19);

	std::shared_ptr<Room> room;
	if (roomid == -1) {
		valid_agent(vm->targ);
		room = world.map->roomAt(vm->targ->x + (vm->targ->getWidth() / 2.0f), vm->targ->y + (vm->targ->getHeight() / 2.0f));
	} else
		room = world.map->getRoom(roomid);
	caos_assert(room);
	float newvalue = room->ca[caindex] + delta;
	if (newvalue < 0.0f)
		newvalue = 0.0f;
	else if (newvalue > 1.0f)
		newvalue = 1.0f;
	room->ca[caindex] = newvalue;
}

/**
 RLOC (string) roomid (integer)
 %status maybe

 Returns a string containing the location of the given room in the following format: x_left, x_right, y_left_ceiling,
 y_right_ceiling, y_left_floor, y_right_floor.
*/
void v_RLOC(caosVM* vm) {
	VM_PARAM_INTEGER(roomid)

	std::shared_ptr<Room> r = world.map->getRoom(roomid);
	caos_assert(r);

	vm->result.setString(fmt::format("{} {} {} {} {} {}", r->x_left, r->x_right, r->y_left_ceiling, r->y_right_ceiling, r->y_left_floor, r->y_right_floor));
}

/**
 MLOC (string) metaroomid (integer)
 %status maybe

 Returns a string containing the location of the given metaroom in the following format: x y width height
*/
void v_MLOC(caosVM* vm) {
	VM_PARAM_INTEGER(metaroomid)

	MetaRoom* r = world.map->getMetaRoom(metaroomid);
	caos_assert(r);

	vm->result.setString(fmt::format("{} {} {} {}", r->x(), r->y(), r->width(), r->height()));
}

/**
 DMAP (command) mapon (integer)
 %status maybe

 Turns the debug map on and off, which shows the edges of rooms and vehicles.
*/
void c_DMAP(caosVM* vm) {
	VM_PARAM_INTEGER(mapon)

	world.showrooms = mapon;
}

/**
 SYS: DMAP (command) mapon (integer)
 %status maybe
 %variants c2
*/
void c_SYS_DMAP(caosVM* vm) {
	c_DMAP(vm);
}

/**
 ERID (string) metaroom_id (integer)
 %status maybe

 Returns a space-seperated list of all room id's contained by the given metaroom.
*/
void v_ERID(caosVM* vm) {
	VM_PARAM_INTEGER(metaroom_id)

	std::string out;

	if (metaroom_id == -1) {
		// TODO
	} else {
		MetaRoom* r = world.map->getMetaRoom(metaroom_id);
		for (auto& room : r->rooms) {
			if (out.size() > 0)
				out = out + " ";
			out = out + fmt::format("{}", room->id);
		}
	}

	vm->result.setString(out);
}

/**
 DELR (command) room_id (integer)
 %status stub
 %variants c2 cv c3 sm

 Removes the given room from the map.
*/
void c_DELR(caosVM* vm) {
	VM_PARAM_INTEGER(room_id)

	std::shared_ptr<Room> r = world.map->getRoom(room_id);
	caos_assert(r);

	// TODO
}

/**
 DELM (command) metaroom_id (integer)
 %status stub

 Removes the given metaroom from the map.
*/
void c_DELM(caosVM* vm) {
	VM_PARAM_INTEGER(metaroom_id)

	MetaRoom* r = world.map->getMetaRoom(metaroom_id);
	caos_assert(r);

	// TODO
}

/**
 HIRP (integer) roomid (integer) caindex (integer) direction (integer)
 %status stub
*/
void v_HIRP(caosVM* vm) {
	VM_PARAM_INTEGER(direction)
	VM_PARAM_INTEGER(caindex)
	caos_assert(0 <= caindex && caindex <= 19);
	VM_PARAM_INTEGER(roomid)

	std::shared_ptr<Room> r = world.map->getRoom(roomid);
	caos_assert(r);

	vm->result.setInt(roomid); // TODO
}

/**
 LORP (integer) roomid (integer) caindex (integer) direction (integer)
 %status stub
*/
void v_LORP(caosVM* vm) {
	VM_PARAM_INTEGER(direction)
	VM_PARAM_INTEGER(caindex)
	caos_assert(0 <= caindex && caindex <= 19);
	VM_PARAM_INTEGER(roomid)

	std::shared_ptr<Room> r = world.map->getRoom(roomid);
	caos_assert(r);

	vm->result.setInt(roomid); // TODO
}

/**
 TORX (float) roomid (integer)
 %status maybe
*/
void v_TORX(caosVM* vm) {
	VM_PARAM_INTEGER(roomid)

	std::shared_ptr<Room> r = world.map->getRoom(roomid);
	caos_assert(r);
	valid_agent(vm->targ);

	float centrex = r->x_left + ((r->x_right - r->x_left) / 2.0f);
	vm->result.setFloat(centrex - vm->targ->x);
}

/**
 TORY (float) roomid (integer)
 %status maybe
*/
void v_TORY(caosVM* vm) {
	VM_PARAM_INTEGER(roomid)

	std::shared_ptr<Room> r = world.map->getRoom(roomid);
	caos_assert(r);
	valid_agent(vm->targ);

	// TODO: calculate this however c2e does it.. or at least check this is right
	float topy = (r->y_left_ceiling - r->y_right_ceiling) / 2.0f;
	if (topy >= 0.0f)
		topy = r->y_left_ceiling + topy;
	else
		topy = r->y_right_ceiling - topy;

	float bottomy = (r->y_left_floor - r->y_right_floor) / 2.0f;
	if (bottomy >= 0.0f)
		topy = r->y_left_floor + bottomy;
	else
		bottomy = r->y_right_floor - bottomy;

	float centrey = topy + ((bottomy - topy) / 2.0f);
	vm->result.setFloat(centrey - vm->targ->y);
}

/**
 CACL (command) family (integer) genus (integer) species (integer) caindex (integer)
 %status stub
*/
void c_CACL(caosVM* vm) {
	VM_PARAM_INTEGER(caindex)
	caos_assert(0 <= caindex && caindex <= 19);
	VM_PARAM_INTEGER(species)
	caos_assert(0 <= species && species <= 65535);
	VM_PARAM_INTEGER(genus)
	caos_assert(0 <= genus && genus <= 255);
	VM_PARAM_INTEGER(family)
	caos_assert(0 <= family && family <= 255);

	// TODO
}

/**
 WIND (integer)
 %status stub
 %variants c1

 Always returns zero, since this command was stubbed in C1.
*/
void v_WIND(caosVM* vm) {
	vm->result.setInt(0);
}

/**
 TEMP (variable)
 %status maybe
 %variants c1 c2
*/
CAOS_LVALUE_ROOM_SIMPLE(TEMP, r->temp)

/**
 LITE (variable)
 %status maybe
 %variants c2
*/
CAOS_LVALUE_ROOM_SIMPLE(LITE, r->lite)

/**
 RADN (variable)
 %status maybe
 %variants c2
*/
CAOS_LVALUE_ROOM_SIMPLE(RADN, r->radn)

/**
 ONTR (variable)
 %status maybe
 %variants c2
*/
CAOS_LVALUE_ROOM_SIMPLE(ONTR, r->ontr)

/**
 INTR (variable)
 %status maybe
 %variants c2
*/
CAOS_LVALUE_ROOM_SIMPLE(INTR, r->intr)

/**
 PRES (variable)
 %status maybe
 %variants c2
*/
CAOS_LVALUE_ROOM_SIMPLE(PRES, r->pres)

/**
 HSRC (variable)
 %status maybe
 %variants c2
*/
CAOS_LVALUE_ROOM_SIMPLE(HSRC, r->hsrc)

/**
 LSRC (variable)
 %status maybe
 %variants c2
*/
CAOS_LVALUE_ROOM_SIMPLE(LSRC, r->lsrc)

/**
 RSRC (variable)
 %status maybe
 %variants c2
*/
CAOS_LVALUE_ROOM_SIMPLE(RSRC, r->rsrc)

/**
 PSRC (variable)
 %status maybe
 %variants c2
*/
CAOS_LVALUE_ROOM_SIMPLE(PSRC, r->psrc)

/**
 WNDX (integer)
 %status maybe
 %variants c2
*/
void v_WNDX(caosVM* vm) {
	valid_agent(vm->targ);
	std::shared_ptr<Room> r = roomContainingAgent(vm->targ);
	caos_assert(r);
	vm->result.setInt(r->windx);
}

/**
 WNDY (integer)
 %status maybe
 %variants c2
*/
void v_WNDY(caosVM* vm) {
	valid_agent(vm->targ);
	std::shared_ptr<Room> r = roomContainingAgent(vm->targ);
	caos_assert(r);
	vm->result.setInt(r->windy);
}

/**
 DOCA (command) times (integer)
 %status stub
*/
void c_DOCA(caosVM* vm) {
	VM_PARAM_INTEGER(times)

	// TODO
}

/**
 SETV DOOR (command) direction (integer) room1 (integer) room2 (integer) value (integer)
 %status maybe
 %variants c2
*/
void c_SETV_DOOR(caosVM* vm) {
	VM_PARAM_INTEGER(value)
	VM_PARAM_INTEGER(room2)
	VM_PARAM_INTEGER(room1)
	VM_PARAM_INTEGER(perm)

	// TODO: what's direction for?

	// code identical to c2e DOOR
	std::shared_ptr<Room> r1 = world.map->getRoom(room1);
	std::shared_ptr<Room> r2 = world.map->getRoom(room2);
	caos_assert(r1);
	caos_assert(r2);
	world.map->setDoorPerm(r1, r2, perm);
}

/**
 FLOR (integer)
 %status maybe
 %variants c2

 Return y coordinate of floor below centre of target agent.
*/
void v_FLOR(caosVM* vm) {
	valid_agent(vm->targ);

	std::shared_ptr<Room> r = roomContainingAgent(vm->targ);
	if (!r)
		vm->result.setInt(0); // TODO
	else
		vm->result.setInt(r->floorYatX(vm->targ->x + (vm->targ->getWidth() / 2.0f)));
}

/**
 GNDW (integer)
 %status stub
 %variants c1

 Return the number of horizontal pixels per piece of ground level data.
*/
void v_GNDW(caosVM* vm) {
	vm->result.setInt(32); // TODO: is it always 32? :) it said 4 here before! help! - fuzzie
}

/**
 GRND (integer) index (integer)
 %status maybe
 %variants c1

 Return the ground level data at the provided index. See GNDW to work out the index required.
*/
void v_GRND(caosVM* vm) {
	VM_PARAM_INTEGER(index)

	caos_assert(index >= 0 && (unsigned int)index < world.groundlevels.size());

	vm->result.setInt(world.groundlevels[index]);
}

/**
 ROOM (command) roomno (integer) left (integer) top (integer) right (integer) bottom (integer) type (integer)
 %status maybe
 %variants c1

 Create or modify a room.
*/
void c_ROOM(caosVM* vm) {
	VM_PARAM_INTEGER(type)
	VM_PARAM_INTEGER(bottom)
	VM_PARAM_INTEGER(right)
	VM_PARAM_INTEGER(top)
	VM_PARAM_INTEGER(left)
	VM_PARAM_INTEGER(roomno)

	std::shared_ptr<Room> r = world.map->getRoom(roomno);
	if (!r) {
		std::shared_ptr<Room> r2(new Room(left, right, top, top, bottom, bottom));
		r = r2;

		// MetaRoom *m = world.map->getMetaRoom(0);
		// unsigned int roomid = m->addRoom(r);
		//assert(roomid == (unsigned int)roomno); // TODO: this is fairly likely to fail, but is a major bug if it does, FIX ME!
		r->id = roomno;
	} else {
		r->x_left = left;
		r->x_right = right;
		r->y_left_ceiling = r->y_right_ceiling = top;
		r->y_left_floor = r->y_right_floor = bottom;
	}

	r->type = type;
}

/**
 ROOM (command) roomno (integer) left (integer) top (integer) right (integer) bottom (integer) type (integer) floorvalue (integer) organic (integer) inorganic (integer) temperature (integer) pressure (integer) light (integer) radiation (integer) heatsource (integer) pressuresource (integer) lightsource (integer) radiationsource (integer) dropstatus (integer)
 %status maybe
 %variants c2
*/
void c_ROOM_c2(caosVM* vm) {
	VM_PARAM_INTEGER(dropstatus)
	VM_PARAM_INTEGER(radiationsource)
	VM_PARAM_INTEGER(lightsource)
	VM_PARAM_INTEGER(pressuresource)
	VM_PARAM_INTEGER(heatsource)
	VM_PARAM_INTEGER(radiation)
	VM_PARAM_INTEGER(light)
	VM_PARAM_INTEGER(pressure)
	VM_PARAM_INTEGER(temperature)
	VM_PARAM_INTEGER(inorganic)
	VM_PARAM_INTEGER(organic)
	VM_PARAM_INTEGER(floorvalue)
	VM_PARAM_INTEGER(type)
	VM_PARAM_INTEGER(bottom)
	VM_PARAM_INTEGER(right)
	VM_PARAM_INTEGER(top)
	VM_PARAM_INTEGER(left)
	VM_PARAM_INTEGER(roomno)

	std::shared_ptr<Room> r = world.map->getRoom(roomno);
	if (!r) {
		r = std::shared_ptr<Room>(new Room(left, right, top, top, bottom, bottom));
		r->id = roomno;
	} else {
		r->x_left = left;
		r->x_right = right;
		r->y_left_ceiling = r->y_right_ceiling = top;
		r->y_left_floor = r->y_right_floor = bottom;
	}

	r->type = type;
	r->floorvalue = floorvalue;
	r->ontr = organic;
	r->intr = inorganic;
	r->temp = temperature;
	r->pres = pressure;
	r->lite = light;
	r->radn = radiation;
	r->hsrc = heatsource;
	r->psrc = pressuresource;
	r->lsrc = lightsource;
	r->rsrc = radiationsource;
	r->dropstatus = dropstatus;
}

/**
 ROOM (integer) roomno (integer) data (integer)
 %status maybe
 %variants c1 c2

 Return some data for the specified room number.
 Returns 0 if no such room.

 data: 0 is left, 1 is top, 2 is right, 3 is bottom, 4 is room type.
 for c2: 5 is floor value, 6 is organic nutrient, 7 is inorganic nutrient,
 8 is temperature, 9 is pressure, 10 is wind x, 11 is wind y, 12 is light,
 13 is radiation, 14 is heat source, 15 is pressure source, 16 is light
 source, 17 is radiation source, 18 is the visited flag and 19 is the
 drop status
*/
void v_ROOM_c1(caosVM* vm) {
	// TODO: the original docs here (for C1) said 1 is right and 2 is top, check?

	VM_PARAM_INTEGER(data)
	caos_assert(data >= 0 && data <= (engine.version == 1 ? 4 : 19));
	VM_PARAM_INTEGER(roomno)

	std::shared_ptr<Room> r = world.map->getRoom(roomno);
	if (!r) {
		vm->result.setInt(0);
		return;
	}

	switch (data) {
		case 0:
			vm->result.setInt(r->x_left);
			break;

		case 1:
			vm->result.setInt(r->y_left_ceiling);
			break;

		case 2:
			vm->result.setInt(r->x_right);
			break;

		case 3:
			vm->result.setInt(r->y_left_floor);
			break;

		case 4:
			vm->result.setInt(r->type);
			break;

		case 5:
			vm->result.setInt(r->floorvalue);
			break;

		case 6:
			vm->result.setInt(r->ontr);
			break;

		case 7:
			vm->result.setInt(r->intr);
			break;

		case 8:
			vm->result.setInt(r->temp);
			break;

		case 9:
			vm->result.setInt(r->pres);
			break;

		case 10:
			vm->result.setInt(r->windx);
			break;

		case 11:
			vm->result.setInt(r->windy);
			break;

		case 12:
			vm->result.setInt(r->lite);
			break;

		case 13:
			vm->result.setInt(r->radn);
			break;

		case 14:
			vm->result.setInt(r->hsrc);
			break;

		case 15:
			vm->result.setInt(r->psrc);
			break;

		case 16:
			vm->result.setInt(r->lsrc);
			break;

		case 17:
			vm->result.setInt(r->rsrc);
			break;

		case 18:
			vm->result.setInt(0); // TODO: visited
			break;

		case 19:
			vm->result.setInt(r->dropstatus);
			break;
	}
}

/**
 WRAP (variable) metaroom_id (integer)
 %status done
 %variants openc2e

 The world-wrapping flag for the specified metaroom. 1 to enable wrapping, 0 to disable.
*/
CAOS_LVALUE(WRAP,
			VM_PARAM_INTEGER(metaroom_id);
			MetaRoom* mr = world.map->getMetaRoom(metaroom_id);
			caos_assert(mr),
			caosValue((int)mr->wraparound()),
			mr->setWraparound(newvalue.getInt()))

/**
 SSFC (command) roomno (integer) count (integer) x1 (integer) y1 (integer)
 %status maybe
 %variants c2
 %stackdelta any

 Set floor points of the specified rooms.
 You must provide 'count' pairs of (x, y) coordinates as parameters, relative to the room, and with the y coordinates inverted (ie, 0 is the bottom of the room).
 The coordinates must start at the left side of the room, and end on the right side.
 Count can be zero, which removes any existing floor points.
*/
void c_SSFC(caosVM* vm) {
	// Note: due to parser hacks, our arguments are passed in a different order than normal
	VM_PARAM_INTEGER(roomno);
	VM_PARAM_INTEGER(coordcount);

	caos_assert(coordcount >= 0); // this should never happen unless the parser breaks or we load a bad savefile

	std::shared_ptr<Room> r = world.map->getRoom(roomno);
	caos_assert(r);

	r->floorpoints.clear();

	int lastx = -1;
	for (int i = 0; i < coordcount; i++) {
		VM_PARAM_INTEGER(x);
		VM_PARAM_INTEGER(y);

		// check that coordinates are positive
		caos_assert(x >= 0);
		caos_assert(y >= 0);

		// check that coordinates go from left to right
		caos_assert(x > lastx);
		lastx = x;

		// check that coordinates lie within room
		caos_assert((unsigned int)x <= (r->x_right - r->x_left));
		caos_assert((unsigned int)y <= (r->y_left_floor - r->y_left_ceiling));

		r->floorpoints.push_back(std::pair<unsigned int, unsigned int>(x, y));
	}
}
/**
 RMNO (integer)
 %status stub
 %variants c2
*/
void v_RMNO(caosVM* vm) {
	vm->result.setInt(0); // TODO
}

/**
 RMN# (integer) room (integer) direction (integer)
 %status stub
 %variants c2
*/
void v_RMN(caosVM* vm) {
	VM_PARAM_INTEGER(door)
	VM_PARAM_INTEGER(direction)

	vm->result.setInt(0); // TODO
}

/**
 RMNR (variable) room (integer) direction (integer) door (integer)
 %status stub
 %variants c2
*/
void v_RMNR(caosVM* vm) {
	VM_PARAM_INTEGER(door)
	VM_PARAM_INTEGER(direction)
	VM_PARAM_INTEGER(room)

	vm->result.setInt(0); // TODO
}
void s_RMNR(caosVM* vm) {
	VM_PARAM_INTEGER(door)
	VM_PARAM_INTEGER(direction)
	VM_PARAM_INTEGER(room)

	VM_PARAM_INTEGER(newvalue)

	// TODO
}

/**
 RMND (variable) room (integer) direction (integer) door (integer)
 %status stub
 %variants c2
*/
void v_RMND(caosVM* vm) {
	VM_PARAM_INTEGER(door)
	VM_PARAM_INTEGER(direction)
	VM_PARAM_INTEGER(room)

	vm->result.setInt(0); // TODO
}
void s_RMND(caosVM* vm) {
	VM_PARAM_INTEGER(door)
	VM_PARAM_INTEGER(direction)
	VM_PARAM_INTEGER(room)

	VM_PARAM_INTEGER(newvalue)

	// TODO
}
/**
 DELN (command) room (integer) direction (integer)
 %status stub
 %variants c2
*/
void c_DELN_c2(caosVM* vm) {
	VM_PARAM_INTEGER(direction)
	VM_PARAM_INTEGER(room)

	// TODO
}

/* vim: set noet: */
