/*
 *  SFCFile.h
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

#ifndef _SFCFILE_H
#define _SFCFILE_H

#include "endianlove.h"
#include <map>
#include <vector>
#include <string>
#include <istream>

/*
 * Note that we don't use shared_ptr here, everything is owned by the
 * SFCFile. I couldn't find a way to cast downwards with shared_ptr,
 * and since everything revolves around slurpMFC - which can only return
 * the base SFCClass type - it seemed best to just use pointers, since
 * ownership is clear. - fuzzie
 */

class SFCClass;
class MapData;
class Object;
class Scenery;

struct Script {
	uint8 genus, family;
	uint16 species, eventno;
	std::string data;
};

class SFCFile {
private:
	bool reading_compound;
	bool reading_scenery;
	
	std::vector<SFCClass *> storage;
	std::map<unsigned int, unsigned int> types;

	std::istream *ourStream;

public:
	MapData *mapdata;
	std::vector<Object *> objects;
	std::vector<Scenery *> scenery;
	std::vector<Script> scripts;

	uint32 scrollx, scrolly;
	// TODO: favourite places

	SFCFile() : reading_compound(false), reading_scenery(false) { }
	~SFCFile();
	void read(std::istream *i);
	SFCClass *slurpMFC(unsigned int reqtype = 0);

	uint8 read8();
	uint16 read16();
	uint32 read32();
	signed int reads32() { return (signed int)read32(); }
	std::string readBytes(unsigned int n);
	std::string readstring();
};

class SFCClass {
protected:
	friend class SFCFile;

	SFCFile *parent;
	SFCClass(SFCFile *p) : parent(p) { }
	virtual ~SFCClass() { }

	SFCClass *slurpMFC(unsigned int reqtype = 0) { return parent->slurpMFC(reqtype); }
	uint8 read8() { return parent->read8(); }
	uint16 read16() { return parent->read16(); }
	uint32 read32() { return parent->read32(); }
	signed int reads32() { return parent->reads32(); }
	std::string readBytes(unsigned int n) { return parent->readBytes(n); }
	std::string readstring() { return parent->readstring(); }

	virtual void read() = 0;
};

class CGallery : public SFCClass {
public:
	uint32 noframes;
	uint32 firstimg;
	std::string filename;

	// no real need for sizes/offsets/etc..

	CGallery(SFCFile *p) : SFCClass(p) { }
	void read();
};

class CDoor : public SFCClass {
public:
	uint8 openness;
	uint16 otherroom;
	
	CDoor(SFCFile *p) : SFCClass(p) { }
	void read();
};

class CRoom : public SFCClass {
public:
	uint32 id;
	uint32 left, top, right, bottom;
	std::vector<CDoor *> doors[4];
	uint32 roomtype;
	uint8 floorvalue;
	uint8 inorganicnutrients, organicnutrients, temperature, pressure, lightlevel, radiation;
	signed char heatsource, pressuresource, lightsource, radiationsource;
	uint32 windx, windy;
	std::vector<std::pair<uint32, uint32> > floorpoints;
	std::string music;
	uint32 dropstatus;

	// TODO: misc data
	
	CRoom(SFCFile *p) : SFCClass(p) { }
	void read();
};

class MapData : public SFCClass {
public:
	CGallery *background;
	std::vector<CRoom *> rooms;
	
	// TODO: misc data
	
	MapData(SFCFile *p) : SFCClass(p) { }
	void read();
};

class Entity : public SFCClass {
public:
	CGallery *sprite;

	uint8 currframe, imgoffset;
	uint32 zorder;
	uint32 x, y;
	bool haveanim;
	uint8 animframe;
	std::string animstring;

	uint32 partzorder;
	uint8 bhvrclick; // TODO: appropriate? should be enum?
	uint8 bhvrtouch;

	std::vector<std::pair<uint32, uint32> > pickup_handles;
	std::vector<std::pair<uint32, uint32> > pickup_points;

	// TODO: misc data/flags
};

class Object : public SFCClass {
public:
	uint8 genus, family;
	uint16 species;

	uint32 unid; // needed?
	uint16 attr;
	uint32 left, top, right, bottom; // what is this?
	uint8 bhvrclickstate;

	CGallery *sprite;

	uint32 tick1, tick2;

	uint32 variables[100];

	uint8 size, threat;
	uint32 range, accg, velx, vely, rest, aero;

	bool frozen;

	std::vector<Script> scripts;
};

class CompoundObject : public Object {
public:
	std::vector<Entity *> parts;
	
	// TODO: hotspots
	// TODO: misc data
};

class Blackboard : public CompoundObject {
public:
	uint32 textx, texty;
	uint16 backgroundcolour, chalkcolour, aliascolour;
	std::map<uint32, std::string> strings;
};

class Vehicle : public CompoundObject {
public:
	uint32 cabinleft, cabintop, cabinright, cabinbottom;

	// TODO: misc data
};

class Lift : public Vehicle {
	// TODO: misc data
};

class SimpleObject : public Object {
public:
	Entity *entity;
};

class PointerTool : public SimpleObject {
public:
	// TODO: misc data
};

class CallButton : public SimpleObject {
public:
	Lift *ourLift;
	unsigned int liftid;
};

class Scenery : public SimpleObject {
public:
	// TODO: misc data
};

#endif

/* vim: set noet: */
