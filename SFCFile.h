/*
 *  SFCFile.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sat 21 Oct 2006.
 *  Copyright (c) 2006-2008 Alyssa Milburn. All rights reserved.
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
class SFCObject;
class SFCScenery;
class SFCMacro;

struct SFCScript {
	uint8 genus, family;
	uint16 species, eventno;
	std::string data;

	void read(class SFCFile *);
	void install();
};

class SFCFile {
private:
	bool reading_compound;
	bool reading_scenery;
	unsigned int ver;
	
	std::vector<SFCClass *> storage;
	std::map<unsigned int, unsigned int> types;

	std::istream *ourStream;

public:
	MapData *mapdata;
	std::vector<SFCObject *> objects;
	std::vector<SFCScenery *> scenery;
	std::vector<SFCScript> scripts;
	std::vector<SFCMacro *> macros;

	uint32 scrollx, scrolly;
	std::string favplacename;
	uint32 favplacex, favplacey;
	std::vector<std::string> speech_history;

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

	bool readingScenery() { return reading_scenery; }
	bool readingCompound() { return reading_compound; }
	void setVersion(unsigned int v);
	unsigned int version() { return ver; }

	void copyToWorld();
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

public:
	SFCFile *getParent() { return parent; }
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
	unsigned int groundlevels[261];

	// TODO: misc data
	
	MapData(SFCFile *p) : SFCClass(p) { }
	void read();
	void copyToWorld();
	virtual ~MapData();
};

class SFCEntity : public SFCClass {
public:
	CGallery *sprite;

	uint8 currframe, imgoffset;
	uint32 zorder;
	uint32 x, y;
	bool haveanim;
	uint8 animframe;
	std::string animstring;

	uint32 relx, rely;

	uint32 partzorder;
	int bhvrclick[3];
	uint8 bhvrtouch;

	std::vector<std::pair<int, int> > pickup_handles;
	std::vector<std::pair<int, int> > pickup_points;

	// TODO: misc data/flags
	SFCEntity(SFCFile *p) : SFCClass(p) { }
	void read();
};

class SFCObject : public SFCClass {
protected:
	SFCObject(SFCFile *p) : SFCClass(p) { }

public:
	uint8 genus, family;
	uint16 species;

	uint32 unid; // needed?
	uint16 attr;
	uint32 left, top, right, bottom; // what is this?
	uint8 actv;

	std::string currentsound;

	CGallery *sprite;

	uint32 tickreset, tickstate;

	uint32 variables[100];

	uint8 size, threat;
	uint32 range, accg, velx, vely, rest, aero;
	uint32 gravdata;

	bool frozen;

	std::vector<SFCScript> scripts;
	void read();
	virtual void copyToWorld() = 0;
	virtual class Agent *copiedAgent() = 0;
};

struct SFCHotspot {
	int left, top, right, bottom;
	int function;
	uint16 message;
	uint8 mask;
};

class SFCCompoundObject : public SFCObject {
protected:
	class CompoundAgent *ourAgent;

public:
	std::vector<SFCEntity *> parts;

	SFCHotspot hotspots[6];

	SFCCompoundObject(SFCFile *p) : SFCObject(p) { ourAgent = 0; }
	void read();
	void copyToWorld();
	class Agent *copiedAgent() { return (Agent *)ourAgent; }
};

class SFCBlackboard : public SFCCompoundObject {
public:
	uint32 textx, texty;
	uint32 backgroundcolour, chalkcolour, aliascolour;
	std::vector<std::pair<uint32, std::string> > strings;

	SFCBlackboard(SFCFile *p) : SFCCompoundObject(p) { }
	void read();
	void copyToWorld();
};

class SFCVehicle : public SFCCompoundObject {
public:
	uint32 cabinleft, cabintop, cabinright, cabinbottom;
	int xvec, yvec;
	uint8 bump;

	// TODO: misc data

	SFCVehicle(SFCFile *p) : SFCCompoundObject(p) { }
	void read();
	void copyToWorld();
};

class SFCLift : public SFCVehicle {
	friend class SFCCallButton;

public:
	uint32 nobuttons;
	uint32 currentbutton;
	uint32 callbuttony[8];
	bool alignwithcabin;
	// TODO: misc data

	SFCLift(SFCFile *p) : SFCVehicle(p) { }
	void read();
	void copyToWorld();
};

class SFCSimpleObject : public SFCObject {
protected:
	class SimpleAgent *ourAgent;

public:
	SFCEntity *entity;

	SFCSimpleObject(SFCFile *p) : SFCObject(p) { ourAgent = 0; }
	void read();
	void copyToWorld();
	class Agent *copiedAgent() { return (Agent *)ourAgent; }
};

class SFCPointerTool : public SFCSimpleObject {
public:
	// TODO: misc data

	SFCPointerTool(SFCFile *p) : SFCSimpleObject(p) { }
	void read();
	void copyToWorld();
};

class SFCCallButton : public SFCSimpleObject {
public:
	SFCLift *ourLift;
	uint8 liftid;

	SFCCallButton(SFCFile *p) : SFCSimpleObject(p) { }
	void read();
	void copyToWorld();
};

class SFCScenery : public SFCSimpleObject {
public:
	SFCScenery(SFCFile *p) : SFCSimpleObject(p) { }
	void copyToWorld();
};

class SFCMacro : public SFCClass {
protected:
	SFCObject *owner, *from, *targ;
	std::string script;

public:
	SFCMacro(SFCFile *p) : SFCClass(p) { }
	void read();
	void activate();
};

#endif

/* vim: set noet: */
