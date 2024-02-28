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

#include <cstdint>
#include <istream>
#include <map>
#include <string>
#include <vector>

/*
 * Note that we don't use std::shared_ptr here, everything is owned by the
 * SFCFile. I couldn't find a way to cast downwards with std::shared_ptr,
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
	uint8_t genus, family;
	uint16_t species, eventno;
	std::string data;

	void read(class SFCFile*);
	void install();
};

class SFCFile {
  private:
	unsigned int ver;

	std::vector<SFCClass*> storage;
	std::map<unsigned int, unsigned int> types;

	std::istream* ourStream;

  public:
	MapData* mapdata;
	std::vector<SFCObject*> objects;
	std::vector<SFCScenery*> scenery;
	std::vector<SFCScript> scripts;
	std::vector<SFCMacro*> macros;

	uint32_t scrollx, scrolly;
	std::string favplacename;
	uint32_t favplacex, favplacey;
	std::vector<std::string> speech_history;

	SFCFile() {}
	~SFCFile();
	void read(std::istream* i);
	SFCClass* slurpMFC(unsigned int reqtype = 0);

	uint8_t read8();
	uint16_t read16();
	uint32_t read32();
	int32_t reads32() { return (int32_t)read32(); }
	std::string readBytes(unsigned int n);
	std::string readstring();

	void setVersion(unsigned int v);
	unsigned int version() { return ver; }

	void copyToWorld();
};

class SFCClass {
  protected:
	friend class SFCFile;

	SFCFile* parent;
	SFCClass(SFCFile* p)
		: parent(p) {}
	virtual ~SFCClass() {}

	SFCClass* slurpMFC(unsigned int reqtype = 0) { return parent->slurpMFC(reqtype); }
	uint8_t read8() { return parent->read8(); }
	uint16_t read16() { return parent->read16(); }
	uint32_t read32() { return parent->read32(); }
	signed int reads32() { return parent->reads32(); }
	std::string readBytes(unsigned int n) { return parent->readBytes(n); }
	std::string readstring() { return parent->readstring(); }

	virtual void read() = 0;

  public:
	SFCFile* getParent() { return parent; }
};

class CGallery : public SFCClass {
  public:
	uint32_t noframes;
	uint32_t firstimg;
	std::string filename;

	// no real need for sizes/offsets/etc..

	CGallery(SFCFile* p)
		: SFCClass(p) {}
	void read();
};

class CDoor : public SFCClass {
  public:
	uint8_t openness;
	uint16_t otherroom;

	CDoor(SFCFile* p)
		: SFCClass(p) {}
	void read();
};

class CRoom : public SFCClass {
  public:
	uint32_t id;
	int32_t left, top, right, bottom;
	std::vector<CDoor*> doors[4];
	int32_t roomtype;
	uint8_t floorvalue;
	uint8_t inorganicnutrients, organicnutrients, temperature, pressure, lightlevel, radiation;
	signed char heatsource, pressuresource, lightsource, radiationsource;
	uint32_t windx, windy;
	std::vector<std::pair<uint32_t, uint32_t> > floorpoints;
	std::string music;
	uint32_t dropstatus;

	// TODO: misc data

	CRoom(SFCFile* p)
		: SFCClass(p) {}
	void read();
};

class MapData : public SFCClass {
  public:
	CGallery* background;
	std::vector<CRoom*> rooms;
	unsigned int groundlevels[261];

	// TODO: misc data

	MapData(SFCFile* p)
		: SFCClass(p) {}
	void read();
	void copyToWorld();
	virtual ~MapData();
};

class SFCEntity : public SFCClass {
  public:
	CGallery* sprite;

	uint8_t currframe, imgoffset;
	int32_t zorder;
	uint32_t x, y;
	bool haveanim;
	uint8_t animframe;
	std::string animstring;

	// TODO: misc data/flags
	SFCEntity(SFCFile* p)
		: SFCClass(p) {}
	void read();
};

class SFCObject : public SFCClass {
  protected:
	SFCObject(SFCFile* p)
		: SFCClass(p) {}

  public:
	uint8_t genus, family;
	uint16_t species;

	uint32_t unid; // needed?
	uint16_t attr;
	uint32_t left, top, right, bottom; // what is this?
	uint8_t actv;

	std::string currentsound;

	CGallery* sprite;

	uint32_t tickreset, tickstate;

	uint32_t variables[100];

	uint8_t size, threat;
	uint32_t range, accg, rest, aero;
	int32_t velx, vely;
	uint32_t gravdata;

	bool frozen;

	std::vector<SFCScript> scripts;
	void read();
	virtual void copyToWorld() = 0;
	virtual class Agent* copiedAgent() = 0;
};

struct SFCHotspot {
	int left, top, right, bottom;
	int function;
	uint16_t message;
	uint8_t mask;
};

class SFCCompoundObject : public SFCObject {
  protected:
	class CompoundAgent* ourAgent;

  public:
	std::vector<SFCEntity*> parts;
	std::vector<uint32_t> parts_relx;
	std::vector<uint32_t> parts_rely;

	SFCHotspot hotspots[6];

	SFCCompoundObject(SFCFile* p)
		: SFCObject(p) { ourAgent = 0; }
	void read();
	void copyToWorld();
	class Agent* copiedAgent() {
		return (Agent*)ourAgent;
	}
};

class SFCBlackboard : public SFCCompoundObject {
  public:
	uint32_t textx, texty;
	uint32_t backgroundcolour, chalkcolour, aliascolour;
	std::vector<std::pair<uint32_t, std::string> > strings;

	SFCBlackboard(SFCFile* p)
		: SFCCompoundObject(p) {}
	void read();
	void copyToWorld();
};

class SFCVehicle : public SFCCompoundObject {
  public:
	uint32_t cabinleft, cabintop, cabinright, cabinbottom;
	int xvec, yvec;
	uint8_t bump;

	// TODO: misc data

	SFCVehicle(SFCFile* p)
		: SFCCompoundObject(p) {}
	void read();
	void copyToWorld();
};

class SFCLift : public SFCVehicle {
	friend class SFCCallButton;

  public:
	uint32_t nobuttons;
	uint32_t currentbutton;
	uint32_t callbuttony[8];
	bool alignwithcabin;
	// TODO: misc data

	SFCLift(SFCFile* p)
		: SFCVehicle(p) {}
	void read();
	void copyToWorld();
};

class SFCSimpleObject : public SFCObject {
  protected:
	class SimpleAgent* ourAgent;

  public:
	SFCEntity* entity;

	uint32_t partzorder;
	int bhvrclick[3];
	uint8_t bhvrtouch;

	std::vector<std::pair<int, int> > pickup_handles;
	std::vector<std::pair<int, int> > pickup_points;

	SFCSimpleObject(SFCFile* p)
		: SFCObject(p) { ourAgent = 0; }
	void read();
	void copyToWorld();
	class Agent* copiedAgent() {
		return (Agent*)ourAgent;
	}
};

class SFCPointerTool : public SFCSimpleObject {
  public:
	// TODO: misc data

	SFCPointerTool(SFCFile* p)
		: SFCSimpleObject(p) {}
	void read();
	void copyToWorld();
};

class SFCCallButton : public SFCSimpleObject {
  public:
	SFCLift* ourLift;
	uint8_t liftid;

	SFCCallButton(SFCFile* p)
		: SFCSimpleObject(p) {}
	void read();
	void copyToWorld();
};

class SFCScenery : public SFCObject {
  protected:
	class SimpleAgent* ourAgent = nullptr;

  public:
	SFCEntity* entity;

	SFCScenery(SFCFile* p)
		: SFCObject(p) {}
	void read();
	void copyToWorld();
	class Agent* copiedAgent() {
		return (Agent*)ourAgent;
	}
};

class SFCMacro : public SFCClass {
  protected:
	SFCObject *owner, *from, *targ;
	std::string script;

  public:
	SFCMacro(SFCFile* p)
		: SFCClass(p) {}
	void read();
	void activate();
};

#endif

/* vim: set noet: */
