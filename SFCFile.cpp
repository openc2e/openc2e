/*
 *  SFCFile.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat 21 Oct 2006.
 *  Copyright (c) 2006 Alyssa Milburn. All rights reserved.
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

#include "SFCFile.h"
#include "exceptions.h"

/*
 * sfcdumper.py has better commentary on this format - use it for debugging
 * and make sure to update it if you update this!
 */

#define TYPE_MAPDATA 1
#define TYPE_CGALLERY 2
#define TYPE_CDOOR 3
#define TYPE_CROOM 4

SFCFile::~SFCFile() {
	// This contains all the objects we've constructed, so we can just zap this and
	// everything neatly disappears.
	for (std::vector<SFCClass *>::iterator i = storage.begin(); i != storage.end(); i++) {
		delete *i;
	}
}

void SFCFile::read(std::istream *i) {
	ourStream = i;

	mapdata = (MapData *)slurpMFC(TYPE_MAPDATA);
	assert(mapdata);

	// TODO
}

bool validSFCType(unsigned int type, unsigned int reqtype) {
	if (reqtype == 0) return true;
	if (type == reqtype) return true;

	return false;
}

SFCClass *SFCFile::slurpMFC(unsigned int reqtype) {
	assert(!ourStream->fail());

	// read the pid (this only works up to 0x7ffe, but we'll cope)
	uint16 pid = read16();

	if (pid == 0) {
		// null object
		return 0;
	} else if (pid == 0xffff) {
		// completely new class, read details
		uint16 schemaid = read16();
		uint16 strlen = read16();
		char temp[strlen];
		ourStream->read(temp, strlen);
		std::string classname(temp, strlen);
		
		pid = storage.size();
		
		// push a null onto the stack
		storage.push_back(0);

		// set the types array as necessary
		if (classname == "MapData")
			types[pid] = TYPE_MAPDATA;
		else if (classname == "CGallery")
			types[pid] = TYPE_CGALLERY;
		else if (classname == "CDoor")
			types[pid] = TYPE_CDOOR;
		else if (classname == "CRoom")
			types[pid] = TYPE_CROOM;
		else
			throw creaturesException(std::string("SFCFile doesn't understand class name '") + classname + "'!");
	} else if (pid & 0x8000 != 0x8000) {
		// return an existing object
		pid -= 1;
		assert(pid < storage.size());
		assert(validSFCType(types[pid], reqtype));
		SFCClass *temp = storage[pid];
		assert(temp);
		return temp;
	} else {
		// create a new object of an existing class
		pid ^= 0x8000;
		pid -= 1;
		assert(pid < storage.size());
		assert(!(storage[pid]));
	}

	SFCClass *newobj;

	// construct new object of specified type
	assert(validSFCType(types[pid], reqtype));
	switch (types[pid]) {
		case TYPE_MAPDATA: newobj = new MapData(this); break;
		case TYPE_CGALLERY: newobj = new CGallery(this); break;
		case TYPE_CDOOR: newobj = new CDoor(this); break;
		case TYPE_CROOM: newobj = new CRoom(this); break;
		// TODO: make sure we set reading_compound/reading_scenery
		default:
			throw creaturesException("SFCFile didn't find a valid type in internal variable, argh!");
	}

	// push the object onto storage, and make it deserialize itself
	types[storage.size()] = types[pid];
	storage.push_back(newobj);
	newobj->read();

	// TODO: make sure we unset reading_compound/reading_scenery

	// return this new object
	return newobj;
}

uint8 SFCFile::read8() {
	char temp[1];
	ourStream->read(temp, 1);
	return temp[0];
}

uint16 SFCFile::read16() {
	char temp[2];
	ourStream->read(temp, 2);
	uint16 *i = (uint16 *)&temp;
	return swapEndianShort(*i);
}

uint32 SFCFile::read32() {
	char temp[4];
	ourStream->read(temp, 4);
	uint32 *i = (uint32 *)&temp;
	return swapEndianLong(*i);
}

std::string SFCFile::readstring() {
	uint32 strlen = read8();
	if (strlen == 0xff) {
		strlen = read16();
		if (strlen == 0xffff)
			strlen = read32();
	}

	return readBytes(strlen);
}

std::string SFCFile::readBytes(unsigned int n) {
	char temp[n];
	ourStream->read(temp, n);
	return std::string(temp, n);
}

// ------------------------------------------------------------------

void MapData::read() {
	// discard unknown bytes
	assert(read16() == 1);
	assert(read16() == 0);
	read32(); read32(); read32();

	background = (CGallery *)slurpMFC(TYPE_CGALLERY);
	assert(background);
	uint32 norooms = read32();
	for (unsigned int i = 0; i < norooms; i++) {
		CRoom *temp = (CRoom*)slurpMFC(TYPE_CROOM);
		assert(temp);
		rooms.push_back(temp);
	}
}

void CGallery::read() {
	noframes = read32();
	filename = parent->readBytes(4);
	firstimg = read32();
	
	// discard unknown bytes
	read32();

	for (unsigned int i = 0; i < noframes; i++) {
		// discard unknown bytes
		read8(); read8(); read8();
		// discard width, height and offset
		read32(); read32(); read32();
	}
}

void CDoor::read() {
	openness = read8();
	otherroom = read16();
	
	// discard unknown bytes
	assert(read16() == 0);
}

void CRoom::read() {
	id = read32();

	// magic constant?
	assert(read16() == 2);

	left = read32();
	top = read32();
	right = read32();
	bottom = read32();

	for (unsigned int i = 0; i < 4; i++) {
		uint16 nodoors = read16();
		for (unsigned int j = 0; j < nodoors; j++) {
			CDoor *temp = (CDoor*)slurpMFC(TYPE_CDOOR);
			assert(temp);
			doors[i].push_back(temp);
		}
	}

	roomtype = read32(); assert(roomtype < 4);

	floorvalue = read8();
	inorganicnutrients = read8();
	organicnutrients = read8();
	temperature = read8();
	heatsource = reads32();

	pressure = read8();
	pressuresource = reads32();

	windx = reads32();
	windy = reads32();

	lightlevel = read8();
	lightsource = reads32();

	radiation = read8();
	radiationsource = reads32();

	// discard unknown bytes
	readBytes(800);

	uint16 nopoints = read16();
	for (unsigned int i = 0; i < nopoints; i++) {
		floorpoints.push_back(std::pair<uint32, uint32>(read32(), read32()));
	}

	// discard unknown bytes
	assert(read32() == 0);

	music = readstring();
	dropstatus = read32(); assert(dropstatus < 3);
}

/* vim: set noet: */
