/*
 *  World.h
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

#ifndef _WORLD_H
#define _WORLD_H

#include "Map.h"
#include "Scriptorium.h"
#include "Catalogue.h"
#include "Camera.h"
#include "Agent.h"
#include "CompoundPart.h"
#include "prayManager.h"
#include "historyManager.h"
#include <set>
#include <map>
#include <boost/filesystem/path.hpp>

struct cainfo {
	float gain;
	float loss;
	float diffusion;
};

struct scriptevent {
	unsigned short scriptno;
	AgentRef agent, from;
	caosVar p[2];
};

class SDLBackend;

class World {
protected:
	class PointerAgent *theHand;
	std::list<scriptevent> scriptqueue;

public:
	bool quitting, saving, paused;
	
	Map map;

	std::multiset<CompoundPart *, partzorder> zorder; // sorted from top to bottom
	std::multiset<renderable *, renderablezorder> renders; // sorted from bottom to top
	std::list<boost::shared_ptr<Agent> > agents;
	
	std::map<unsigned int, std::map<unsigned int, cainfo> > carates;
	std::map<std::string, caosVar> variables;
	std::map<caosVar, caosVar, caosVarCompare> eame_variables; // non-serialised
	std::vector<caosVM *> vmpool;
	std::map<int, boost::weak_ptr<Agent> > unidmap;

	std::vector<boost::filesystem::path> data_directories;
	Scriptorium scriptorium;
	Catalogue catalogue;
	prayManager praymanager;
	imageGallery gallery;
	historyManager history;
		
	std::string gametype;
	float pace;
	unsigned int race;
	unsigned int ticktime, tickcount;
	unsigned int worldtickcount;
	MainCamera camera;
	SDLBackend *backend;
	bool showrooms, autokill;

	AgentRef selectedcreature;
	void selectCreature(boost::shared_ptr<Agent> c);
	AgentRef focusagent; unsigned int focuspart;
	void setFocus(class TextEntryPart *p);

	Agent *agentAt(unsigned int x, unsigned int y, bool obey_all_transparency = true, bool needs_mouseable = false);
	CompoundPart *partAt(unsigned int x, unsigned int y, bool obey_all_transparency = true, bool needs_mouseable = false);
	class PointerAgent *hand() { return theHand; }
	
	caosVM *getVM(Agent *owner);
	void freeVM(caosVM *);
	void queueScript(unsigned short event, AgentRef agent, AgentRef from = AgentRef(), caosVar p0 = caosVar(), caosVar p1 = caosVar());
	
	World();
	~World();
	void init();
	void initCatalogue();
	
	void executeInitScript(boost::filesystem::path p);
	void executeBootstrap(boost::filesystem::path p);
	void executeBootstrap(bool switcher);

	std::string getUserDataDir();
	std::string findFile(std::string path);
	std::vector<std::string> findFiles(std::string dir, std::string wild);

	std::string generateMoniker(std::string basename);
	
	void tick();
	void drawWorld();
	void drawWorld(Camera *cam, SDLSurface *surface);

	int getUNID(Agent *whofor);
	void freeUNID(int unid);

	Agent *lookupUNID(int unid);
};

extern World world;
#endif
/* vim: set noet: */
