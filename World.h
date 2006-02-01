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

class World {
protected:
	class PointerAgent *theHand;
	std::list<scriptevent> scriptqueue;

public:
	bool quitting, saving, paused;
	
	Map map;

	std::multiset<CompoundPart *, partzorder> zorder; // sorted from top to bottom
	std::multiset<renderable *, renderablezorder> renders; // sorted from bottom to top
	std::list<Agent *> agents;
	
	std::map<unsigned int, std::map<unsigned int, cainfo> > carates;
	std::map<std::string, caosVar> variables;
	std::map<caosVar, caosVar> eame_variables; // non-serialised
	std::vector<caosVM *> vmpool;
	std::vector<Agent *> killqueue;
	std::map<int, Agent *> unidmap;

	std::vector<boost::filesystem::path> data_directories;
	Scriptorium scriptorium;
	Catalogue catalogue;
	prayManager praymanager;
	std::string gametype;
	float pace;
	unsigned int ticktime, tickcount;
	MainCamera camera;
	SDLBackend backend;
	bool showrooms;
	std::map<unsigned int, SDL_Surface **> backsurfs; // TODO: this is a horrible horrible icky hack

	AgentRef selectedcreature;
	void selectCreature(Agent *c);
	AgentRef focusagent; unsigned int focuspart;
	void setFocus(class TextEntryPart *p);

	Agent *agentAt(unsigned int x, unsigned int y, bool obey_all_transparency = false, bool needs_mouseable = false);
	CompoundPart *partAt(unsigned int x, unsigned int y, bool obey_all_transparency = false, bool needs_mouseable = false);
	class PointerAgent *hand() { return theHand; }
	
	caosVM *getVM(Agent *owner);
	void freeVM(caosVM *);
	void queueScript(unsigned short event, AgentRef agent, AgentRef from = AgentRef(), caosVar p0 = caosVar(), caosVar p1 = caosVar());
	
	World();
	void init();
	void initCatalogue();
	
	void executeInitScript(boost::filesystem::path p);
	void executeBootstrap(boost::filesystem::path p);
	void executeBootstrap(bool switcher);

	std::string getUserDataDir();
	std::string findFile(std::string path);

	void tick();
	void drawWorld();

	int World::getUNID(Agent *whofor);
	void World::freeUNID(int unid);

	Agent *lookupUNID(int unid);
};

extern World world;
#endif
/* vim: set noet: */
