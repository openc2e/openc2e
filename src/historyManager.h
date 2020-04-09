/*
 *  historyManager.h
 *  openc2e
 *
 *  Created by Alyssa Milburn on Wed 26 Apr 2006.
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

#ifndef _HISTORYMANAGER_H
#define _HISTORYMANAGER_H

#include "AgentRef.h"
#include "creatures/lifestage.h"
#include <memory>
#include <vector>
#include <map>

using std::shared_ptr;
using std::weak_ptr;

class genomeFile;

struct historyevent {
	unsigned int eventno;
	lifestage stage;
	std::string photo; // TODO: correct?
	std::string monikers[2];
	std::string networkid;
	unsigned int timestamp;
	int tage;
	std::string usertext;
	std::string worldname;
	std::string worldmoniker;
	unsigned int worldtick;
	historyevent(unsigned int eno, class CreatureAgent * = 0);
};

enum monikerstatus { referenced = 1, creature = 2, borncreature = 3, exported = 4, dead = 5, deadandkilled = 6, unreferenced = 7 };

class monikerData {
private:
	monikerstatus status;
	std::string moniker;

public:
	weak_ptr<genomeFile> genome;
	AgentRef owner;
	std::vector<historyevent> events;
	int gender;
	int variant;
	std::string name;
	unsigned char genus;
	bool warpveteran;
	unsigned int no_crossover_points, no_point_mutations;
	
	void init(std::string, shared_ptr<genomeFile>);
	historyevent &addEvent(unsigned int event, std::string moniker1 = "", std::string moniker2 = "");
	void moveToAgent(AgentRef a);
	void moveToCreature(AgentRef c);
	void wasBorn();
	void hasDied();
	monikerstatus getStatus();
};

class historyManager {
private:
	std::map<std::string, monikerData> monikers;

public:
	std::string newMoniker(shared_ptr<genomeFile>);
	bool hasMoniker(std::string);
	monikerData &getMoniker(std::string);
	std::string findMoniker(shared_ptr<genomeFile>);
	std::string findMoniker(AgentRef);
	void delMoniker(std::string);
};

#endif
/* vim: set noet: */
