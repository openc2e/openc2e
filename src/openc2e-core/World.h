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

#include "AgentRef.h"
#include "partzorder.h"
#include "renderablezorder.h"

#include <list>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

class caosValue;
class caosVM;
class CompoundPart;
class genomeFile;
class MainCamera;
class Map;
class historyManager;
class imageManager;
class prayManager;
class renderable;
class RenderTarget;
class Scriptorium;
struct scriptevent;

struct cainfo {
	float gain;
	float loss;
	float diffusion;
};

class World {
  protected:
	class PointerAgent* theHand;
	std::list<scriptevent> scriptqueue;

	std::map<int, std::weak_ptr<Agent> > unidmap;
	std::vector<caosVM*> vmpool;

  public:
	int vmpool_size() const { return vmpool.size(); }
	bool quitting, saving, paused;

	std::unique_ptr<Map> map;

	std::multiset<CompoundPart*, partzorder> zorder; // sorted from top to bottom
	std::multiset<renderable*, renderablezorder> renders; // sorted from bottom to top
	std::list<std::shared_ptr<Agent> > agents;

	std::map<unsigned int, std::map<unsigned int, cainfo> > carates;
	std::map<std::string, caosValue> variables;

	std::unique_ptr<Scriptorium> scriptorium;
	std::unique_ptr<prayManager> praymanager;
	std::unique_ptr<imageManager> gallery;
	std::unique_ptr<historyManager> history;

	float pace;
	unsigned int race;
	unsigned int ticktime, tickcount;
	unsigned int worldtickcount;
	unsigned int timeofday, dayofseason, season, year;
	bool showrooms, autokill, autostop;

	std::vector<unsigned int> groundlevels;

	AgentRef selectedcreature;
	void selectCreature(std::shared_ptr<Agent> c);
	AgentRef focusagent;
	unsigned int focuspart;
	void setFocus(class CompoundPart* p);

	Agent* agentAt(unsigned int x, unsigned int y, bool obey_all_transparency = true, bool needs_mouseable = false);
	CompoundPart* partAt(unsigned int x, unsigned int y, bool obey_all_transparency = true, bool needs_mouseable = false, bool needs_activateable = false);
	class PointerAgent* hand() {
		return theHand;
	}

	caosVM* getVM(Agent* owner);
	void freeVM(caosVM*);
	void queueScript(unsigned short event, AgentRef agent, AgentRef from, caosValue p0, caosValue p1);

	World();
	~World();
	void init();
	void initCatalogue();
	void shutdown();

	void executeInitScript(std::string p);
	void executeBootstrap(std::string p);
	void executeBootstrap(bool switcher);

	void newMoniker(std::shared_ptr<genomeFile> g, std::string genefile, AgentRef agent);
	std::shared_ptr<genomeFile> loadGenome(std::string& filename);
	std::string generateMoniker(std::string basename);

	int findCategory(unsigned char family, unsigned char genus, unsigned short species);

	void tick();
	void drawWorld();
	void drawWorld(class Camera* cam, RenderTarget* surface);

	int newUNID(Agent* whofor);
	void setUNID(Agent* whofor, int unid);
	void freeUNID(int unid);

	std::shared_ptr<Agent> lookupUNID(int unid);
};

extern World world;
#endif
/* vim: set noet: */
